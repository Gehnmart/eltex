#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 2048
#define ADDR "192.168.56.2"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

typedef struct {
  int                stop;
  int                sfd;
  struct sockaddr_in addr;
} ServInfo;

int stop = 0;

void *ServThread(void *argv) {
  ServInfo *serv = (ServInfo *)argv;
  struct sockaddr_in client;
  memset(&client, 0, sizeof(client));

  if (bind(serv->sfd, (struct sockaddr *)&serv->addr, sizeof(serv->addr)) == -1)
    handle_error("bind():");

  char recv_buf[101] = {0};
  char send_buf[101] = {0};
  socklen_t cl_size = sizeof(client);

  while (!stop) {
    recvfrom(serv->sfd, recv_buf, sizeof(recv_buf), 0,
             (struct sockaddr *)&client, &cl_size);
    printf("packet %s\n", recv_buf);
    strncpy(send_buf, recv_buf, sizeof(recv_buf));
    sendto(serv->sfd, send_buf, sizeof(recv_buf), 0, (struct sockaddr *)&client,
           cl_size);
  }

  close(serv->sfd);
  pthread_exit(EXIT_SUCCESS);
}

int main() {
  ServInfo serv;
  serv.sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (serv.sfd == -1) handle_error("socket");

  memset(&serv.addr, 0, sizeof(serv.addr));
  serv.addr.sin_family = AF_INET;
  serv.addr.sin_port = htons(PORT);
  inet_pton(AF_INET, ADDR, &serv.addr.sin_addr.s_addr);

  pthread_t serv_thread;
  if (pthread_create(&serv_thread, NULL, ServThread, (void *)&serv) == -1) {
    handle_error("pthread_create():");
  }
  while (getchar() == '0');
  stop = 1;
  sendto(serv.sfd, NULL, 0, 0, (struct sockaddr *)&serv.addr,
         sizeof(serv.addr));

  pthread_join(serv_thread, NULL);

  exit(EXIT_SUCCESS);
}