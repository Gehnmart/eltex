#include "threads.h"

int AddUser(ServerController *controller, char *username, mqd_t user_mq) {
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

int DelUser(ServerController *controller, char *username) {
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
      memset((char *)user, 0, sizeof(user));
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

int AddMessage(ServerController *controller, Message *message) {
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