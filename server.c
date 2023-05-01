//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
	
#define TRUE 1
#define FALSE 0
#define PORT 8888

typedef struct Ships {
int map[10][10];
} Ships;

void SetShips(int sd, Ships *player){
	int s = 0;
	int x=0, y=0;
	char buffer[10];
	while(s < 1)
	{
		read(sd, buffer, 10);
		// scanf("%d %d",x,y);
		sprintf(buffer, "%d %d", x, y);
		player->map[x][y] = 1;
		s++;
	}
}

void SendMap(int sd, Ships *player){
	char buffer[10];
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++){
			sprintf(buffer, "%d", player->map[i][j]);
			write(sd, buffer, strlen(buffer));
		}
	}
}

void MissOrHit(int sd, int x, int y, Ships *player){
	if(player->map[x][y] == 1){
		player->map[x][y] = 2;
		write(sd, "HIT", 3);
	}
	else{
		player->map[x][y] = 3;
		write(sd, "MISS", 4);
	}
}

void Shoot(int sd, Ships *player){
	int x=0, y=0;
	char buffer[10];
	read(sd, buffer, 10);
	sprintf(buffer, "%d %d", x, y);
	MissOrHit(sd, x, y, player);
}

int NumberOfShips(Ships *player){
	int c = 0;

	for(int i=0; i < 10; i++)
		for(int j=0; j < 10; j++){
			if(player->map[i][j] == 1)
			c++;
		}

	return c;
}



int main(int argc , char *argv[])
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[2] ,
		max_clients = 2 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
		
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
		perror("socket failed");
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
			Ships player[2] = {0};
			puts("Game is starting");
			message = "Hello in the game, please set your ships\n";
			for(int i = 0; i < max_clients; i++){
				send(client_socket[i], message, strlen(message), 0);
				SetShips(client_socket[i], &player[i]);
			}
			message = "Game is starting\n";
			for(int i = 0; i < max_clients; i++){
					send(client_socket[i], message, strlen(message), 0);
			}

			while(1){
				for(int i = 0; i < max_clients; i++){
					message = "Your turn\n";
					send(client_socket[i], message, strlen(message), 0);
					Shoot(client_socket[i], &player[(i+1)%2]);
					message = "Wait for your turn\n";
					send(client_socket[i], message, strlen(message), 0);
				}
				if(NumberOfShips(&player[0]) == 0){
					message = "Player 1 won\n";
					send(client_socket[0], message, strlen(message), 0);
					message = "You won\n";
					send(client_socket[1], message, strlen(message), 0);
					message = "You lost\n";
					send(client_socket[0], message, strlen(message), 0);
					break;
				}
				if(NumberOfShips(&player[1]) == 0){
					message = "Player 0 won\n";
					send(client_socket[1], message, strlen(message), 0);
					message = "You won\n";
					send(client_socket[0], message, strlen(message), 0);
					message = "You lost\n";
					send(client_socket[1], message, strlen(message), 0);
					break;
				}

			}
			//close connections
			for (i = 0; i < max_clients; i++){
				sd = client_socket[i];
				close(sd);
			}
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
