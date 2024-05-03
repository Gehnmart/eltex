#include <fcntl.h>
#include <fcntl.h> /* For O_* constants */
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

#include "../user.h"
#include "../message.h"

unsigned AutoIncMsg() {
  static unsigned increment = 0;
  return increment++;
}

unsigned AutoIncUsr() {
  static unsigned increment = 0;
  return increment++;
}

MessageList g_message_list;

void MessageUpdater(mqd_t user_mq) {
  mq_send(user_mq, (char *)&g_message_list.messages[g_message_list.len], BUF_MAX, 0);
}

void *ChatProc(void *argv) {
  UserList *users = (UserList *)argv;
  struct mq_attr attr = {0, 10, BUF_MAX, 0};
  mq_unlink("/chat");
  mqd_t chat_mq = mq_open("/chat", O_CREAT | O_RDWR, 0666, &attr);
  if (chat_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  while(1) {
    Message message = {0};
    ssize_t msg_len = mq_receive(chat_mq, (char *)&message, BUF_MAX, NULL);
    printf("%s\n", message.message);
    if (msg_len == -1) {
      perror("mq_receive");
      continue;
    }

    strncpy(g_message_list.messages[g_message_list.len].message, message.message, MESSAGE_LEN_MAX);
    strncpy(g_message_list.messages[g_message_list.len].user, message.user, USERNAME_MAX);
    g_message_list.messages[g_message_list.len].id = g_message_list.len;
    
    for(int i = 0; i < USER_MAX; ++i) {
      if(users->users[i].name[0] != 0){
        MessageUpdater(users->users[i].user_mq);
      }
    }
    g_message_list.len++;
  }

}

char UserConnect(UserList *list, unsigned id, char *name) {
  for (int i = 0; i < USER_MAX; i++) {
    if (strncmp(list->users[i].name, name, USERNAME_MAX) == 0) {
      return STATUS_NO;
    }
  }
  for (int i = 0; i < USER_MAX; i++) {
    if (list->users[i].name[0] == 0) {
      struct mq_attr attr = {0, 10, BUF_MAX, 0};

      char request_mq[QUEUE_NAME_LEN_MAX] = {0};

      strncpy(request_mq, "/", USERNAME_MAX);
      strncat(request_mq, name, USERNAME_MAX);
      printf("%ld\n", strlen(request_mq));
      mq_unlink(request_mq);
      mqd_t register_mq = mq_open(request_mq, O_CREAT | O_RDWR, 0666, &attr);
      if (register_mq < 0) {
        char errbuf[256];
        sprintf(errbuf, "user \"%s\" mq_open:", name);
        perror(errbuf);
        return STATUS_NO;
      }
      list->users[i].id = id;
      list->users[i].user_mq = register_mq;
      strncpy(list->users[i].name, request_mq, USERNAME_MAX);
      printf("SUCCESSFUL CONNECTED %s|\n", request_mq);
      return STATUS_OK;
    }
  }
  return STATUS_NO;
}

void *RegisterHandler(void *argv) {
  UserList *list = (UserList *)argv;
  struct mq_attr attr = {0, 10, BUF_MAX, 0};
  mq_unlink("/register");
  mqd_t register_mq = mq_open("/register", O_CREAT | O_RDWR, 0666, &attr);
  if (register_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  while (1) {
    UserCreateReq user_request;
    ssize_t msg_len =
        mq_receive(register_mq, (char *)&user_request, BUF_MAX, NULL);
    if (msg_len == -1) {
      perror("mq_receive");
      continue;
    }

    char request_mq[QUEUE_NAME_LEN_MAX] = {0};
    UserCreateRes user_response;

    user_response.id = AutoIncUsr();
    strncpy(user_response.name, user_request.name, USERNAME_MAX);
    strncpy(request_mq, "/r", USERNAME_MAX);
    strncat(request_mq, user_response.name, USERNAME_MAX);
    mqd_t mqdes_client = mq_open(request_mq, O_WRONLY);
    if (mqdes_client < 0) {
      perror("mq_open");
      continue;
    }

    user_response.status =
        UserConnect(list, user_response.id, user_response.name);
    strncpy(user_response.name, request_mq, USERNAME_MAX);
    printf("%s\n", user_response.name);
    mq_send(mqdes_client, (char *)&user_response, BUF_MAX, 0);
    mq_close(mqdes_client);
  }
}

int main() {
  pthread_t register_thread;
  pthread_t chat_thread;
  UserList list = {0};
  pthread_create(&register_thread, NULL, RegisterHandler, &list);
  pthread_create(&chat_thread, NULL, ChatProc, &list);

  pthread_join(chat_thread, NULL);
  pthread_join(register_thread, NULL);
  exit(EXIT_SUCCESS);
}