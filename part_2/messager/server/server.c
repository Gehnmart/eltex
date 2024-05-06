#include "server.h"

#include "../general_resource.h"

char g_stop;

int AddMessage(MessagerController *controller, Message *message) {
  MessageList *message_list = controller->message_list;
  pthread_mutex_lock(&message_list->mutex);
  if (message_list->messages[message_list->len].text[0] == 0) {
    strncpy(message_list->messages[message_list->len].text, message->text,
            MESSAGE_LEN_MAX - 1);
    strncpy(message_list->messages[message_list->len].user, message->user,
            USERNAME_MAX - 1);
    strncpy(message_list->messages[message_list->len].metadata.data_all,
            message->metadata.data_all, USERNAME_MAX);
  }
  message_list->len++;
  pthread_mutex_unlock(&message_list->mutex);

  return SUCCESS;
}

int AddUser(MessagerController *controller, char *username, mqd_t user_mq) {
  UserList *user_list = controller->user_list;
  char ret_status = FAILURE;
  pthread_mutex_lock(&user_list->mutex);
  for (int i = 0; i < USER_MAX; ++i) {
    User *user = &user_list->users[i];
    if (user->name[0] == 0) {
      strncpy(user->name, username, strlen(username));
      user->user_mq = user_mq;
      ret_status = SUCCESS;
      printf("INFO USER '%s' ADDED\n", username);
      break;
    }
  }
  pthread_mutex_unlock(&user_list->mutex);
  Message message = {0};
  snprintf(message.text, MESSAGE_LEN_MAX, "User '%s' connected", username);
  strncpy(message.user, "SERVER", USERNAME_MAX);
  message.metadata.data.type = APPEND_USER;
  strncpy(message.metadata.data.username, username, USERNAME_MAX);
  AddMessage(controller, &message);

  return ret_status;
}

int DelUser(MessagerController *controller, char *username) {
  UserList *user_list = controller->user_list;
  char ret_status = FAILURE;
  pthread_mutex_lock(&user_list->mutex);
  for (int i = 0; i < USER_MAX; ++i) {
    User *user = &user_list->users[i];
    if (user->name[0] == 0) {
      continue;
    }
    if (strncmp(user->name, username, NAME_MAX - 1) == 0) {
      mq_close(user->user_mq);
      memset(user->name, 0, sizeof(user->name));
      user->user_mq = 0;
      user->last_receive_msg = 0;
      printf("INFO USER '%s' DELETED\n", username);
      break;
    }
  }
  pthread_mutex_unlock(&user_list->mutex);
  Message message = {0};
  snprintf(message.text, MESSAGE_LEN_MAX, "User '%s' leaved;", username);
  strncpy(message.user, "SERVER", USERNAME_MAX);
  message.metadata.data.type = DELETE_USER;
  strncpy(message.metadata.data.username, username, USERNAME_MAX);
  AddMessage(controller, &message);
  return ret_status;
}

void *RegisterHandler(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  struct mq_attr attr = {0, 10, sizeof(Message), 0};
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  mqd_t register_mq = mq_open(REGISTER_MQ, flags, mode, &attr);
  if (register_mq < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
    g_stop = 1;
    return NULL;
  }

  while (!g_stop) {
    char username[USERNAME_MAX] = {0};
    int slen = mq_receive(register_mq, (char *)&username, BUF_MAX, NULL);
    if (slen < 0) {
      if (errno == EAGAIN) {
        usleep(1000);
        continue;
      } else {
        fprintf(stderr, "ERROR LINE-%d mq_receive: %s\n", __LINE__,
                strerror(errno));
        g_stop = 1;
        break;
      }
    }

    mqd_t user_mq = mq_open(username, O_RDWR);
    if (user_mq < 0) {
      fprintf(stderr, "INFO LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
      continue;
    }

    char status = AddUser(controller, username, user_mq);
    if (mq_send(user_mq, &status, sizeof(status), 0) < 0) {
      fprintf(stderr, "INFO LINE-%d mq_send: %s\n", __LINE__, strerror(errno));
      if (mq_close(user_mq) < 0) {
        fprintf(stderr, "ERROR LINE-%d mq_close: %s\n", __LINE__,
                strerror(errno));
      }
    }
  }

  if (mq_close(register_mq) < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_close: %s\n", __LINE__, strerror(errno));
  }
  if (mq_unlink(REGISTER_MQ) < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_unlink: %s\n", __LINE__, strerror(errno));
  }

  exit(EXIT_SUCCESS);
}

void *MessageHandler(void *argv) {
  MessagerController *controller = (MessagerController *)argv;
  struct mq_attr attr = {0, 10, sizeof(Message), 0};
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  mqd_t chat_mq = mq_open(CHAT_MQ, flags, mode, &attr);
  if (chat_mq < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
  } else {
    while (!g_stop) {
      Message message = {0};
      int slen = mq_receive(chat_mq, (char *)&message, sizeof(Message), NULL);
      if (slen < 0) {
        if (errno == EAGAIN) {
          continue;
        } else {
          fprintf(stderr, "ERROR LINE-%d mq_receive: %s\n", __LINE__,
                  strerror(errno));
          g_stop = 1;
          break;
        }
      }
      if (strncmp(message.text, "/exit", 5) == 0) {
        DelUser(controller, message.user);
      } else {
        printf("INFO RECEIVED MESSAGE '%s' FROM USER '%s'\n", message.text,
               message.user);
        AddMessage(controller, &message);
      }
    }
  }
  if (mq_close(chat_mq) < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_close: %s\n", __LINE__, strerror(errno));
  }
  if (mq_unlink(CHAT_MQ) < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_unlink: %s\n", __LINE__, strerror(errno));
  }

  return NULL;
}

int SendMessagesToUser(MessagerController *controller, User *user) {
  while (user->last_receive_msg < controller->message_list->len) {
    printf("INFO MESSAGE '%s' SEND TO '%s'\n",
           controller->message_list->messages[user->last_receive_msg].text,
           user->name);
    mq_send(user->user_mq,
            (char *)&controller->message_list->messages[user->last_receive_msg],
            sizeof(Message), 0);
    user->last_receive_msg++;
  }

  return SUCCESS;
}

void *MessageSender(void *argv) {
  MessagerController *controller = (MessagerController *)argv;
  UserList *user_list = controller->user_list;

  while (!g_stop) {
    pthread_mutex_lock(&user_list->mutex);
    for (int i = 0; i < USER_MAX; ++i) {
      User *user = &user_list->users[i];
      if (user->name[0] != 0 && controller->message_list->len > 0) {
        SendMessagesToUser(controller, user);
      }
    }
    pthread_mutex_unlock(&user_list->mutex);
    usleep(1000);
  }

  return NULL;
}

int main() {
  UserList user_list = {0};
  MessageList message_list = {0};
  MessagerController controller = {&user_list, &message_list};

  pthread_mutex_init(&user_list.mutex, NULL);
  pthread_mutex_init(&message_list.mutex, NULL);

  pthread_t pt_registration;
  pthread_t pt_receiver;
  pthread_t pt_sender;

  pthread_create(&pt_registration, NULL, RegisterHandler, &controller);
  pthread_create(&pt_receiver, NULL, MessageHandler, &controller);
  pthread_create(&pt_sender, NULL, MessageSender, &controller);
  getchar();
  g_stop = 1;
  pthread_join(pt_registration, NULL);
  pthread_join(pt_receiver, NULL);
  pthread_join(pt_sender, NULL);

  pthread_mutex_destroy(&user_list.mutex);
  pthread_mutex_destroy(&message_list.mutex);

  exit(EXIT_SUCCESS);
}