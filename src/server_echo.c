#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12345

enum {
  NQUEUESIZE = 5,
};

int main(void)
{
  int s, ws, soval, cc;
  struct sockaddr_in sa, ca;
  socklen_t ca_len;
  char message[10240];
  char buf[1024];

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  soval = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &soval, sizeof(soval)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(SERVER_PORT);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
    perror("bind");
    exit(1);
  }

  if (listen(s, NQUEUESIZE)) {
    perror("listen");
    exit(1);
  }

  fprintf(stderr, "Ready.\n");

  for (;;) {
    fprintf(stderr, "Waiting for a connection..\n");
    memset(message, 0, sizeof(message));

    ca_len = sizeof(ca);
    if ((ws = accept(s, (struct sockaddr *)&ca, &ca_len)) == -1) {
      perror("accept");
      exit(1);
    }
    fprintf(stderr, "Connection established.\n");

    fprintf(stderr, "Receiving a message from the client...\n");
    memset(buf, 0, sizeof(buf));
    while ((cc = read(ws, buf, sizeof(buf))) > 0) {
      write(1, buf, cc); /* 1: stdout */
      strcat(message, buf);
      memset(buf, 0, sizeof(buf));
    }
    if (cc == -1) {
      perror("read");
      exit(1);
    }
    fprintf(stderr, "\n\nFinished receiving.\n");

    fprintf(stderr, "Sending the message...\n");
    if ((cc = write(ws, message, strlen(message))) == -1) {
      perror("write");
      exit(1);
    }
    fprintf(stderr, "Message sent.\n");

    if (shutdown(ws, SHUT_RDWR) == -1) {
      perror("shutdown");
      exit(1);
    }

    if (close(ws) == -1) {
      perror("close");
      exit(1);
    }
  }
}
