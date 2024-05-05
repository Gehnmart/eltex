#include "server.h"

#include "../general_resource.h"

char g_exit = 1;

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
      printf("%s\n", username);
      break;
    }
  }
  pthread_mutex_unlock(&user_list->mutex);

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
      close(user->user_mq);
      memset(user->name, 0, sizeof(user->name));
      user->user_mq = 0;
      printf("User %s deleted\n", username);
      break;
    }
  }
  pthread_mutex_unlock(&user_list->mutex);

  return ret_status;
}

void *RegisterHandler(void *argv) {
  MessagerController *controller = (MessagerController *)argv;
  struct mq_attr attr;
  attr.mq_msgsize = sizeof(Message);
  attr.mq_maxmsg = 10;
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  mqd_t register_mq = mq_open(REGISTER_MQ, flags, mode, &attr);
  if (register_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  while (g_exit) {
    char username[USERNAME_MAX] = {0};
    int slen = mq_receive(register_mq, (char *)&username, BUF_MAX, NULL);
    if (slen < 0) {
      if (errno == EAGAIN) {
        sleep(1);
        continue;
      } else {
        perror("Register() mq_receive:");
      }
    }

    mqd_t user_mq = mq_open(username, O_RDWR);
    if (user_mq < 0) {
      perror("Register() mq_open2:");
      continue;
    }

    char status = AddUser(controller, username, user_mq);
    if (mq_send(user_mq, &status, sizeof(status), 0) < 0) {
      perror("Register() mq_send:");
    }
  }

  if (mq_close(register_mq) < 0) {
    perror("mq_close register_mq");
  }
  if (mq_unlink(REGISTER_MQ) < 0) {
    perror("mq_unlink register_mq");
  }

  exit(EXIT_SUCCESS);
}

int AddMessage(MessagerController *controller, Message *message) {
  MessageList *message_list = controller->message_list;
  pthread_mutex_lock(&message_list->mutex);
  if (message_list->messages[message_list->len].message[0] == 0) {
    strncpy(message_list->messages[message_list->len].message, message->message,
            MESSAGE_LEN_MAX - 1);
    strncpy(message_list->messages[message_list->len].user, message->user,
            USERNAME_MAX - 1);
  }
  message_list->len++;
  pthread_mutex_unlock(&message_list->mutex);

  return SUCCESS;
}

void *MessageHandler(void *argv) {
  MessagerController *controller = (MessagerController *)argv;
  struct mq_attr attr = {0};
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = BUF_MAX;
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  mqd_t chat_mq = mq_open(CHAT_MQ, flags, mode, &attr);
  if (chat_mq < 0) {
    perror("mq_open");
  } else {
    while (g_exit) {
      Message message;
      int slen = mq_receive(chat_mq, (char *)&message, BUF_MAX, NULL);
      if (slen < 0) {
        if (errno == EAGAIN) {
          continue;
        } else {
          perror("Register() mq_receive:");
        }
      }
      if (strncmp(message.message, "/exit", 5) == 0) {
        DelUser(controller, message.user);
        AddMessage(controller, &message);
      } else {
        printf("Received message %s from %s\n", message.message, message.user);
        AddMessage(controller, &message);
      }
    }
  }
  if (mq_close(chat_mq) < 0) {
    perror("mq_close chat_mq");
  }
  if (mq_unlink(CHAT_MQ) < 0) {
    perror("mq_unlink chat_mq");
  }

  return NULL;
}

int SendMessagesToUser(MessagerController *controller, User *user) {
  while (user->last_receive_msg < controller->message_list->len) {
    printf("Message send to %s mq %d\n  %s\n", user->name, user->user_mq,
           controller->message_list->messages[user->last_receive_msg].message);
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

  while (g_exit) {
    for (int i = 0; i < USER_MAX; ++i) {
      User *user = &user_list->users[i];
      if (user->name[0] != 0 && controller->message_list->len > 0) {
        SendMessagesToUser(controller, user);
      }
    }
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
  g_exit = 0;
  pthread_join(pt_registration, NULL);
  pthread_join(pt_receiver, NULL);
  pthread_join(pt_sender, NULL);

  exit(EXIT_SUCCESS);
}