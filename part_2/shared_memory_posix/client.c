#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/mman.h>

#define errExit(msg)                                               \
  do {                                                             \
    fprintf(stderr, "%d %s %s\n", __LINE__, msg, strerror(errno)); \
    exit(EXIT_FAILURE);                                            \
  } while (0)

typedef struct {
  char text[1024];
} Message;

int main() {
  int shmfd;
  sem_t *message_sent, *message_delivered;

  shmfd = shm_open("/shm_message", O_RDWR, 0);
  if (shmfd == -1)
    errExit("shm_open");
  
  if (ftruncate(shmfd, sizeof(Message)) == -1)
    errExit("ftruncate");

  Message *message = mmap(NULL, sizeof(*message),
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED, shmfd, 0);
  if (message == MAP_FAILED)
    errExit("mmap");

  message_sent = sem_open("/message_sent", O_RDWR);
  if(message_sent == SEM_FAILED)
    errExit("sem_open");
  message_delivered = sem_open("/message_delivered", O_RDWR);
  if(message_delivered == SEM_FAILED)
    errExit("sem_open");

  sem_wait(message_sent);

  printf("%s\n", message->text);

  memcpy(message->text, "Hello!", 7);

  sem_post(message_delivered);
  munmap(message, sizeof(*message));

  sem_close(message_sent);
  sem_close(message_delivered);

  exit(EXIT_SUCCESS);
}