#include "server.h"

/** Flag to enable debugging */
#define DEBUG_SERVER 1


/** Array with games */
tGame games[MAX_GAMES];

//----------------------------------------------------------------------------------------
void initServerStructures (){

	int i;

		if (DEBUG_SERVER)
			printf ("\nServer is on!!!Initializing...\n");

		srand (time(NULL));

		// Init each game
		for (i=0; i<MAX_GAMES; i++){

			initBoard (games[i].board);

			if ((rand()%2)==0)
				games[i].currentPlayer = player1;
			else
				games[i].currentPlayer = player2;

			memset (games[i].player1Name, 0, STRING_LENGTH);
			memset (games[i].player2Name, 0, STRING_LENGTH);
			games[i].endOfGame = FALSE;
			games[i].status = gameEmpty;
		}
}
//----------------------------------------------------------------------------------------
tPlayer switchPlayer (tPlayer currentPlayer){

	tPlayer nextPlayer;

		if (currentPlayer == player1)
			nextPlayer = player2;
		else
			nextPlayer = player1;

	return nextPlayer;
}
//----------------------------------------------------------------------------------------
int searchEmptyGame (){

	int i, found, result;

		// Init...
		found = FALSE;
		i = 0;
		result = -1;

		// Search for a non-active game
		while ((!found) && (i<MAX_GAMES)){

			if ((games[i].status == gameEmpty) ||
				(games[i].status == gameWaitingPlayer)){
				found = TRUE;
			}
			else
				i++;
		}

		if (found){
			result = i;
		}
		else{
			result = ERROR_SERVER_FULL;
		}

	return result;
}
//----------------------------------------------------------------------------------------
int locateGameForPlayer (tString name){
	int index=-1,i=0;
	while((i< MAX_GAMES) && (index==-1)) {
		if(strcmp(name, games[i].player1Name) == 0 || strcmp(games[i].player2Name, name) == 0){
			index=i;
		}
		i++;
	}
return index;
}
//----------------------------------------------------------------------------------------
void initGameByIndex (int index){
	initBoard (games[index].board);

	if ((rand()%2)==0)
		games[index].currentPlayer = player1;
	else
		games[index].currentPlayer = player2;

	memset (games[index].player1Name, 0, STRING_LENGTH);
	memset (games[index].player2Name, 0, STRING_LENGTH);
	games[index].endOfGame = FALSE;
	games[index].status = gameEmpty;


}
//------------------------------------------------------------------------------------PAG14----
int *registerplayer_1_svc(tMessage *argMsg, struct svc_req *rqstp){
	
	static int  result;
	int gameIndex;
	
		// Init...
		gameIndex = -1;

		// Search fon an empty game-----------------------------------------------------
		result=searchEmptyGame ();
		
		if(result == ERROR_SERVER_FULL)
			printf("Todas las partidas estan llenas");
		else{
			gameIndex = result;
			
			if(locateGameForPlayer(argMsg->msg)!=-1)
					result= ERROR_NAME_REPEATED;	
			else{
				result = OK_NAME_REGISTERED;

				if (games[gameIndex].status == gameEmpty){ 
					strcpy(games[gameIndex].player1Name, argMsg->msg);
					games[gameIndex].status = gameWaitingPlayer;
				}
				else {
					if(games[gameIndex].status == gameWaitingPlayer){
						strcpy(games[gameIndex].player2Name , argMsg->msg);
						games[gameIndex].status = gameReady;
					}
				}
			}
		}	

	return &result;
}
//-----------------------------------------------------------------------------------PAG14-----
tBlock *getgamestatus_1_svc(tMessage *argMsg, struct svc_req *rqstp){

	tBlock *response;
	int gameIndex;
		
		// Init...
		response = (tBlock*) malloc (sizeof (tBlock));

		// Locate the game-------------------------------------------------------
		gameIndex = locateGameForPlayer (argMsg->msg);
		
		if(gameIndex==-1)
			response->code=ERROR_PLAYER_NOT_FOUND;
		
		if(games[gameIndex].status==gameWaitingPlayer && games[gameIndex].endOfGame==FALSE){
			response->code = TURN_WAIT;
			strcpy(response->msg,"Waiting rival...");
			strcpy(response->board,games[gameIndex].board);
		}
		else{
		
			if(games[gameIndex].endOfGame==TRUE){
				
				if(isBoardFull (games[gameIndex].board)==TRUE){
					response->code = GAMEOVER_DRAW;
					strcpy(response->msg,"Draw");
				}
				if(checkWinner (games[gameIndex].board, argMsg->msg) == TRUE){
					response->code = GAMEOVER_WIN;
					strcpy(response->msg,"You Win");
				} 
				if(checkWinner (games[gameIndex].board, argMsg->msg) == FALSE){
					response->code = GAMEOVER_LOSE;
					strcpy(response->msg,"You Lose");
				
				}
				strcpy(response->board,games[gameIndex].board);

				if(games[gameIndex].status==gameWaitingPlayer)
					initGameByIndex(gameIndex);
					

				else {
					games[gameIndex].status=gameWaitingPlayer;
					games[gameIndex].currentPlayer= switchPlayer(games[gameIndex].currentPlayer);
				}

					
				
					
			}
			else{
				if(games[gameIndex].currentPlayer==player1){
					if(strcmp(games[gameIndex].player1Name, argMsg->msg)){
						response->code = TURN_MOVE;
						strcpy(response->msg,"Its your turn. You play with:o");
						}
					
					if(strcmp(games[gameIndex].player2Name, argMsg->msg)){
						response->code = TURN_WAIT;
						strcpy(response->msg,"Your rival is thinking... please, wait! You play with:x");
					}
					
				}
				else{//currentPlayer= player2
					if(strcmp(games[gameIndex].player1Name, argMsg->msg)){
						response->code = TURN_WAIT;
						strcpy(response->msg,"Your rival is thinking... please, wait! You play with:o");
					}
					
					if(strcmp(games[gameIndex].player2Name, argMsg->msg)){
						response->code = TURN_MOVE;
						strcpy(response->msg,"Its your turn. You play with:x");
					}	
				}
				strcpy(response->board,games[gameIndex].board);
			}
		}
		
		


	return response;
}
//----------------------------------------------------------------------------------------
tBlock * insertchipinboard_1_svc(tColumn *argCol, struct svc_req *rqstp){

	tMove moveResult;
	tBlock *response;
	int gameIndex;
		// Init...
		response = (tBlock*) malloc (sizeof (tBlock));

		// Locate the game
		gameIndex = locateGameForPlayer (argCol->player);
		
		if(gameIndex==-1)
			response->code=ERROR_PLAYER_NOT_FOUND;
			
		if(checkMove(games[gameIndex].board, argCol->column) == OK_move){
			
			insertChip(games[gameIndex].board, games[gameIndex].currentPlayer, argCol->column);
			
			if(checkWinner (games[gameIndex].board, games[gameIndex].currentPlayer)==TRUE){
				games[gameIndex].endOfGame=TRUE;
				
			}
				
				
			if(games[gameIndex].currentPlayer==player1){			
						response->code = TURN_WAIT;
						strcpy(response->msg,"Your rival is thinking... please, wait! You play with:o");	
			}
			else{
					response->code = TURN_WAIT;
					strcpy(response->msg,"Your rival is thinking... please, wait! You play with:x");
			}
			strcpy(response->board,games[gameIndex].board);	
			
			if(	!games[gameIndex].endOfGame)
				games[gameIndex].currentPlayer= switchPlayer(games[gameIndex].currentPlayer);
			
		}
		
//-------------------------------------------------------------------------


	return response;
}
