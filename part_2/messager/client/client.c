#include "threads/threads.h"

pthread_mutex_t g_mutex_ncurses = PTHREAD_MUTEX_INITIALIZER;
char g_stop;
int g_new_user_flag = 1;

void SigWinch(int signo) {
  struct winsize size;
  pthread_mutex_lock(&g_mutex_ncurses);
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
  pthread_mutex_unlock(&g_mutex_ncurses);
}

int Register(User *user) {
  char status = SUCCESS;
  struct mq_attr attr = {0, 10, sizeof(Message), 0};
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  mqd_t register_mq = mq_open(REGISTER_MQ, O_WRONLY);
  if (register_mq < 0) {
    PrintError("mq_open", __LINE__);
    return FAILURE;
  }

  char temp[BUF_MAX] = {0};
  temp[0] = '/';
  strcpy(temp + 1, user->name);
  strcpy(user->name, temp);
  printf("%s\n", user->name);
  user->user_mq = mq_open(user->name, flags, mode, &attr);
  if (user->user_mq < 0) {
    PrintError("mq_open", __LINE__);
    status = FAILURE;
  } else {
    int read_bytes;
    read_bytes = mq_send(register_mq, user->name, strlen(user->name), 0);
    if (read_bytes < 0) {
      PrintError("mq_send", __LINE__);
      status = FAILURE;
      goto Error;
    }

    do {
      read_bytes = mq_receive(user->user_mq, &status, BUF_MAX, 0);
      if (read_bytes < 0) {
        if (errno == EAGAIN) {
          continue;
        } else {
          PrintError("mq_receive", __LINE__);
          status = FAILURE;
          break;
        }
      }
    } while (read_bytes < 0);
  }
Error:
  if (status == FAILURE && user->user_mq != -1) {
    if (mq_close(user->user_mq) < 0) {
      PrintError("mq_close", __LINE__);
      status = FAILURE;
    }
    if (mq_unlink(user->name) < 0) {
      PrintError("mq_unlink", __LINE__);
      status = FAILURE;
    }
  }
  if (mq_close(register_mq) < 0) {
    PrintError("mq_close", __LINE__);
    status = FAILURE;
  }

  return status;
}

int main(int argc, char **argv) {
  User user = {0};
  UserList user_list = {0};
  MessageList message_list = {0};
  MessagerController controller = {&user, &user_list, &message_list};

  pthread_t message_sender;
  pthread_t message_receiver;
  pthread_t user_receiver;

  if (argc <= 1) {
    fprintf(stderr, "Please input ./client 'name'\n");
    exit(EXIT_FAILURE);
  } else {
    strncpy(user.name, argv[1], USERNAME_MAX);
  }

  pthread_mutex_init(&user_list.mutex, NULL);
  pthread_mutex_init(&message_list.mutex, NULL);

  int status = Register(&user);
  if (status != FAILURE) {
    initscr();
    signal(SIGWINCH, SigWinch);
    cbreak();

    pthread_create(&message_sender, NULL, MessageSender, &controller);
    pthread_create(&message_receiver, NULL, MessageReceiver, &controller);
    pthread_create(&user_receiver, NULL, UserReceiver, &controller);

    pthread_join(message_sender, NULL);
    pthread_join(message_receiver, NULL);
    pthread_join(user_receiver, NULL);

    endwin();
  }
  pthread_mutex_destroy(&user_list.mutex);
  pthread_mutex_destroy(&message_list.mutex);
  exit(EXIT_SUCCESS);
}