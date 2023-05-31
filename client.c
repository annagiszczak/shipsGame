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

int n = 6;

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
  char shoot_map[1024]="";
  char trash[10]="";

  
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
  //   rc = send(sd, argv[i], strlen(argv[i]) + 1, 0);
  //Odczytuje HELLO MESSAGE
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);
  memset(buffer,'\0', strlen(buffer));

  //Odczytuje Hello game message
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  //Ustawia statki
  for(int j = 3; j>0; j--){
    memset(buffer, '\0', strlen(buffer));
    printf("Dane dla statku: %d\n", j);
    if(read(sd, buffer, sizeof(buffer))<0) puts("Cannot read data\n");
    else printf("%s\n", buffer);
    memset(buffer, '\0', strlen(buffer));
    for(int i = 0; i < j; i++){
      printf("Dane dla bloczku: %d\n", i);
      scanf(" %s", buffer);
      scanf(" %s", buffer2);
      if(send(sd, buffer, strlen(buffer) + 1, 0)<0) puts("Cannot send data\n");
      else printf("Wyslano %s\n", buffer);
      // delay(1);
      if(send(sd, buffer2, strlen(buffer2) + 1, 0)<0) puts("Cannot send data\n");
      else printf("Wyslano %s\n", buffer2);
      // delay(1);
      memset(buffer, '\0', strlen(buffer));
      memset(buffer2, '\0', strlen(buffer2));
    }
  }

  //Odczytuje Game is starting
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  //
  
 //glowna petla
  while(1){
    memset(buffer, '\0', sizeof(buffer));
    read(sd, buffer, sizeof(buffer));  //YOUT_MESS odczytuje your turn
    printf("\n %s\n", buffer);
    memset(buffer, '\0', sizeof(buffer));
    while(1){
      scanf("%s", buffer); //x
      scanf("%s", buffer2); //y
      if(send(sd, buffer, strlen(buffer) + 1, 0)<0) puts("Cannot send data\n"); //send x
      else read(sd, trash, sizeof(trash)); //read OK 
      if(send(sd, buffer2, strlen(buffer2) + 1, 0)<0) puts("Cannot send data\n"); //send y
      // else read(sd, trash, sizeof(trash)); //read OK 
      memset(buffer, '\0', sizeof(buffer)); //x
      memset(buffer2, '\0', sizeof(buffer2)); //y
      memset(shoot_map, '\0', sizeof(shoot_map)); //mapa
      if(read(sd, buffer, sizeof(buffer))<0) puts("Cannot send data\n"); //read MISS or HIT or LASTHIT and MAP
      else send(sd, "ok", sizeof("ok"), 0); //send OK
      printf("\n %s\n", buffer); 
      //delay(2);
      //wyswietla strzaly
      //read(sd, shoot_map, sizeof(shoot_map));
      //delaye dodac czy cos
      //printf("%s\n", shoot_map);
      //delay(1);
      if(strcmp(buffer,"Miss, Your opponent's turn\n")==0){
        break;
      }else if(strcmp(buffer,"The last ship sunk\n")==0){
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

//zamyka polaczenie
rc = send(sd, buffer, strlen(buffer) + 1, 0);
if(rc<0){
  perror("cannot send data ");
  close(sd);
  exit(1);
}
return 0;
  
}

