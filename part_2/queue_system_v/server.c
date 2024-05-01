#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct {
  long mtype;
  char mtext[64];
} Message;

int main() {
  int msqid;
  key_t key;

  if((key = ftok("server.c", 'B')) < 0) {
    perror("ftok error");
    exit(EXIT_FAILURE);
  }
  
  if((msqid = msgget(key, IPC_CREAT|0666)) < 0) {
    perror("msgget error");
    exit(EXIT_FAILURE);
  }
  
  Message msgs = {1, "Hi!"};
  msgsnd(msqid, &msgs, sizeof(msgs), 0);

  Message msgr;
  msgrcv(msqid, &msgr, sizeof(msgr), 1, 0);
  printf("%s\n", msgr.mtext);

  msgctl(msqid, IPC_RMID, NULL);


  exit(EXIT_SUCCESS);
}