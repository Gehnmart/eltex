#include <pthread.h>
#include <time.h>

#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int port = PORT;
int stop = 0;

pthread_mutex_t port_mutex = PTHREAD_MUTEX_INITIALIZER;

int port_inc() {
  pthread_mutex_lock(&port_mutex);
  port++;
  pthread_mutex_unlock(&port_mutex);
  return port;
}

char *get_time(char *buf) {
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return asctime_r(timeinfo, buf);
}

void server_list_destroy(ServerCtl *server_list) {
  for (int i = 0; i < server_list->len; i++) {
    pthread_join(server_list->threads[i], NULL);
  }
  free(server_list->threads);
  free(server_list->clients);
}

pthread_t *server_list_add(ServerCtl *server_list, struct sockaddr_in *client) {
  if (server_list->len == 0) {
    server_list->len = 1;
    server_list->threads =
        (pthread_t *)calloc(sizeof(pthread_t), server_list->len);
    server_list->clients = (struct sockaddr_in *)calloc(
        sizeof(struct sockaddr_in), server_list->len);
    memcpy(&server_list->clients[server_list->len - 1], client,
           sizeof(*client));
  } else {
    server_list->len++;
    pthread_t *new_threads = (pthread_t *)realloc(
        server_list->threads, sizeof(pthread_t) * server_list->len);
    server_list->threads = new_threads;
    struct sockaddr_in *new_clients = (struct sockaddr_in *)realloc(
        server_list->clients, sizeof(struct sockaddr_in) * server_list->len);
    server_list->clients = new_clients;
    memcpy(&server_list->clients[server_list->len - 1], client,
           sizeof(*client));
  }

  return &server_list->threads[server_list->len - 1];
}

void *ChildServer(void *argv) {
  struct sockaddr_in *client = (struct sockaddr_in *)argv;
  printf("%s\n", inet_ntoa(client->sin_addr));
  int sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) handle_error("socket");

  int coking_addr = 0;
  inet_pton(AF_INET, ADDR, &coking_addr);
  struct sockaddr_in serv;
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = coking_addr;

  int new_port = 0;
  while (!stop) {
    new_port = port_inc();
    serv.sin_port = htons(new_port);
    if (bind(sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
      if (errno == EADDRINUSE)
        continue;
      else
        handle_error("bind error");
    }
    break;
  }

  char buf[BUF_MAX] = {0};
  socklen_t cl_size = sizeof(*client);
  sendto(sfd, &new_port, sizeof(new_port), 0, (struct sockaddr *)client,
         cl_size);
  while (!stop) {
    recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)client, &cl_size);
    if (strncmp(buf, "time", sizeof(buf)) == 0) {
      get_time(buf);
      sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *)client, cl_size);
    } else if (strncmp(buf, "exit", sizeof(buf)) == 0) {
      break;
    }
  }

  close(sfd);
  pthread_exit(EXIT_SUCCESS);
}

void *ListenServer(void *argv) {
  ServerCtl *server_list = (ServerCtl *)argv;

  char buf[BUF_MAX];
  while (!stop) {
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    socklen_t cl_size = sizeof(client);
    recvfrom(server_list->sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client,
             &cl_size);
    pthread_t *new_thread = server_list_add(server_list, &client);
    pthread_create(new_thread, NULL, ChildServer,
                   &server_list->clients[server_list->len - 1]);
  }

  close(server_list->sfd);
  pthread_exit(EXIT_SUCCESS);
}

int main() {
  ServerCtl server_list = {0};
  server_list.sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_list.sfd == -1) handle_error("socket");
  int coking_addr = 0;
  inet_pton(AF_INET, ADDR, &coking_addr);
  struct sockaddr_in serv;
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(port);
  serv.sin_addr.s_addr = coking_addr;

  if (bind(server_list.sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1)
    handle_error("bind");

  pthread_t listen_server;
  pthread_create(&listen_server, NULL, ListenServer, (void *)&server_list);
  while (1) {
    if (getchar() == '0') break;
  }
  stop = 1;
  sendto(server_list.sfd, "exit", 5, 0, (struct sockaddr *)&serv, sizeof(serv));
  pthread_join(listen_server, NULL);
  server_list_destroy(&server_list);

  exit(EXIT_SUCCESS);
}