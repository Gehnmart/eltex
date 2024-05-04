#include "server.h"

#include "../general_resource.h"

char g_exit = 1;

int AddUser(MessagerController *controller, char *username, mqd_t user_mq) {
  UserList *user_list = controller->user_list;
  char ret_status = FAILURE;
  pthread_mutex_lock(&user_list->mutex);
  for (int i = 0; i < USER_MAX; ++i) {
    User *user = &user_list->users[i];
    if (user->name[0] != 0) {
      continue;
    }
    strncpy(user->name, username, strlen(username));
    user->user_mq = user_mq;
    ret_status = SUCCESS;
    printf("%s\n", username);
    break;
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
    if(strncmp(user->name, username, NAME_MAX - 1) == 0){
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
  struct mq_attr attr = {0, 10, BUF_MAX, 0};

  mqd_t register_mq =
      mq_open(REGISTER_MQ, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);
  if (register_mq < 0) {
    perror("Register() mq_open1:");
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

    mqd_t user_mq = mq_open(username, O_WRONLY);
    if (user_mq < 0) {
      perror("Register() mq_open2:");
      continue;
    }

    char status = AddUser(controller, username, user_mq);
    mq_send(user_mq, &status, BUF_MAX, 0);
  }

  mq_close(register_mq);
  mq_unlink(REGISTER_MQ);

  exit(EXIT_SUCCESS);
}

int AddMessage(MessagerController *controller, Message *message) {
  MessageList *message_list = controller->message_list;
  pthread_mutex_lock(&message_list->mutex);
  for (int i = 0; i < MESSAGE_MAX; ++i) {
    if (message_list->messages[i].message[0] == 0) {
      strncpy(message_list->messages[i].message, message->message,
              MESSAGE_LEN_MAX - 1);
      strncpy(message_list->messages[i].user, message->user, USERNAME_MAX - 1);
      break;
    }
  }
  message_list->len++;
  pthread_mutex_unlock(&message_list->mutex);

  return SUCCESS;
}

void *MessageHandler(void *argv) {
  MessagerController *controller = (MessagerController *)argv;
  struct mq_attr attr = {0, 10, BUF_MAX, 0};
  mqd_t chat_mq = mq_open(CHAT_MQ, O_CREAT | O_RDWR, 0666, &attr);
  if (chat_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }
  while (g_exit) {
    Message message;
    int slen = mq_receive(chat_mq, (char *)&message, BUF_MAX, NULL);
    if (slen < 0) {
      if (errno == EAGAIN) {
        usleep(1000);
        continue;
      } else {
        perror("Register() mq_receive:");
      }
    }
    if(strncmp(message.message, "/exit", 5) == 0) {
      DelUser(controller, message.user);
    } else {
      printf("Получил сообщение %s от %s\n", message.message, message.user);
      AddMessage(controller, &message);
    }
  }

  mq_close(chat_mq);
  mq_unlink(CHAT_MQ);

  exit(EXIT_SUCCESS);
}

int SendMessagesToUser(MessagerController *controller, User *user) {
  while (user->last_receive_msg < controller->message_list->len) {
    printf("Message send to %s\n  %s\n", user->name, controller->message_list->messages[user->last_receive_msg].message);
    mq_send(user->user_mq,
            (char *)&controller->message_list->messages[user->last_receive_msg],
            BUF_MAX, 0);
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
  exit(EXIT_SUCCESS);
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