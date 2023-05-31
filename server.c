//ShipsGame client program by Anna Giszczak and Barbara Chytła
//Server base taken from GeeksForGeeks socket programming example code
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 
#include <time.h>
	
#define TRUE 1
#define FALSE 0
#define PORT 8888

void delay(int number_of_sec){
	int mili_sec = 1000*number_of_sec;
	clock_t start_time = clock();
	while(clock()<start_time + mili_sec);
}

//MESSAGE variables
char *HELLO_MESS = "Welcome to the game, please set your ships\n";
char *GAME_MESS = "Game is starting\n";		
char *YOUT_MESS = "Your turn\n";

//ictory messages
char *WINP0_MESS = "Player 0 won\n";
char *WINP1_MESS = "Player 1 won\n";
char *WIN_MESS = "You won\n";
char *LOSE_MESS = "You lose\n";

char send_map[1024] = "";
char int_var[10]="";
char send_mess[1024]="";
char trash[10]="";

typedef struct Ships {
	int map[10][10];
	int n;
	int three[3][2];
	int two[2][2];
	int one[1][2];
	int shoot_map[10][10]; //map
} Ships;

void SetShips(int sd, Ships *player){
	memset(player->map, 0, sizeof(player->map));
	memset(player->shoot_map, 0, sizeof(player->shoot_map));
	player->n = 6;
	int x=0, y=0, rc;
	char bufferx[3];
	char buffery[3];
	char *SHIP_MESS[]={"\nSet your three-mast ships\n", "\nSet your two-mast ships\n", "\nSet your one-mast ships\n"};
	//sends SHIP MESSAGE
	if(send(sd, SHIP_MESS[0], strlen(SHIP_MESS[0]), 0)<0) puts("error, cannot send data\n");
	for(int i = 0; i<3; i++){
		puts("I am waiting for x, y coordinates\n");
		if(read(sd, bufferx, 3)<0) puts("error, cannot read data\n");
		else puts("I have read x coordinate\n");
		x = atoi(bufferx);
		memset(bufferx, '\0', sizeof(bufferx));
		if(read(sd, buffery, 3)<0) puts("error, cannot read data\n");
		else puts("I have read y coordinate\n");
		y = atoi(buffery);
		memset(buffery, '\0', sizeof(buffery));
		printf("x: %d y: %d\n", x, y);
		player->map[x][y]=1;
		player->three[i][0]=x;
		player->three[i][1]=y;
	}
	if(send(sd, SHIP_MESS[1], strlen(SHIP_MESS[1]), 0)<0) printf("error, cannot send data\n");
	for(int i = 0; i<2; i++){
		puts("I am waiting for x, y coordinates\n");
		if(read(sd, bufferx, 3)<0) puts("error, cannot read data\n");
		else puts("I have read x coordinate\n");
		x = atoi(bufferx);
		memset(bufferx, '\0', sizeof(bufferx));
		if(read(sd, buffery, 3)<0) puts("error, cannot read data\n");
		else puts("I have read y coordinate\n");
		y = atoi(buffery);
		memset(buffery, '\0', sizeof(buffery));
		printf("x: %d y: %d\n", x, y);
		player->map[x][y]=1;
		player->two[i][0]=x;
		player->two[i][1]=y;
	}
	if(send(sd, SHIP_MESS[2], strlen(SHIP_MESS[2]), 0)<0) printf("error, cannot send data\n");
	for(int i = 0; i<1; i++){
		puts("I am waiting for x, y coordinates\n");
		if(read(sd, bufferx, 3)<0) puts("error, cannot read data\n");
		else puts("I have read x coordinate\n");
		x = atoi(bufferx);
		memset(bufferx, '\0', sizeof(bufferx));
		if(read(sd, buffery, 3)<0) puts("error, cannot read data\n");
		else puts("I have read y coordinate\n");
		y = atoi(buffery);
		memset(buffery, '\0', sizeof(buffery));
		printf("x: %d y: %d\n", x, y);
		player->map[x][y]=1;
		player->one[i][0]=x;
		player->one[i][1]=y;
	}
	for(int j = 0;j<10;j++){
		for(int i=0;i<10;i++){
			printf(" %d ",player->map[j][i]);

		}
		puts("\n");
	}
}

