#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

/* This is a reference socket server implementation that prints out the messages
 * received from clients. */

#define MAX_PENDING 10
#define MAX_LINE 10
#define ARRAY_SIZE 1024

int main(int argc, char *argv[]) {
  // char* host_addr = argv[1];
  // int port = atoi(argv[2]);
  /* hard-coded ip address for project3a*/
  char* host_addr = "127.0.0.1";
  int port = atoi(argv[1]);

  /*setup passive open*/
  int s;
  if((s = socket(PF_INET, SOCK_STREAM, 0)) <0) {
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

  /* Bind the socket to the address */
  if((bind(s, (struct sockaddr*)&sin, sizeof(sin)))<0) {
    perror("simplex-talk: bind");
    exit(1);
  }
  // connections can be pending if many concurrent client requests
  listen(s, MAX_PENDING);  
 
  /* wait for connection, then receive and print text */
  int new_s;
  socklen_t len = sizeof(sin);
  char buf[ARRAY_SIZE];
  char number[ARRAY_SIZE];
  while(1) {
    if((new_s = accept(s, (struct sockaddr *)&sin, &len)) <0){
      perror("simplex-talk: accept");
      exit(1);
    }
    // printf("Waiting for connection...\n");
    int seq = 0;
    while(len = recv(new_s, buf, sizeof(buf), 0)){
      //buf[len] = '\0';
      if (seq > 0) {
        char expectedMsg[ARRAY_SIZE] = "HELLO ";
        memset(number, 0, strlen(number));
        sprintf(number, "%d\n", seq+1);
        strcat(expectedMsg, number);
	expectedMsg[strlen(expectedMsg)] = 0;
        if (strcmp(expectedMsg, buf) == 0) {
          fputs(buf, stdout);
	  fflush(stdout);
	  // fputs("Finished three way handshaking!!!\n", stdout);
	  fflush(stdout);
        } else {
          perror("ERROR");
        }
	continue;
        //close(new_s);
        //close(s);
	//break;
      }
      fputs(buf, stdout);
      fflush(stdout);
      char *pt = strtok(buf, " ");
      char ** res = NULL;
      int idx =0;
      while (pt) {
        res = (char **)realloc(res, sizeof(char*) * strlen(buf));
        if (res == NULL) exit(-1);
        res[idx++] = pt;
        pt = strtok(NULL, " ");
      }
      seq = atoi(res[1]);
      seq++;
      memset(number, 0, sizeof(number));
      sprintf(number, " %d\n", seq);
      strcat(res[0], number);
      //res[0][strlen(res[0])] = 0;
      send(new_s, res[0], strlen(buf), 0);
      memset(buf, 0, sizeof(buf));
    }
    close(new_s);
  }

  return 0;
}
