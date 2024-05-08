#include "threads.h"

void *MessageReceiver(void *argv) {
  ServerController *controller = (ServerController *)argv;
  struct mq_attr attr = {0, 10, sizeof(Message), 0};
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  mqd_t chat_mq = mq_open(CHAT_MQ, flags, mode, &attr);
  if (chat_mq < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
  } else {
    while (!controller->server_stop) {
      Message message = {0};
      int slen = mq_receive(chat_mq, (char *)&message, sizeof(Message), NULL);
      if (slen < 0) {
        if (errno == EAGAIN) {
          usleep(10000);
          continue;
        } else {
          fprintf(stderr, "ERROR LINE-%d mq_receive: %s\n", __LINE__,
                  strerror(errno));
          controller->server_stop = 1;
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