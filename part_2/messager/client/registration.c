#include "registration.h"

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