#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define errExit(msg)                                               \
  do {                                                             \
    fprintf(stderr, "%d %s %s\n", __LINE__, msg, strerror(errno)); \
    exit(EXIT_FAILURE);                                            \
  } while (0)

typedef struct {
  char text[1024];
} Message;

void SemOp(int semid, int op) {
  struct sembuf buf = {0};
  buf.sem_op = op;
  if (semop(semid, &buf, 1) == -1) errExit("semop");
}

int main() {
  int semid, shmid;
  key_t semkey, shmkey;
  
  semkey = ftok("./server.c", 'A');
  if (semkey == -1) errExit("ftok");

  shmkey = ftok("./server.c", 'B');
  if (shmkey == -1) errExit("ftok");

  shmid = shmget(shmkey, sizeof(Message), 0666);
  if (shmid == -1) errExit("shmget");

  semid = semget(semkey, 1, 0666);
  if (semid == -1) errExit("semget");

  Message *message = shmat(shmid, NULL, 0);
  if (message == (void *)-1) errExit("shmat");

  SemOp(semid, -1);

  printf("%s\n", message->text);

  memcpy(message->text, "Hello!", 7);

  SemOp(semid, 2);

  exit(EXIT_SUCCESS);
}