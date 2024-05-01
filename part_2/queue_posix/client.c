#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#define BUF_MAX 256

int main() {
  int msqid;
  mqd_t mqd;
  unsigned int prior = 1;

  struct mq_attr attr = {0, 10, BUF_MAX, 0};

  mqd = mq_open("/my_queue", O_CREAT | O_RDWR, 0666, &attr);
  if(mqd < 0){
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  char buf[BUF_MAX] = {0};
  mq_receive(mqd, buf, BUF_MAX, &prior);
  printf("%s", buf);

  mq_send(mqd, "Hello!", 7, 2);

  mq_close(mqd);
  mq_unlink("/my_queue");

  exit(EXIT_SUCCESS);
}