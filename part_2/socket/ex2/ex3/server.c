#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int stop = 0;
ClientT *list = NULL;
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;

void GetTime(char *buf, int n) {
  time_t rawtime;
  struct tm *timeinfo;
  memset(buf, 0, n);

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  asctime_r(timeinfo, buf);
}

void ClientListInit(const struct sockaddr_in *client) {
  list = (ClientT *)malloc(sizeof(ClientT));
  list->next = NULL;
  list->prev = NULL;
  memcpy(&list->client, client, sizeof(struct sockaddr_in));
}

void ClientListAppend(const struct sockaddr_in *client) {
  ClientT *ptr = list;
  pthread_mutex_lock(&client_list_mutex);
  if (list == NULL) {
    ClientListInit(client);
  } else {
    for (; ptr->next != NULL; ptr = ptr->next);
    ptr->next = (ClientT *)malloc(sizeof(ClientT));
    memcpy(&ptr->next->client, client, sizeof(struct sockaddr_in));
    ptr->next->next = NULL;
    ptr->next->prev = ptr;
  }
  pthread_mutex_unlock(&client_list_mutex);
}

ClientT *GetClient() {
  ClientT *retval = NULL;

  pthread_mutex_lock(&client_list_mutex);
  ClientT *ptr = list;
  if (list != NULL) {
    for (; ptr->next != NULL; ptr = ptr->next);
    if (ptr->prev == NULL) {
      retval = ptr;
      list = NULL;
    } else {
      retval = ptr;
      ptr->prev->next = NULL;
    }
  }
  pthread_mutex_unlock(&client_list_mutex);
  return retval;
}

void ClientListFree(ClientT *list) {
  ClientT *ptr = list;
  for (; ptr != NULL;) {
    ClientT *temp = ptr;
    ptr = ptr->next;
    free(temp);
  }
}

void *ServerRoutine(void *argv) {
  ClientT *client_el = NULL;
  Server *server = (Server *)argv;
  char buf[BUF_MAX] = {0};
  socklen_t cl_size = sizeof(struct sockaddr_in);
  while (!stop) {
    if ((client_el = GetClient(&server->list)) != NULL) {
      GetTime(buf, sizeof(buf));
      sendto(server->sfd, buf, sizeof(buf), 0,
             (struct sockaddr *)&client_el->client, cl_size);
      free(client_el);
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
  ServerCtl *ctl;
};

void *LissenServer(void *argv) {
  struct sockaddr_in client;
  struct LissenParam *param = (struct LissenParam *)argv;
  char buf[BUF_MAX];
  socklen_t cl_size = sizeof(client);
  while (!stop) {
    memset(&client, 0, sizeof(client));
    recvfrom(param->sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client,
             &cl_size);
    printf("CLIENT CONN\n");
    ClientListAppend(&client);
  }

  pthread_exit(EXIT_SUCCESS);
}

int main() {
  struct sockaddr_in serv;
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
  ClientListFree(list);

  exit(EXIT_SUCCESS);
}