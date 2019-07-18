#include "clientGame.h"
//--------------------------------------------------------------------------
void sendMessageToServer (int socketServer, char* message){
	unsigned int length;
	int bytesSend;
	length=strlen(message);
	bytesSend = send(socketServer, &length, sizeof(unsigned int), 0);
	int totalBytes=0;

		bytesSend = send(socketServer, message, length, 0);
		totalBytes += bytesSend;
}
//--------------------------------------------------------------------------
void receiveMessageFromServer (int socketServer, char* message){

	unsigned int length;
	int bytesRead;
	bytesRead = recv(socketServer, &length, sizeof(unsigned int), 0);
	if(bytesRead < 0)
		showError("ERROR message");
	int totalBytes=0;
	char* ptr = message;
	memset(ptr, 0, sizeof(tString));
	while(totalBytes < length){
		
		bytesRead = recv(socketServer, ptr, length, 0);
		totalBytes += bytesRead;
		ptr+= bytesRead;
	}
}
//--------------------------------------------------------------------------
void receiveBoard (int socketServer, tBoard board){
	int bytesRead;
	memset(board, 0, sizeof(tBoard));
	bytesRead = recv(socketServer, board, sizeof(tBoard), 0);
	if(bytesRead < 0)
		showError("ERROR board");

}
//---------------------------------------------------------------------------
unsigned int receiveCode (int socketServer){
	int bytesRead;
	unsigned int coda;
	bytesRead = recv(socketServer, &coda, sizeof(unsigned int), 0);
	if(bytesRead < 0)
		showError("ERROR code");	
	return coda;

}
//---------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------
void sendMoveToServer (int socketServer, unsigned int move){
	int bytesSend;
	bytesSend = send(socketServer, &move, sizeof(unsigned int), 0);

	if(bytesSend < 0)
		showError("ERROR sending move");
}

//--------------------------------------------------------------------------------

int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	unsigned int port;					/** Port number (server) */
	struct sockaddr_in server_address;			/** Server address structure */
	char* serverIP;						/** Server IP */

	tBoard board;						/** Board to be displayed */
	tString playerName;					/** Name of the player */
	tString rivalName;					/** Name of the rival */
	tString message;					/** Message received from server */
	unsigned int column;					/** Selected column */
	unsigned int code;					/** Code sent/receive to/from server */
	unsigned int endOfGame;					/** Flag to control the end of the game */
//---------------------------------------------------------------------------------------------------------
	int name1Length;					/** Length of the name player*/
	int name2Length;					/** Length of the name player (rival) */
	unsigned int move;



		// Check arguments!
		if (argc != 3){
			fprintf(stderr,"ERROR wrong number of arguments\n");
			fprintf(stderr,"Usage:\n$>%s serverIP port\n", argv[0]);
			exit(0);
		}

		// Get the server address
		serverIP = argv[1];

		// Get the port
		port = atoi(argv[2]);

		// Create socket-------------------------------------------------------------------
		socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


		// Check if the socket has been successfully created
		if (socketfd < 0)
			showError("ERROR while creating the socket");

		// Fill server address structure----------------------------------------------------
		memset(&server_address, 0, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = inet_addr(serverIP);
		server_address.sin_port = htons(port);

		// Connect with server--------------------------------------------------------------
		if (connect(socketfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
			showError("ERROR while establishing connection");
		printf ("Connection established with server!\n");

		// Init player's name
		do{
			memset(playerName, 0, STRING_LENGTH);
			printf ("Enter player name:");
			fgets(playerName, STRING_LENGTH-1, stdin);

			// Remove '\n'
			playerName[strlen(playerName)-1] = 0;

		}while (strlen(playerName) <= 2);


		// Send player's name to the server--------------------------------------------------
		name1Length = send(socketfd, playerName, STRING_LENGTH, 0); 	
		if (name1Length < 0) 
			showError("ERROR while writing to the socket");


		// Receive rival's name--------------------------------------------------------------
		memset(rivalName, 0, STRING_LENGTH);
		name2Length = recv(socketfd, rivalName, STRING_LENGTH, 0);


		printf ("You are playing against %s\n", rivalName);

		// Init
		endOfGame = FALSE;

		// Game starts
		printf ("Game starts!\n\n");

		// While game continues...
		while(!endOfGame){

			// Recive code,mesage,board	
			code = receiveCode(socketfd);
			receiveMessageFromServer (socketfd,message);
			receiveBoard (socketfd,board);

			//Print Board
			printBoard(board,message);
		
			//check endOfGame
			if ((code==GAMEOVER_WIN) || (code==GAMEOVER_DRAW)||(code==GAMEOVER_LOSE)){ 
				endOfGame=TRUE;
				
			//print message
			}
			else{	
				// Print Board				
				if (code==TURN_MOVE) {
					move=readMove();
					sendMoveToServer(socketfd,move);
				}
			}
		}// while

	// Close socket----------------------
	close(socketfd);
	return 0;

}//END