void Shoot(int sd, Ships *player){

	int x=0, y=0, rc;
	char bufferx[3];
	char buffery[3];
	char *HIT_MESS = "Well done! Shoot, once again\n";
	char *MISS_MESS = "Miss, Your opponent's turn\n";
	char *LASTHIT_MESS = "The last ship sunk\n";
	while(1){
		if(send(sd, YOUT_MESS, strlen(YOUT_MESS)+1, 0)<0) puts("error, cannot send data\n");
		if(read(sd, bufferx, 3)<0) puts("Cannot send data");
		else send(sd, "ok", strlen("ok")+1, 0);
		x = atoi(bufferx);
		memset(bufferx, '\0', sizeof(bufferx));
		if(read(sd, buffery, 3)<0) puts("Cannot send data");
		y = atoi(buffery);
		memset(buffery, '\0', sizeof(buffery));
		printf("x: %d y: %d\n", x, y);

		if(player->map[x][y]==1){ //1 - there is a ship
			player->map[x][y]=2; //2 - mark as hit
			player->shoot_map[x][y]=1; //mark opponent's ship as hit
			player->n--; 
			if(player->n==0){
				send(sd,LASTHIT_MESS, strlen(LASTHIT_MESS), 0);
				puts("Hit and sunk last ship\n");
				break;
			}
			puts("Hit");
			memset(send_map, '\0', sizeof(send_map));
			memset(send_mess, '\0', sizeof(send_map));
			puts("Sending map to client\n"); 
			for(int j=0;j<10;j++){
				for(int i=0;i<10;i++){
					memset(int_var, '\0', sizeof(int_var));
					sprintf(int_var, "%d", player->shoot_map[j][i]);
					strcat(send_map, int_var);
					strcat(send_map, " ");
				}
				strcat(send_map, "\n");
			}
			strcat(send_map, HIT_MESS);
			strcat(send_mess, send_map);
			
			if(send(sd,send_mess, strlen(send_mess)+1, 0)<0) puts("error, cannot send data\n");
			else read(sd, trash, sizeof(trash));
			printf("%s\n", send_map);

		} else if(player->map[x][y]==0){ //0 - no ship in that place
			player->shoot_map[x][y]=-1; //mark as missed
			memset(send_map, '\0', sizeof(send_map));
			memset(send_mess, '\0', sizeof(send_map));
			for(int j=0;j<10;j++){
				for(int i=0;i<10;i++){
					memset(int_var, '\0', sizeof(int_var));
					sprintf(int_var, "%d", player->shoot_map[j][i]);
					strcat(send_map, int_var);
					strcat(send_map, " ");
				}
				strcat(send_map, "\n");
			}
			strcat(send_map, MISS_MESS);
			strcat(send_mess, send_map);
			if(send(sd,send_map, strlen(send_map)+1, 0)<0) puts("error, cannot send data\n");
			else read(sd, trash, sizeof(trash));
			break;
		}
	}
	puts("Shoot function ended\n");
}




int main(int argc , char *argv[])
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[2] ,
		max_clients = 2 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
	Ships player[2] = {0};
		
	char buffer[1025]; 
		
	//set of socket descriptors
	fd_set readfds;
		
	//connect message
	char *message = "connected \r\n";
	
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
	
	//set master socket to allow multiple connections
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons( PORT );
		
	//bind the socket 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);
		
	//maximum of 2 pending connections 
	if (listen(master_socket, 4) < 0)
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
				
			//highest file descriptor number
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//wait for an activity on one of the sockets
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
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}

		//game code
		if(client_socket[0] != 0 && client_socket[1] != 0){
			
			puts("Game is starting\n");
			for(int i = 0; i < max_clients; i++){
				send(client_socket[i], HELLO_MESS, strlen(HELLO_MESS)+1, 0);
				SetShips(client_socket[i], &player[i]);
			}
			
			for(int i = 0; i < max_clients; i++){
					send(client_socket[i], GAME_MESS, strlen(GAME_MESS), 0);
			}

			
			while(1){
				for(int i = 0; i < max_clients; i++){
					Shoot(client_socket[i], &player[(i+1)%2]);
					if(player[(i+1)%2].n == 0) break;
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
			getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);
			printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
			for (i = 0; i < max_clients; i++){
				sd = client_socket[i];
				close(sd);
				client_socket[i] = 0;
			}
		}
	}
		
	return 0;
}
