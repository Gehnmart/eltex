#include "threads.h"

void *RegisterHandler(void *argv) {
  ServerController *controller = (ServerController *)argv;

  struct mq_attr attr = {0, 10, sizeof(Message), 0};
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  mqd_t register_mq = mq_open(REGISTER_MQ, flags, mode, &attr);
  if (register_mq < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
    controller->server_stop = 1;
    return NULL;
  }

  while (!controller->server_stop) {
    char username[USERNAME_MAX] = {0};
    int slen = mq_receive(register_mq, (char *)&username, BUF_MAX, NULL);
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

  return NULL;
}