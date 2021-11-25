#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include <pthread.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

/* This is a reference socket server implementation that prints out the messages
 * received from clients. */

#define MAX_PENDING 1000
#define MAX_LINE 10
#define ARRAY_SIZE 1024

typedef struct
{
  int fd;
  int seq;
  char *msg;
} Client;

Client clients[100];

//void *connectToClient(void *arg);
void handle_first_shake(int fd);
void handle_second_shake(int fd);

int main(int argc, char *argv[])
{

  /* hard-coded ip address for project3a*/
  char *host_addr = "127.0.0.1";
  int port = atoi(argv[1]);

  /*setup passive open*/
  int s;
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
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
  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0)
  {
    perror("simplex-talk: bind");
    exit(1);
  }

  listen(s, MAX_PENDING);
  fd_set fd_set_for_all;
  fd_set fd_set_in_use;
  FD_ZERO(&fd_set_for_all);
  FD_SET(s, &fd_set_for_all);

  Client reads[MAX_PENDING];
  while (1)
  {
    socklen_t len = sizeof(sin);

    fd_set_in_use = fd_set_for_all;
    select(FD_SETSIZE, &fd_set_in_use, NULL, NULL, NULL);

    int fd;
    for (fd = 0; fd < FD_SETSIZE; fd++)
    {
      if (FD_ISSET(fd, &fd_set_in_use))
      {
        if (fd == s)
        {
          int new_s;
          new_s = accept(s, (struct sockaddr *)&sin, &len);
          int flags = fcntl(fd, F_GETFL, 0);
          fcntl(fd, F_SETFL, flags | O_NONBLOCK);
          FD_SET(new_s, &fd_set_for_all);
        }
        else
        {
          handle_first_shake(fd);
          // sleep(0.2);
          handle_second_shake(fd);
          close(fd);
          FD_CLR(fd, &fd_set_for_all);
        }
      }
    }
  }

  return 0;
}

void handle_first_shake(int fd)
{
  char buf[ARRAY_SIZE];
  char number[ARRAY_SIZE];
  int seq;
  if (recv(fd, buf, sizeof(buf), 0) < 0)
  {
    perror("First shake read error.");
  }
  else
  {
    fputs(buf, stdout);
    fputs("\n", stdout);
    fflush(stdout);
    char *pt = strtok(buf, " ");
    char **res = NULL;
    int idx = 0;
    while (pt)
    {
      res = (char **)realloc(res, sizeof(char *) * strlen(buf));
      if (res == NULL)
        exit(-1);
      res[idx++] = pt;
      pt = strtok(NULL, " ");
    }
    seq = atoi(res[1]);
    char* msg = malloc(strlen(res[0]+1));
    strcpy(msg, res[0]);
    clients[fd].seq = seq;
    clients[fd].msg = msg;
    clients[fd].fd = fd;
    seq++;
    memset(number, 0, sizeof(number));
    sprintf(number, " %d", seq);
    strcat(res[0], number);
    send(fd, res[0], strlen(buf), 0);
    memset(buf, 0, sizeof(buf));
  }
}

void handle_second_shake(int fd)
{
  char expectedMsg[ARRAY_SIZE];
  char buf[ARRAY_SIZE];
  // printf("Message is %s\n",clients[fd].msg);
  snprintf(expectedMsg, ARRAY_SIZE, "%s %d", clients[fd].msg, clients[fd].seq + 2);
  if (recv(fd, buf, sizeof(buf), 0) < 0)
  {
    perror("Second shake read error.");
  }
  if (strcmp(expectedMsg, buf) == 0)
  {
    fputs(buf, stdout);
    fputs("\n", stdout);
    fflush(stdout);
  }
  else
  {
    perror("ERROR");
  }
  free(clients[fd].msg);
}
