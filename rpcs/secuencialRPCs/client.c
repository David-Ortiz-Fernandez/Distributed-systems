#include "client.h"


/** Timer interval */
#define TIMER_SEC 3

/** Flag to debug the client side */
#define DEBUG_CLIENT 1

/** Client handle */
CLIENT *clientPlayer;

//----------------------------------------------------------------------------------------V
void setTimer (){

	struct itimerval timer;

		// Set the timer
		timer.it_value.tv_sec = TIMER_SEC;
		timer.it_value.tv_usec = 0;
		timer.it_interval = timer.it_value;


		if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
			perror("Error while setting the timer\n");
			exit(1);
		}
}
//----------------------------------------------------------------------------------------V
void stopTimer (){

	struct itimerval timer;

		// Set the timer
		timer.it_value.tv_sec = 0;
		timer.it_value.tv_usec = 0;
		timer.it_interval = timer.it_value;


		if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
			perror("Error while stopping the timer\n");
			exit(1);
		}

}
//----------------------------------------------------------------------------------------V
void *alarmHandler (void){

	if (DEBUG_CLIENT)
		printf ("[Client] Timer()! -> Waking up...\n");
}
//----------------------------------------------------------------------------------------V
unsigned int readMove (){
	tString enteredMove;
	unsigned int move;
	unsigned int isRightMove;


		// Init...
		isRightMove = FALSE;
		move = STRING_LENGTH;

		while (!isRightMove){

			printf ("Enter a move [0-6]:");

			// Read move
			fgets (enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Length of entered move is not correct
			if (strlen(enteredMove) != 1){
				printf ("Entered move is not correct. It must be a number between [0-6]\n");
			}

			// Check if entered move is a number
			else if (isdigit(enteredMove[0])){

				// Convert move to an int
				move =  enteredMove[0] - '0';

				if (move > 6)
					printf ("Entered move is not correct. It must be a number between [0-6]\n");
				else
					isRightMove = TRUE;
			}

			// Entered move is not a number
			else
				printf ("Entered move is not correct. It must be a number between [0-6]\n");
		}

	return move;
}
//----------------------------------------------------------------------------------------V
void startGame(char *host){

	tMessage playerName;				/** Player name *///el nombre del jugador
	unsigned int endOfGame;				/** Flag to control the end of the game */
	tBlock *response;				/** Response from server *///el code el mensage y el board esta todo en este struct 
	int* resCode;					/** Result for registering the player *///resulado de registrar el usuario
	tColumn column;					/** Player move */ // habra que mirar en el server contiene el numero de la columna


		// Init...
		response = (tBlock*) malloc (sizeof(tBlock));
		resCode = (int*) malloc (sizeof (int));
		endOfGame = FALSE;
		*resCode = 0;

		// Create client handle---------------------------------------------------
		clientPlayer = clnt_create (host, CONECTA4, CONECTA4VER, "udp"); 

		// Check
 		if (clientPlayer == NULL) {
 			clnt_pcreateerror (host);
 			exit (1);
 			}


		// While player is not registered in the server side-----------------------
		while (*resCode != OK_NAME_REGISTERED){ //rescode es un puntero por eso se mira el contenido

		// Init player's name
			do{
				memset(playerName.msg, 0, 128);
				printf ("Enter player name:");
				fgets(playerName.msg, 127, stdin);
				// Remove '\n'
				playerName.msg[strlen(playerName.msg)-1] = 0;
			}while (strlen(playerName.msg) <= 2);
			
			
			resCode=registerplayer_1(&playerName, clientPlayer); //una vez registrao tenemos el nombre del jugador(primer argumento, 										el segundo es para conexion) /la funcion devuelve un puntero tb

		}

		// Player is successfully registered
		if (DEBUG_CLIENT)
			printf ("Player registered... setting up the alarm!\n");

		// Set alarm...
		if (signal(SIGALRM, (void (*)(int)) alarmHandler) == SIG_ERR) {
			perror("Error while setting a handler for SIGALRM");
			exit(1);
		}
		// While game continues...---------------------------------------------
		while (!endOfGame){
			//estado de la partida
			response = getgamestatus_1(&playerName, clientPlayer);
			printBoard (response->board, response->msg);			


			// Other player is thinking... waiting!
			if (response->code==TURN_WAIT){

				// Set timer to wake up from pause...
				setTimer ();

				// Player waits...
				pause();

				// Stop timer!
				stopTimer();
			}
			if ((response->code==GAMEOVER_WIN)||(response->code==GAMEOVER_DRAW)||(response->code==GAMEOVER_LOSE)){

				endOfGame=TRUE;
				
			}

			if (response->code==TURN_MOVE){
				
				///hay que rellenar la columna con dos campos el registro y pedir por pantalla 
				column.column=readMove();
				strcpy(column.player,playerName.msg);
			
				response=insertchipinboard_1(&column, clientPlayer);

			}

		}//while


	// Destroy the client handler
	clnt_destroy (clientPlayer);
}
//----------------------------------------------------------------------------------------
int main (int argc, char *argv[]){

	char *host;

		if (argc < 2) {
			printf ("usage: %s server_host\n", argv[0]);
			exit (1);
		}

		// Get host
		host = argv[1];

		// Start the game
		startGame (host);

	exit (0);
}
