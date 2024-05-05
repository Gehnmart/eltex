#include "client.h"
#include "../general_resource.h"

pthread_mutex_t g_mutex_ncurses = PTHREAD_MUTEX_INITIALIZER;
int g_stop = 0;

int Register(User *user) {
  WINDOW *register_win;
  char status = SUCCESS;
  mqd_t register_mq;
  struct mq_attr attr;
  attr.mq_msgsize = sizeof(Message);
  attr.mq_maxmsg = 10;

  register_mq = mq_open(REGISTER_MQ, O_WRONLY);
  if (register_mq < 0) {
    perror("Register() mq_open:");
    return FAILURE;
  }
  register_win = newwin(3, USERNAME_MAX, LINES / 2, COLS / USERNAME_MAX / 2);

  while (!g_stop) {
    box(register_win, 0, 0);
    wrefresh(register_win);
    curs_set(TRUE);
    wmove(register_win, 0, 1);
    wprintw(register_win, "Enter the username:");
    wmove(register_win, 1, 1);
    wgetnstr(register_win, user->name, USERNAME_MAX - 1);
    curs_set(FALSE);
    wrefresh(register_win);

    if (strncmp(user->name, "/exit", 6) == 0) {
      status = FAILURE;
      break;
    }

    char temp[BUF_MAX] = {0};
    temp[0] = '/';
    strcpy(temp + 1, user->name);
    strcpy(user->name, temp);
    printf("%s\n", user->name);
    user->user_mq = mq_open(user->name, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);
    if (user->user_mq < 0) {
      perror("mq_open");
      status = FAILURE;
      break;
    }

    mq_send(register_mq, user->name, strlen(user->name), 0);
    
    int bytes;
    do {
      bytes = mq_receive(user->user_mq, &status, BUF_MAX, 0);
      if (bytes < 0) {
        if(errno == EAGAIN)
          continue;
        status = FAILURE;
        perror("mq_receive");
        break;
      }
    } while(bytes < 0);
    
    if(status == SUCCESS)
      break;
  }

  delwin(register_win);
  clear();
  refresh();
  mq_close(register_mq);

  return status;
}

void *UserReceiver(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  pthread_mutex_lock(&g_mutex_ncurses);
  WINDOW *user_win = newwin(LINES - 3, USERNAME_MAX, 0, COLS - USERNAME_MAX);
  box(user_win, 0, 0);
  wrefresh(user_win);
  pthread_mutex_unlock(&g_mutex_ncurses);
  while(!g_stop) {
    
  }


  pthread_mutex_lock(&g_mutex_ncurses);
  delwin(user_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  return NULL;
}

void *MessageReceiver(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  pthread_mutex_lock(&g_mutex_ncurses);
  WINDOW *chat_win = newwin(LINES - 3, COLS - USERNAME_MAX, 0, 0);
  box(chat_win, 0, 0);
  wrefresh(chat_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  while(!g_stop) {
    Message message = {0};
    int byte_read = mq_receive(controller->user->user_mq, (char *)&message, sizeof(Message), NULL);
    if(byte_read < 0) {
      if(errno == EAGAIN)
        continue;
      perror("mq_receive");
      break;
    }
    memcpy((void *)&controller->message_list->messages[controller->message_list->len], (void *)&message, sizeof(Message));
    controller->message_list->len++;

    pthread_mutex_lock(&g_mutex_ncurses);
    int maxy = getmaxy(chat_win) - 2;
    int list_len = controller->message_list->len;
    int start = list_len > maxy ? list_len - maxy : 0;
    int end = MIN(list_len, maxy + start);
    int index = 1;
    for (int i = start; i < end; i++) {
      wmove(chat_win, index++, 1);
      wprintw(chat_win, "%s: %s", controller->message_list->messages[i].user,
              controller->message_list->messages[i].message);
    }
    wrefresh(chat_win);
    pthread_mutex_unlock(&g_mutex_ncurses);
  }


  pthread_mutex_lock(&g_mutex_ncurses);
  delwin(chat_win);
  pthread_mutex_unlock(&g_mutex_ncurses);
  return NULL;
}

void *MessageSender(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  mqd_t chat_mq = mq_open(CHAT_MQ, O_WRONLY);
  if (chat_mq < 0) {
    perror("mq_open");
    g_stop = 1;
    return NULL;
  }

  pthread_mutex_lock(&g_mutex_ncurses);
  WINDOW *input_win = newwin(3, COLS, LINES - 3, 0);
  box(input_win, 0, 0);
  wrefresh(input_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  char message_buf[MESSAGE_LEN_MAX] = {0};
  Message message = {0};
  strncpy(message.user, controller->user->name, USERNAME_MAX - 1);
  while(!g_stop) {
    pthread_mutex_lock(&g_mutex_ncurses);
    curs_set(TRUE);
    wmove(input_win, 1, 1);
    pthread_mutex_unlock(&g_mutex_ncurses);

    wgetnstr(input_win, message_buf, MESSAGE_LEN_MAX - 1);

    pthread_mutex_lock(&g_mutex_ncurses);
    curs_set(FALSE);
    wclear(input_win);
    box(input_win, 0, 0);
    wrefresh(input_win);
    pthread_mutex_unlock(&g_mutex_ncurses);

    if (strncmp(message_buf, "/exit", MESSAGE_LEN_MAX) == 0) {
      strncpy(message.message, "/exit", 6);
      mq_send(chat_mq, (char *)&message, sizeof(Message), 0);
      g_stop = 1;
      break;
    } else {
      strncpy(message.message, message_buf, MESSAGE_LEN_MAX);
      mq_send(chat_mq, (char *)&message, sizeof(Message), 0);
    }
  }

  mq_close(chat_mq);
  mq_unlink(controller->user->name);
  pthread_mutex_lock(&g_mutex_ncurses);
  delwin(input_win);
  pthread_mutex_unlock(&g_mutex_ncurses);
  
  return NULL;
}

int main() {
  User user = {0};
  UserList user_list = {0};
  MessageList message_list = {0};
  MessagerController controller = {&user, &user_list, &message_list};

  pthread_mutex_init(&user_list.mutex, NULL);
  pthread_mutex_init(&message_list.mutex, NULL);

  pthread_t message_sender;
  pthread_t message_receiver;
  pthread_t user_receiver;

  initscr();
  cbreak();

  int status = Register(&user);
  if(status != FAILURE) {
    pthread_create(&message_sender, NULL, MessageSender, &controller);
    pthread_create(&message_receiver, NULL, MessageReceiver, &controller);
    pthread_create(&user_receiver, NULL, UserReceiver, &controller);

    pthread_join(message_sender, NULL);
    pthread_join(message_receiver, NULL);
    pthread_join(user_receiver, NULL);
  }

  endwin();
  exit(EXIT_SUCCESS);
}