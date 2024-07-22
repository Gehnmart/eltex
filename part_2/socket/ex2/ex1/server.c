#include <pthread.h>
#include <time.h>

#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int port = PORT_SRV;
int stop = 0;

pthread_mutex_t port_mutex = PTHREAD_MUTEX_INITIALIZER;

int port_inc() {
  pthread_mutex_lock(&port_mutex);
  port++;
  pthread_mutex_unlock(&port_mutex);
  return port;
}

void GetTime(char *buf, int n) {
  time_t rawtime;
  struct tm *timeinfo;
  memset(buf, 0, n);

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  asctime_r(timeinfo, buf);
}

void ServerListDestroy(ServerCtl *ctl) {
  for (int i = 0; i < ctl->len; i++) {
    pthread_join(ctl->cl_list[i].thread, NULL);
  }
  free(ctl->cl_list);
}

int SearchFreePort(int sfd, struct sockaddr_in *serv) {
  while (!stop) {
    serv->sin_port = htons(port_inc());
    if (bind(sfd, (struct sockaddr *)serv, sizeof(*serv)) == -1) {
      if (errno == EADDRINUSE)
        continue;
      else
        return -1;
    }
    return 0;
  }
}

pthread_t *ServerListAdd(ServerCtl *ctl, struct sockaddr_in *client) {
  if (ctl->len == 0) {
    ctl->cl_list = calloc(sizeof(Client), 1);
    memcpy(&ctl->cl_list[0].client, client, sizeof(*client));
    ctl->len = 1;
  } else {
    ctl->len++;
    ctl->cl_list = realloc(ctl->cl_list, sizeof(Client) * ctl->len);
    memcpy(&ctl->cl_list[ctl->len - 1].client, client, sizeof(*client));
  }

  return &ctl->cl_list[ctl->len - 1].thread;
}

void *ChildServer(void *argv) {
  struct sockaddr_in *client = (struct sockaddr_in *)argv;

  struct sockaddr_in serv;
  int sfd, new_port = 0;
  char buf[BUF_MAX] = {0};
  socklen_t cl_size = sizeof(*client);

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    perror("socket(): ChildServer");
    pthread_exit(NULL);
  }

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  inet_pton(AF_INET, ADDR_SRV, &serv.sin_addr.s_addr);

  if (SearchFreePort(sfd, &serv) == -1) {
    perror("close(): ChildServer");
    pthread_exit(NULL);
  }

  printf("CLIENT {addr = %s, port = %d} CONNECTED\n",
         inet_ntoa(client->sin_addr), ntohs(client->sin_port));

  sendto(sfd, &new_port, sizeof(new_port), 0, (struct sockaddr *)client,
         cl_size);
  while (!stop) {
    recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)client, &cl_size);
    if (strncmp(buf, "time", sizeof(buf)) == 0) {
      GetTime(buf, sizeof(buf));
      sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *)client, cl_size);
    } else if (strncmp(buf, "exit", sizeof(buf)) == 0) {
      break;
    }
  }

  if (close(sfd) == -1) {
    perror("close(): ChildServer");
  }

  pthread_exit(NULL);
}

void *ListenServer(void *argv) {
  ServerCtl *ctl = (ServerCtl *)argv;

  char buf[BUF_MAX];
  struct sockaddr_in client;
  socklen_t cl_size = sizeof(client);
  while (!stop) {
    recvfrom(ctl->sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client,
             &cl_size);
    pthread_t *new_thread = ServerListAdd(ctl, &client);
    pthread_create(new_thread, NULL, ChildServer,
                   &ctl->cl_list[ctl->len - 1].client);
  }

  pthread_exit(EXIT_SUCCESS);
}

int main() {
  ServerCtl ctl = {0};
  pthread_t listen_server;
  struct sockaddr_in serv;

  ctl.sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (ctl.sfd == -1) {
    handle_error("socket():");
  }

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(port);
  inet_pton(AF_INET, ADDR_SRV, &serv.sin_addr.s_addr);

  if (bind(ctl.sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
    handle_error("bind():");
  }

  pthread_create(&listen_server, NULL, ListenServer, (void *)&ctl);
  while (!stop) {
    if (getchar() == '0') {
      stop = 1;
      sendto(ctl.sfd, "exit", 5, 0, (struct sockaddr *)&serv, sizeof(serv));
    }
  }
  pthread_join(listen_server, NULL);
  ServerListDestroy(&ctl);

  if (close(ctl.sfd) == -1) {
    handle_error("close():");
  }

  exit(EXIT_SUCCESS);
}