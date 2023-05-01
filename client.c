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


int main (int argc, char *argv[]) { /* licznik argumentow, tablica argumentow */

  const int SERVER_PORT=8888;

  int sd, rc;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  char buffer[1024];
  char buffer2[1024];

  
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
  for(int i = 0; i < 1; i++){
    scanf("%s", buffer);
    scanf("%s", buffer2);
    if(send(sd, buffer, strlen(buffer) + 1, 0)<0) printf("Error\n");
    if(send(sd, buffer2, strlen(buffer2) + 1, 0)<0) printf("Error\n");
    printf("%s  ", buffer);
    printf("%s", buffer2);
  }

  //Odczytuje Game is starting
  read(sd, buffer, sizeof(buffer));
  printf("%s\n", buffer);




  // while(1){
    // puts("Hello");
  //   read(sd, buffer, sizeof(buffer));
  //   printf("%s\n", buffer);
  //   // rc = send(sd, argv[i], strlen(argv[i]) + 1, 0);
  //   //wyslac koordynaty statku
    
  // }

return 0;
  
}
