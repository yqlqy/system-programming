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
  // char buf[MAX_LINE];
  int count = atoi(argv[3]);
  int threeWayHandShaking = 0;
  while (1){//((fgets(buf, sizeof(buf),stdin)) {
    char sendInfo[] = "HELLO ";
    char number[ARRAY_SIZE];
    sprintf(number, "%d\n", count);
    strcat(sendInfo, number);
    int len = strlen(sendInfo)+1;
    send(s, sendInfo, len, 0);
    threeWayHandShaking++;
    count++;
    char buf[ARRAY_SIZE];
    memset(sendInfo, 0, strlen(sendInfo));
    memset(number, 0, sizeof(number));
    
    if (threeWayHandShaking == 2) {
      close(s);
      //printf("Closing socket and breaking...");
      break;
    }

    char expectedMsg[ARRAY_SIZE] = "HELLO ";
    sprintf(number, "%d\n", count);
    strcat(expectedMsg, number);
    expectedMsg[strlen(expectedMsg)] = 0;
    int msgLen;
    msgLen = recv(s, buf, sizeof(buf), 0);
    buf[msgLen] = '\0';
    // printf("Expected msg: %s\n", expectedMsg);
    // printf("Buf msg: %s\n", buf); 

    if (strcmp(expectedMsg, buf) == 0) {
      fputs(buf, stdout);
      fflush(stdout);
      count++;
    } else {
      perror("ERROR");
    }

    
    //sleep(3);
  }

  return 0;
}

//int isMatch(char assert[], char msg[]) {
//  int len;
//  len = strlen(msg);
//  int i;
//  for (i = 0; i < len; i++) {
//    if (msg[i] == '\n' || msg[i] == 0) {
//      return 1;
//    } else if (msg[i] != assert[i]) {
//      return 0;
//    }
//  }
//  return 1;
//}
