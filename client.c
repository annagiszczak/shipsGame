//ShipsGame client program by Anna Giszczak and Barbara Chytła
//Client base taken from W. Bajdecki

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include <time.h>

int n = 6;

void delay(int number_of_sec){
	int mili_sec = 1000*number_of_sec;
	clock_t start_time = clock();
	while(clock()<start_time + mili_sec);
}

int main (int argc, char *argv[]) { 

  const int SERVER_PORT=8888;

  int sd, rc;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  char buffer[1024]="";
  char buffer2[1024]="";
  char shoot_map[1024]="";
  char trash[10]="";

  
  if(argc < 2) {
    printf("usage:  %s <server> \n",argv[0]);
    exit(1);
  }

  h = gethostbyname(argv[1]); 
  if(h==NULL) {
    printf("%s: unknown host '%s'\n",argv[0],argv[1]);
    exit(1);
  }

  servAddr.sin_family = h->h_addrtype;
  memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons(SERVER_PORT);

  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd<0) {
    perror("cannot open socket ");
    exit(1);
  }

  /* bind any port number */
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);
  
  rc = bind(sd, (struct sockaddr *) &localAddr, sizeof(localAddr));
  if(rc<0) {
    printf("%s: cannot bind port TCP %u\n",argv[0],SERVER_PORT);
    perror("error ");
    exit(1);
  }
				
  /* connect to server */
  rc = connect(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
  if(rc<0) {
    perror("cannot connect ");
    exit(1);
  }
  
  //reads HELLO MESSAGE
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);
  memset(buffer,'\0', strlen(buffer));

  //reads greeting
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  //setting ships
  for(int j = 3; j>0; j--){
    memset(buffer, '\0', strlen(buffer));
    printf("Set ship: %d\n", j);
    if(rc=read(sd, buffer, sizeof(buffer))<0) puts("Cannot read data\n");
    else printf("%s\n", buffer);
    memset(buffer, '\0', strlen(buffer));
    for(int i = 0; i < j; i++){
      printf("Pair of coordinates: %d\n", i);
      scanf(" %s", buffer);
      scanf(" %s", buffer2);
      if(rc=send(sd, buffer, strlen(buffer) + 1, 0)<0) puts("Cannot send data\n");
      if(rc=send(sd, buffer2, strlen(buffer2) + 1, 0)<0) puts("Cannot send data\n");
      memset(buffer, '\0', strlen(buffer));
      memset(buffer2, '\0', strlen(buffer2));
    }
  }

  //reads GAME MESSAGE
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  
 //main game code
  while(1){
    memset(buffer, '\0', sizeof(buffer));
    if(rc=read(sd, buffer, sizeof(buffer))<0) puts("Cannot read data\n");  //reads YOUT_MESS
    printf("\n %s\n", buffer);
    memset(buffer, '\0', sizeof(buffer));
    scanf("%s", buffer); //input x
    scanf("%s", buffer2); //input y
    if(rc=send(sd, buffer, strlen(buffer) + 1, 0)<0) puts("Cannot send data\n"); //send x
    else read(sd, trash, sizeof(trash)); 
    if(rc=send(sd, buffer2, strlen(buffer2) + 1, 0)<0) puts("Cannot send data\n"); //send y
    // else read(sd, trash, sizeof(trash));
    memset(buffer, '\0', sizeof(buffer)); 
    memset(buffer2, '\0', sizeof(buffer2)); 
    memset(shoot_map, '\0', sizeof(shoot_map)); 
    if(rc=read(sd, buffer, sizeof(buffer))<0) puts("Cannot read data\n"); //reads HIT MESSAGE and MAP
    else send(sd, "ok", sizeof("ok"), 0); 
    printf("\n %s\n", buffer); 

    if(strcmp(buffer,"The last ship sunk\n")==0){
      memset(buffer, '\0', sizeof(buffer));
      read(sd, buffer, sizeof(buffer));
      printf("\n %s\n", buffer);
      memset(buffer, '\0', sizeof(buffer));
      read(sd, buffer, sizeof(buffer));
      printf("\n %s\n", buffer);
      memset(buffer, '\0', sizeof(buffer));
      read(sd, buffer, sizeof(buffer));
      printf("\n %s\n", buffer);
      memset(buffer, '\0', sizeof(buffer));
      close(sd);
      exit(1);

    }
    if(rc<0){
      perror("cannot send data ");
      close(sd);
      exit(1);
    }
  }

return 0;
  
}

