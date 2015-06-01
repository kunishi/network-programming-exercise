#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12345

void send_message(int s)
{
  char buf[1024];
  while (fgets(buf, sizeof(buf), stdin) != NULL) {
    if (write(s, buf, strlen(buf)) == -1) {
      perror("send error");
      return;
    }
  }
}

int main(void)
{
  int s, cc;
  struct sockaddr_in sa;
  char buf[1024];
  char *message = "A message content\n";

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(SERVER_PORT);
  sa.sin_addr.s_addr = inet_addr(SERVER_ADDR);

#if defined(INFO)
  fprintf(stderr, "Connecting to the server...\n");
#endif
  if (connect(s, (struct sockaddr *)&sa, sizeof(sa)) == -1) {
    perror("connect");
    exit(1);
  }
#if defined(INFO)
  fprintf(stderr, "Connected.\n");
#endif

#if defined(INFO)
  fprintf(stderr, "Send a message to the server:\n\n");
#endif
  send_message(s);
  if (shutdown(s, SHUT_WR) == -1) {
    perror("shutdown write");
    exit(1);
  }
#if defined(INFO)
  fprintf(stderr, "\n\nFinished sending.\n");
#endif

#if defined(INFO)
  fprintf(stderr, "Message from the server:\n\n");
#endif
  while ((cc = read(s, buf, sizeof(buf))) > 0) {
    write(1, buf, cc); /* 1: stdout */
  }
  if (cc == -1) {
    perror("read");
    exit(1);
  }
#if defined(INFO)
  fprintf(stderr, "\n\nFinished receiving.\n");
#endif

  if (close(s) == -1) {
    perror("close");
    exit(1);
  }

  return 0;
}
