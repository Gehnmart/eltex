#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define SERV "224.0.0.1"
#define PORT 2048
#define BUF_MAX 50

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int stop = 0;

void GetTime(char *buf, int n) {
  time_t rawtime;
  struct tm *timeinfo;
  memset(buf, 0, n);

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  asctime_r(timeinfo, buf);
}

void *ServRoutine(void *argv) {
  (void)argv;
  char buf[BUF_MAX];
  int sfd;
  struct sockaddr_in endpoint;

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    handle_error("socket error:");
  }

  memset(&endpoint, 0, sizeof(endpoint));
  endpoint.sin_family = AF_INET;
  endpoint.sin_port = htons(PORT);
  inet_pton(AF_INET, SERV, &endpoint.sin_addr.s_addr);

  while (!stop) {
    GetTime(buf, sizeof(buf));
    sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&endpoint,
           sizeof(endpoint));
    sleep(1);
  }

  close(sfd);
  pthread_exit(EXIT_SUCCESS);
}

int main() {
  int err = 0;
  pthread_t serv_thread;
  
  err = pthread_create(&serv_thread, NULL, ServRoutine, NULL);
  if(err == -1){
    handle_error("pthread_create():");
  }

  while(getchar() != '0');
  stop = 1;
  pthread_join(serv_thread, NULL);

  exit(EXIT_SUCCESS);
}