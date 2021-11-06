#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_LINE 20

int main (int argc, char *argv[]) {
  char* host_addr = argv[1];
  int port = atoi(argv[2]);

  /* Open a socket */
  int s;
  if((s = socket(PF_INET, SOCK_STREAM, 0)) <0){
    perror("simplex-talk: socket");
    exit(1);
  }

  /* Config the server address */
  struct sockaddr_in sin;
  sin.sin_family = AF_INET; 
  sin.sin_addr.s_addr = inet_addr(host_addr);
  sin.sin_port = htons(port);
  // Set all bits of the padding field to 0
  memset(sin.sin_zero, '\0', sizeof(sin.sin_zero));

  /* Connect to the server */
  if(connect(s, (struct sockaddr *)&sin, sizeof(sin))<0){
    perror("simplex-talk: connect");
    close(s);
    exit(1);
  }

  /*main loop: get and send lines of text */
  // char buf[MAX_LINE];
  int count = atoi(argv[3]);
  int threeWayHandShaking = 0;
  while (1){//((fgets(buf, sizeof(buf),stdin)) {
    char sendInfo[] = "HELLO ";
    char number[5];
    sprintf(number, "%d\n", count);
    strcat(sendInfo, number);
    // buf[MAX_LINE-1] = '\0';
    int len = strlen(sendInfo)+1;
    send(s, sendInfo, len, 0);
    threeWayHandShaking++;
    count++;
    // char recvFrom[10];
    // char recvInfo[10];
    // sprintf(recvInfo, "HELLO %d\n", count);
    // char received[len];
    memset(sendInfo, 0, sizeof(sendInfo));
    memset(number, 0, sizeof(number));
    recv(s, sendInfo, len, 0);
    char expectedMsg[] = "HELLO ";
    sprintf(number, "%d\n", count);
    strcat(expectedMsg, number);
    if (strcmp(sendInfo, expectedMsg) == 0) {
      fputs(sendInfo, stdout);
    } else {
      perror("ERROR");
    }
    if (threeWayHandShaking == 2) {
      close(s);
      break;
    }
    
    sleep(3);
  }
  // char buf[10] = "Hello";
  // printf("line46");
  // while (1) {
  //   printf("Client call...");
  //   buf[strlen(buf)-1] = '\0';
  //   send(s, buf, strlen(buf)+1, 0);
  //   sleep(5);
  // }

  return 0;
}
