#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_LINE 20
#define ARRAY_SIZE 1024
//int isMatch(char assert[], char msg[]);

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
  int count = atoi(argv[3]);
  int threeWayHandShaking = 0;
  while (1){
    char sendInfo[ARRAY_SIZE];
    snprintf(sendInfo, ARRAY_SIZE, "HELLO %d", count);
    int len = strlen(sendInfo)+1;
    send(s, sendInfo, len, 0);
    threeWayHandShaking++;
    count++;
    char buf[ARRAY_SIZE];
    memset(sendInfo, 0, strlen(sendInfo));
    
    if (threeWayHandShaking == 2) {
      close(s);
      break;
    }

    char expectedMsg[ARRAY_SIZE];
    snprintf(expectedMsg, ARRAY_SIZE, "HELLO %d", count);
    int msgLen;
    msgLen = recv(s, buf, sizeof(buf), 0);
    buf[msgLen] = '\0';
    if (strcmp(expectedMsg, buf) == 0) {
      fputs(buf, stdout);
      fputs("\n", stdout);
      fflush(stdout);
      count++;
    } else {
      perror("ERROR");
    }
  }

  return 0;
}

