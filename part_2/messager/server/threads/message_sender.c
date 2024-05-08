#include "threads.h"

int SendMessagesToUser(ServerController *controller, User *user) {
  while (user->last_receive_msg < controller->message_list->len) {
    usleep(100);
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
  ServerController *controller = (ServerController *)argv;
  UserList *user_list = controller->user_list;

  while (!controller->server_stop) {
    usleep(100);
    pthread_mutex_lock(&user_list->mutex);
    for (int i = 0; i < USER_MAX; ++i) {
      User *user = &user_list->users[i];
      if (user->name[0] != 0 && controller->message_list->len > 0) {
        SendMessagesToUser(controller, user);
      }
    }
    pthread_mutex_unlock(&user_list->mutex);
  }

  return NULL;
}