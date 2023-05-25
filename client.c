/* fpont 12/99 */
/* pont.net    */
/* tcpClient.c */
/* korekta W.Bajdecki 2009, 2019, 2023 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* close */
#include <string.h> /* memset() */
#include <time.h>

int n = 3;

void delay(int number_of_sec){
	int mili_sec = 1000*number_of_sec;
	clock_t start_time = clock();
	while(clock()<start_time + mili_sec);
}

int main (int argc, char *argv[]) { /* licznik argumentow, tablica argumentow */

  const int SERVER_PORT=8888;

  int sd, rc;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  char buffer[1024]="";
  char buffer2[1024]="";
  char shoot_map[3]="";

  
  if(argc < 2) {
    printf("usage:  %s <server> \n",argv[0]);
    exit(1);
  }

  h = gethostbyname(argv[1]); //zamiana nazwy domenowej na adres IP maszyny
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

  // for(int i=2;i<argc;i++) {
    
  //   rc = send(sd, argv[i], strlen(argv[i]) + 1, 0);
    
  //   if(rc<0) {
  //     perror("cannot send data ");
  //     close(sd);
  //     exit(1);
    
  //   }

  //   printf("%s: data%u sent (%s)\n",argv[0],i-1,argv[i]);

   
  // }
  //Odczytuje HELLO MESSAGE
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  //Odczytuje Hello game message
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  //Ustawia statki
  for(int j = 3; j>0; j--){
    printf("%d co \n", j);
    memset(buffer, '\0', sizeof(buffer));
    printf("%d coo\n", j);
    rc = read(sd, buffer, sizeof(buffer));
    printf("%d\n", rc);
    printf("%d cooo\n", j);
    // delay(1);
    printf("%s\n", buffer);
    memset(buffer, '\0', sizeof(buffer));
    for(int i = 0; i < j; i++){
      printf("jestem w petli\n");
      // delay(10);
      scanf(" %s", buffer);
      scanf(" %s", buffer2);
      if(send(sd, buffer, strlen(buffer) + 1, 0)<0) puts("Cannot send data\n");
      delay(1);
      if(send(sd, buffer2, strlen(buffer2) + 1, 0)<0) puts("Cannot send data\n");
      delay(1);
      printf("%s\n", buffer);
      printf("%s\n", buffer2);
      memset(buffer, '\0', sizeof(buffer));
      memset(buffer2, '\0', sizeof(buffer2));
    }
  }

  //Odczytuje Game is starting
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  //
  

  while(1){
    memset(buffer, '\0', sizeof(buffer));
    read(sd, buffer, sizeof(buffer));  //YOUT_MESS
    printf("\n %s\n", buffer);
    memset(buffer, '\0', sizeof(buffer));
    while(1){
      scanf("%s", buffer); //x
      scanf("%s", buffer2); //y
      if(send(sd, buffer, strlen(buffer) + 1, 0)<0) puts("Cannot send data\n"); //send x
      delay(1);
      if(send(sd, buffer2, strlen(buffer2) + 1, 0)<0) puts("Cannot send data\n"); //send y
      memset(buffer, '\0', sizeof(buffer));
      memset(buffer2, '\0', sizeof(buffer2));
      delay(1);
      read(sd, buffer, sizeof(buffer));  //read MISS or HIT or LASTHIT
      printf("\n %s\n", buffer);
      //Wyswietla plansze strzalow
      for(int j = 0;j<10;j++){
        for(int i=0;i<10;i++){
          memset(shoot_map, '\0', sizeof(shoot_map));
          read(sd, shoot_map, sizeof(shoot_map));
          printf("%s", shoot_map);
        }
		  puts("\n");
	    }

      if(strcmp(buffer,"Pudło, kolej przeciwnika\n")==0){
        break;
      }else if(strcmp(buffer,"Zatopiony, ostatni statek\n")==0){
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
        break;
      }
    }
    
  }

return 0;
  
}

