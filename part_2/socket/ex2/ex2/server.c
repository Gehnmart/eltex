#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int stop = 0;

char *GetTime(char *buf) {
  memset(buf, 0, sizeof(buf));
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return asctime_r(timeinfo, buf);
}

void ClientRedirection(ServerCtl *ctl, int sfd, struct sockaddr_in *client) {
  for(int i = 0; i < SERV_COUNT; i++) {
    if(ctl->server_list[i].is_bussy) {
      continue;
    } else {
      memcpy(&ctl->server_list[i].client, client, sizeof(*client));
      printf("CLIENT CONNECTED %s\n", inet_ntoa(client->sin_addr));
      sendto(sfd, &ctl->server_list[i].port, sizeof(ctl->server_list[i].port), 0, (struct sockaddr *) client, sizeof(*client));
      ctl->server_list[i].is_bussy = 0;
      printf("%d\n", ctl->server_list[i].sfd);
      break;
    }
  }
}

void *ServerRoutine(void* argv) {
  Server    *server = (Server *) argv;
  char      buf[BUF_MAX] = {0};
  socklen_t cl_size = sizeof(server->client);
  while(1) {
    while(!server->is_bussy){
      printf("%d\n", server->sfd);
      recvfrom(server->sfd, buf, sizeof(buf), 0, (struct sockaddr *) &server->client, &cl_size);
      printf("CLIENT SEND %s\n", buf);
      if(0 == strncmp(buf, "time", sizeof(buf))) {
        GetTime(buf);
        sendto(server->sfd, buf, sizeof(buf), 0, (struct sockaddr *) &server->client, cl_size);
      } else if (0 == strncmp(buf, "exit", sizeof(buf))) {
        server->is_bussy == 0;
      }
    }
  }

  close(server->sfd);
  pthread_exit(EXIT_SUCCESS);
}

void CreateServers(ServerCtl *ctl) {
  struct sockaddr_in serv;
  int                sfd, addr, res;
  ctl->port = SERV_PORT + 1;
  for(int i = 0; i < SERV_COUNT; i++) {
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sfd == -1) {
      handle_error("socket error:");
    }

    inet_pton(AF_INET, SERV_ADDR, &addr);
    memset(&serv, 0, sizeof(serv));
    serv.sin_addr.s_addr = addr;
    serv.sin_family = AF_INET;
    while(1){
      serv.sin_port = htons(ctl->port);
      res = bind(sfd, (struct sockaddr *) &serv, sizeof(serv));
      if(res == -1 && errno == EADDRINUSE) {
        ctl->port++;
        continue;
      } else if(res == -1) {
        handle_error("bind error:");
      }
      break;
    }
    ctl->server_list[i].sfd  = sfd;
    ctl->server_list[i].port = ctl->port;
    ctl->server_list[i].is_bussy = 0;
  }
}

void RunServers(ServerCtl *ctl) {
  for(int i = 0; i < SERV_COUNT; i++) {
    pthread_create(&ctl->server_list[i].thread, NULL, ServerRoutine, (void *) &ctl->server_list[i]);
  }
}

void JoinServers(ServerCtl *ctl) {
  for(int i = 0; i < SERV_COUNT; i++) {
    pthread_join(ctl->server_list[i].thread, NULL);
  }
}

int main() {
  struct sockaddr_in serv, client;
  ServerCtl          ctl;
  int                sfd, addr;
  char               buf[BUF_MAX];

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sfd == -1) {
    handle_error("socket error:");
  }
  
  inet_pton(AF_INET, SERV_ADDR, &addr);
  memset(&serv, 0, sizeof(serv));
  serv.sin_port = htons(SERV_PORT);
  serv.sin_addr.s_addr = addr;
  serv.sin_family = AF_INET;

  if(bind(sfd, (struct sockaddr *) &serv, sizeof(serv)) == -1) {
    handle_error("bind error:");
  }

  CreateServers(&ctl);
  RunServers(&ctl);
  printf("SERVER START\n");

  socklen_t cl_size = sizeof(client);
  while(!stop) {
    memset(&client, 0, sizeof(client));
    recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &cl_size);
    ClientRedirection(&ctl, sfd, &client);
  }

  JoinServers(&ctl);

  exit(EXIT_SUCCESS);
}