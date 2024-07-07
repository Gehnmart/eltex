#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int stop = 0;

void GetTime(char *buf, int n) {
  memset(buf, 0, n);
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  asctime_r(timeinfo, buf);
}

int ClientRedirection(ServerCtl *ctl, int sfd, struct sockaddr_in *client) {
  static int client_counter = 0;
  for (int i = 0; i < SERV_COUNT; i++) {
    if (ctl->server_list[i].is_bussy) {
      continue;
    } else {
      memcpy(&ctl->server_list[i].client, client, sizeof(*client));
      printf("CLIENT %d CONNECTED %s\n", client_counter++,
             inet_ntoa(client->sin_addr));
      sendto(sfd, &ctl->server_list[i].port, sizeof(ctl->server_list[i].port),
             0, (struct sockaddr *)client, sizeof(*client));
      ctl->server_list[i].is_bussy = 1;
      return 1;
    }
  }
  return 0;
}

void *ServerRoutine(void *argv) {
  Server *server = (Server *)argv;
  char buf[BUF_MAX] = {0};
  socklen_t cl_size = sizeof(server->client);
  while (!stop) {
    while (server->is_bussy) {
      recvfrom(server->sfd, buf, sizeof(buf), 0,
               (struct sockaddr *)&server->client, &cl_size);
      printf("CLIENT SEND %s\n", buf);
      if (0 == strncmp(buf, "time", sizeof(buf))) {
        GetTime(buf, sizeof(buf));
        sendto(server->sfd, buf, sizeof(buf), 0,
               (struct sockaddr *)&server->client, cl_size);
      } else if (0 == strncmp(buf, "exit", sizeof(buf))) {
        server->is_bussy = 0;
        printf("CLIENT EXIT\n");
      }
    }
    usleep(100);
  }

  close(server->sfd);
  pthread_exit(EXIT_SUCCESS);
}

void CreateServers(ServerCtl *ctl) {
  struct sockaddr_in serv;
  int addr, res;
  ctl->port = SERV_PORT + 1;
  for (int i = 0; i < SERV_COUNT; i++) {
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
      handle_error("socket error:");
    }

    inet_pton(AF_INET, SERV_ADDR, &addr);
    memset(&serv, 0, sizeof(serv));
    serv.sin_addr.s_addr = addr;
    serv.sin_family = AF_INET;
    while (!stop) {
      serv.sin_port = htons(ctl->port);
      res = bind(sfd, (struct sockaddr *)&serv, sizeof(serv));
      if (res == -1 && errno == EADDRINUSE) {
        ctl->port++;
        continue;
      } else if (res == -1) {
        handle_error("bind error:");
      }
      break;
    }
    ctl->server_list[i].sfd = sfd;
    ctl->server_list[i].port = ctl->port;
    ctl->server_list[i].is_bussy = 0;
  }
}

void RunServers(ServerCtl *ctl) {
  for (int i = 0; i < SERV_COUNT; i++) {
    pthread_create(&ctl->server_list[i].thread, NULL, ServerRoutine,
                   (void *)&ctl->server_list[i]);
  }
}

void JoinServers(ServerCtl *ctl) {
  for (int i = 0; i < SERV_COUNT; i++) {
    pthread_join(ctl->server_list[i].thread, NULL);
  }
}

struct LissenParam {
  int sfd;
  struct sockaddr_in *client;
  ServerCtl *ctl;
};

void *LissenServer(void *argv) {
  struct LissenParam *param = (struct LissenParam *)argv;
  char buf[BUF_MAX];
  socklen_t cl_size = sizeof(*param->client);
  while (!stop) {
    memset(param->client, 0, sizeof(*param->client));
    recvfrom(param->sfd, buf, sizeof(buf), 0, (struct sockaddr *)param->client,
             &cl_size);
    while (!stop && !ClientRedirection(param->ctl, param->sfd, param->client));
  }

  pthread_exit(EXIT_SUCCESS);
}

int main() {
  struct sockaddr_in serv, client;
  ServerCtl ctl;
  int sfd, addr;
  pthread_t lissen_serv;

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    handle_error("socket error:");
  }

  inet_pton(AF_INET, SERV_ADDR, &addr);
  memset(&serv, 0, sizeof(serv));
  serv.sin_port = htons(SERV_PORT);
  serv.sin_addr.s_addr = addr;
  serv.sin_family = AF_INET;

  if (bind(sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
    handle_error("bind error:");
  }
  struct LissenParam param;
  param.client = &client;
  param.ctl = &ctl;
  param.sfd = sfd;
  CreateServers(&ctl);
  RunServers(&ctl);

  pthread_create(&lissen_serv, NULL, LissenServer, (void *)&param);
  printf("SERVER START\n");

  while (1) {
    if (getchar() == '0') break;
  }
  stop = 1;
  sendto(sfd, "exit", 5, 0, (struct sockaddr *)&serv, sizeof(serv));
  pthread_join(lissen_serv, NULL);

  JoinServers(&ctl);

  exit(EXIT_SUCCESS);
}