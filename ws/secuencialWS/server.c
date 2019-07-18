#include "server.h"

/** Flag to enable debugging */
#define DEBUG_SERVER 1


/** Array with games */
tGame games[MAX_GAMES];

//--------------------------------------------------------------------------------------------------------------------------
void initServerStructures (){

	int i;

	if (DEBUG_SERVER)
		printf ("Initializing...\n");

	// Init seed
	srand (time(NULL));

	// Init each game
	for (i=0; i<MAX_GAMES; i++){

		// Allocate and init board
		games[i].board = (xsd__string) malloc (BOARD_WIDTH*BOARD_HEIGHT);
		initBoard (games[i].board);

		// Calculate the first player to play
		if ((rand()%2)==0)
			games[i].currentPlayer = player1;
		else
			games[i].currentPlayer = player2;

		// Allocate and init player names
		games[i].player1Name = (xsd__string) malloc (STRING_LENGTH);
		games[i].player2Name = (xsd__string) malloc (STRING_LENGTH);
		memset (games[i].player1Name, 0, STRING_LENGTH);
		memset (games[i].player2Name, 0, STRING_LENGTH);

		// Game status
		games[i].endOfGame = FALSE;
		games[i].status = gameEmpty;
	}
}
//--------------------------------------------------------------------------------------------------------------------------
conecta4ns__tPlayer switchPlayer (conecta4ns__tPlayer currentPlayer){

	conecta4ns__tPlayer nextPlayer;

		if (currentPlayer == player1)
			nextPlayer = player2;
		else
			nextPlayer = player1;

	return nextPlayer;
}
//--------------------------------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------------------------------
int locateGameForPlayer (xsd__string name){
	int index=-1,i=0;
	while((i< MAX_GAMES) && (index==-1)) {
		if(strcmp(name, games[i].player1Name) == 0 || strcmp(games[i].player2Name, name) == 0){
			index=i;
		}
		i++;
	}
return index;
}
//--------------------------------------------------------------------------------------------------------------------------
void freeGameByIndex (int index){
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

//--------------------------------------------------------------------------------------------------------------------------
int conecta4ns__register (struct soap *soap, conecta4ns__tMessage playerName, int *code){

	int gameIndex;
	
		// Init...
		gameIndex = -1;

		// Search fon an empty game-----------------------------------------------------
		*code=searchEmptyGame ();
		
		if(*code == ERROR_SERVER_FULL)
			printf("Todas las partidas estan llenas");
		else{
			gameIndex = *code;
			
			if(locateGameForPlayer(playerName.msg)!=-1)
					*code= ERROR_NAME_REPEATED;	
			else{
				*code = OK_NAME_REGISTERED;

				if (games[gameIndex].status == gameEmpty){ 
					playerName.msg[playerName.__size] = 0;
					strcpy(games[gameIndex].player1Name, playerName.msg);
					games[gameIndex].status = gameWaitingPlayer;
				}
				else {
					if(games[gameIndex].status == gameWaitingPlayer){
						playerName.msg[playerName.__size] = 0;
						strcpy(games[gameIndex].player2Name , playerName.msg);
						games[gameIndex].status = gameReady;
					}
				}
			}
		}
  return SOAP_OK;
}
//--------------------------------------------------------------------------------------------------------------------------
int conecta4ns__getStatus (struct soap *soap, conecta4ns__tMessage playerName, conecta4ns__tBlock* status){

	int gameIndex;
	char messageToPlayer [STRING_LENGTH];

		// Set \0 at the end of the string
		playerName.msg[playerName.__size] = 0;

		// Allocate memory for the status structure
		(status->msgStruct).msg = (xsd__string) malloc (STRING_LENGTH);
		memset ((status->msgStruct).msg, 0, STRING_LENGTH);
		status->board = (xsd__string) malloc (BOARD_WIDTH*BOARD_HEIGHT);
		memset (status->board, 0, BOARD_WIDTH*BOARD_HEIGHT);
		status->__size = BOARD_WIDTH*BOARD_HEIGHT;

	
		if (DEBUG_SERVER)
			printf ("Receiving getStatus() request from -> %s [%d]\n", playerName.msg, playerName.__size);
	
	

		// Locate the game-------------------------------------------------------
		gameIndex = locateGameForPlayer (playerName.msg);
		
		if(gameIndex==-1)
			status->code=ERROR_PLAYER_NOT_FOUND;
		
		if(games[gameIndex].status==gameWaitingPlayer && games[gameIndex].endOfGame==FALSE){
			status->code = TURN_WAIT;
			status->msgStruct.__size=strlen("Waiting rival...");
			strcpy(status->msgStruct.msg,"Waiting rival...");
			status->msgStruct.msg[status->msgStruct.__size] = 0;
			strcpy(status->board,games[gameIndex].board);
			status->board[status->__size] = 0;
		}
		else{
		
			if(games[gameIndex].endOfGame==TRUE){
				
				if(isBoardFull (games[gameIndex].board)==TRUE){
					status->code = GAMEOVER_DRAW;
					status->msgStruct.__size=strlen("Draw");
					strcpy(status->msgStruct.msg,"Draw");
					status->msgStruct.msg[status->msgStruct.__size] = 0;
				}
				if(checkWinner (games[gameIndex].board, games[gameIndex].currentPlayer) == TRUE){
					status->code = GAMEOVER_WIN;
					status->msgStruct.__size=strlen("You Win");
					strcpy(status->msgStruct.msg,"You Win");
					status->msgStruct.msg[status->msgStruct.__size] = 0;
				} 
				if(checkWinner (games[gameIndex].board, games[gameIndex].currentPlayer) == FALSE){
					status->code = GAMEOVER_LOSE;
					status->msgStruct.__size=strlen("You Lose");
					strcpy(status->msgStruct.msg,"You Lose");
					status->msgStruct.msg[status->msgStruct.__size] = 0;
				
				}
				strcpy(status->board,games[gameIndex].board);
				status->board[status->__size] = 0;

				if(games[gameIndex].status==gameWaitingPlayer)
					freeGameByIndex(gameIndex);
					

				else {
					games[gameIndex].status=gameWaitingPlayer;
					games[gameIndex].currentPlayer= switchPlayer(games[gameIndex].currentPlayer);
				}
			
					
			}
			else{
				if(games[gameIndex].currentPlayer==player1){
					if(strcmp(games[gameIndex].player1Name, playerName.msg)){
						status->code = TURN_MOVE;
						status->msgStruct.__size=strlen("Its your turn. You play with:o");
						strcpy(status->msgStruct.msg,"Its your turn. You play with:o");
						status->msgStruct.msg[status->msgStruct.__size] = 0;
						}
					
					if(strcmp(games[gameIndex].player2Name, playerName.msg)){
						
						status->code = TURN_WAIT;
						status->msgStruct.__size=strlen("Your rival is thinking... please, wait! You play with:x");
						strcpy(status->msgStruct.msg,"Your rival is thinking... please, wait! You play with:x");
						status->msgStruct.msg[status->msgStruct.__size] = 0;
					}

					
				}
				else{//currentPlayer= player2
					if(strcmp(games[gameIndex].player1Name, playerName.msg)){
						status->code = TURN_WAIT;
						status->msgStruct.__size=strlen("Your rival is thinking... please, wait! You play with:o");
						strcpy(status->msgStruct.msg,"Your rival is thinking... please, wait! You play with:o");
						status->msgStruct.msg[status->msgStruct.__size] = 0;
					}
					
					if(strcmp(games[gameIndex].player2Name, playerName.msg)){
						status->code = TURN_MOVE;
						status->msgStruct.__size=strlen("Its your turn. You play with:x");
						strcpy(status->msgStruct.msg,"Its your turn. You play with:x");
						status->msgStruct.msg[status->msgStruct.__size] = 0;
					}

				}
				strcpy(status->board,games[gameIndex].board);
				status->board[status->__size] = 0;	
			}
		}
		
	return SOAP_OK;
}
//--------------------------------------------------------------------------------------------------------------------------
int conecta4ns__insertChip (struct soap *soap, conecta4ns__tMessage playerName, int playerMove, conecta4ns__tBlock* status){

	int gameIndex;
	conecta4ns__tMove moveResult;


		// Set \0 at the end of the string
		playerName.msg[playerName.__size] = 0;

		// Allocate memory for the status structure
		(status->msgStruct).msg = (xsd__string) malloc (STRING_LENGTH);
		memset ((status->msgStruct).msg, 0, STRING_LENGTH);
		status->board = (xsd__string) malloc (BOARD_WIDTH*BOARD_HEIGHT);
		memset (status->board, 0, BOARD_WIDTH*BOARD_HEIGHT);
		
				


		// Locate the game
		gameIndex = locateGameForPlayer (playerName.msg);
		
		if(gameIndex==-1)
			status->code=ERROR_PLAYER_NOT_FOUND;
			
		if(checkMove(games[gameIndex].board, playerMove) == OK_move){
			
			insertChip(games[gameIndex].board, games[gameIndex].currentPlayer, playerMove);
			
			if(checkWinner (games[gameIndex].board, games[gameIndex].currentPlayer)==TRUE){
				games[gameIndex].endOfGame=TRUE;
				
			}
				
				
			if(games[gameIndex].currentPlayer==player1){			
						status->code = TURN_WAIT;
						status->msgStruct.__size=strlen("Your rival is thinking... please, wait! You play with:o");
						strcpy(status->msgStruct.msg,"Your rival is thinking... please, wait! You play with:o");
						status->msgStruct.msg[status->msgStruct.__size] = 0;
	
			}
			else{
					status->code = TURN_WAIT;
					status->msgStruct.__size=strlen("Your rival is thinking... please, wait! You play with:x");
					strcpy(status->msgStruct.msg,"Your rival is thinking... please, wait! You play with:x");
					status->msgStruct.msg[status->msgStruct.__size] = 0;
			}
			strcpy(status->board,games[gameIndex].board);
			status->board[status->__size] = 0;	
			
			if(!games[gameIndex].endOfGame)	
				games[gameIndex].currentPlayer= switchPlayer(games[gameIndex].currentPlayer);
			
		}

		

	return SOAP_OK;
}
//--------------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv){ 

  int primarySocket, secondarySocket;
  struct soap soap;

		if (argc < 2) {
			printf("Usage: %s <port>\n",argv[0]);
			exit(-1);
		}

		// Init structures
		initServerStructures();

		// Init environment
		soap_init(&soap);

		// Bind to the specified port
		primarySocket = soap_bind(&soap, NULL, atoi(argv[1]), 100);

		// Check result of binding
		if (primarySocket < 0) {
			soap_print_fault(&soap, stderr); exit(-1); 
		}

		printf ("Server is ON! waiting for requests...\n");

		// Listen to next connection
		while (TRUE) {
			// accept
			secondarySocket = soap_accept(&soap);    

			if (secondarySocket < 0) {
				soap_print_fault(&soap, stderr); exit(-1);
			}

			// Execute invoked operation
			soap_serve(&soap);

			// Clean up!
			soap_end(&soap);
		}

  return 0;
}
