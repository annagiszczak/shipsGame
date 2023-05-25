//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <iostream>
#include <string>
#include <cstdio>
#include <cstring> //strlen
#include <cstdlib>
#include <cerrno>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <ctime>
	
#define TRUE 1
#define FALSE 0
#define PORT 8888

void delay(int number_of_sec){
	int mili_sec = 1000*number_of_sec;
	clock_t start_time = clock();
	while(clock()<start_time + mili_sec);
}

///Globalne zmienne MESSAGE
char *HELLO_MESS = "Hello in the game, please set your ships\n";
char *GAME_MESS = "Game is starting\n";		
char *YOUT_MESS = "Your turn\n";

//Victory
char *WINP0_MESS = "Player 0 won\n";
char *WINP1_MESS = "Player 1 won\n";
char *WIN_MESS = "You won\n";
char *LOSE_MESS = "You lose\n";



typedef struct Ships {
int map[10][10];
int n;
} Ships;

void SetShips(int sd, Ships *player){
	memset(player->map, 0, sizeof(player->map));
	player->n = 3;   ///jak zmieniamy statki to tu i u graczy
	int x=0, y=0, rc;
	char bufferx[3];
	char buffery[3];
	for(int i = 0; i < player->n; i++){
		rc = read(sd, bufferx, 3);
		if(rc<0){
			perror("cannot send data");
		}
		x = atoi(bufferx);
		memset(bufferx, '\0', sizeof(bufferx));
		// delay(1);
		rc = read(sd, buffery, 3);
		if(rc<0){
			perror("cannot send data");
		}
		y = atoi(buffery);
		memset(buffery, '\0', sizeof(buffery));
		printf("x: %d y: %d\n", x, y);
		player->map[x][y]=1;
	}
	for(int j = 0;j<10;j++){
		for(int i=0;i<10;i++){
			printf(" %d ",player->map[j][i]);

		}
		puts("\n");
	}
}

void SendMap(int sd, Ships *player){
	char buffer[10];
	for(int j = 0;j<10;j++){
		for(int i=0;i<10;i++){
			printf(" %d ",player->map[j][i]);
			sprintf(buffer, "%d", player->map[i][j]);
			send(sd,buffer, strlen(buffer), 0);
		}
		puts("\n");
	}
	// char buffer[10];
	// for(int i = 0; i < 10; i++){
	// 	for(int j = 0; j < 10; j++){
	// 		sprintf(buffer, "%d", player->map[i][j]);
	// 		write(sd, buffer, strlen(buffer));
	// 	}
	// }
}

// void MissOrHit(int sd, int x, int y, Ships *player){
// 	if(player->map[x][y] == 1){
// 		player->map[x][y] = 2;
// 		write(sd, "HIT", 3);
// 		n--;
// 	}
// 	else{
// 		player->map[x][y] = 3;
// 		write(sd, "MISS", 4);
// 	}
// }

void Shoot(int sd, Ships *player){

	int x=0, y=0, rc;
	char bufferx[3];
	char buffery[3];
	char *HIT_MESS = "Good. Shoot, once again\n";
	char *MISS_MESS = "Miss, Your opponent's turn\n";
	char *LASTHIT_MESS = "The last ship sunk\n";
	while(1){
		rc = read(sd, bufferx, 3);
		if(rc<0){
			perror("Cannot send data");
		}
		x = atoi(bufferx);
		memset(bufferx, '\0', sizeof(bufferx));
		rc = read(sd, buffery, 3);
		if(rc<0){
			perror("Cannot send data");
		}
		y = atoi(buffery);
		memset(buffery, '\0', sizeof(buffery));
		printf("x: %d y: %d\n", x, y);

		if(player->map[x][y]==1){
			player->map[x][y]=2;
			player->n--;
			if(player->n==0){
				send(sd,LASTHIT_MESS, strlen(LASTHIT_MESS), 0);
				puts("siedze w ifie ostatni statek");
				break;
			}
			puts("siedze w ifie obok wiadomosci HIT_message");
			send(sd,HIT_MESS, strlen(HIT_MESS), 0);
		} else if(player->map[x][y]==0){
			send(sd,MISS_MESS, strlen(MISS_MESS), 0);
			break;
		}
	}
	puts("wyszedlem z petli");
}

int main(int argc , char *argv[])
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[2] ,
		max_clients = 2 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
	Ships player[2] = {0};
		
	char buffer[1025]; //data buffer of 1K
		
	//set of socket descriptors
	fd_set readfds;
		
	//a message
	char *message = "ECHO Daemon v1.0 \r\n";
	
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("Socket failed");
		exit(EXIT_FAILURE);
	}
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons( PORT );
		
	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 2) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
		
	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
		
			//send new connection greeting message
			if( send(new_socket, message, strlen(message), 0) != strlen(message) )
			{
				perror("send");
			}
				
			puts("Welcome message sent successfully");
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}

		//Odtąd będziemy korygować kod
		
		if(client_socket[0] != 0 && client_socket[1] != 0){
			
			puts("Game is starting\n");
			// memset(message, '\0', sizeof(&message));
			for(int i = 0; i < max_clients; i++){
				// printf("%d\n", i);
				send(client_socket[i], HELLO_MESS, strlen(HELLO_MESS), 0);
				SetShips(client_socket[i], &player[i]);
			}
			
			// memset(message, '\0', strlen(message));
			
			for(int i = 0; i < max_clients; i++){
					send(client_socket[i], GAME_MESS, strlen(GAME_MESS), 0);
			}

			
			while(1){
				for(int i = 0; i < max_clients; i++){
					send(client_socket[i], YOUT_MESS, strlen(YOUT_MESS), 0);
					Shoot(client_socket[i], &player[(i+1)%2]);
					if(player[(i+1)%2].n == 0) break;
					// message = "Wait for your turn\n";
					// send(client_socket[i], message, strlen(message), 0);
				}
				if(player[0].n == 0){
					send(client_socket[0], WINP1_MESS, strlen(WINP0_MESS), 0);
					send(client_socket[1], WIN_MESS, strlen(WIN_MESS), 0);
					send(client_socket[0], LOSE_MESS, strlen(LOSE_MESS), 0);
					break;
				}else if(player[1].n == 0){
					send(client_socket[1], WINP0_MESS, strlen(WINP1_MESS), 0);
					send(client_socket[0], WIN_MESS, strlen(WIN_MESS), 0);
					send(client_socket[1], LOSE_MESS, strlen(LOSE_MESS), 0);
					break;
				}
			}
			
			
			//close connections
			// for (i = 0; i < max_clients; i++){
			// 	sd = client_socket[i];
			// 	close(sd);
			// 	client_socket[i] = 0;
			// }
		// }*/
		}
		


		/* //To jest fajny kod do sprawdzania czy sie polaczyl 
		//uzytkownik i go wywala lub odczytuje jego wiad jednak
		//nie wiem na razie jak to do nas zaimplementowac


		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}
					
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
					send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}
		*/ 
	}
		
	return 0;
}