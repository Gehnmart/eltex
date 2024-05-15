#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/stat.h>
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

  shmfd = shm_open("/shm_message", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (shmfd == -1)
    errExit("shm_open");
  
  if (ftruncate(shmfd, sizeof(Message)) == -1)
    errExit("ftruncate");

  Message *message = mmap(NULL, sizeof(*message),
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED, shmfd, 0);
  if (message == MAP_FAILED)
    errExit("mmap");

  message_sent = sem_open("/message_sent", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
  if(message_sent == SEM_FAILED)
    errExit("sem_open");
  message_delivered = sem_open("/message_delivered", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
  if(message_delivered == SEM_FAILED)
    errExit("sem_open");

  memcpy(message->text, "Hi!", 4);

  sem_post(message_sent);
  sem_wait(message_delivered);

  printf("%s\n", message->text);
  munmap(message, sizeof(*message));
  shm_unlink("/shm_message");

  sem_close(message_sent);
  sem_close(message_delivered);
  sem_unlink("/message_sent");
  sem_unlink("/message_delivered");

  exit(EXIT_SUCCESS);
}