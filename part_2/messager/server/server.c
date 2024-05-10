#include "server_controller.h"
#include "threads/threads.h"

int main() {
  ServerController controller;
  int err;

  pthread_t pt_registration;
  pthread_t pt_receiver;
  pthread_t pt_sender;

  ControllerInit(&controller);

  err = pthread_create(&pt_registration, NULL, RegisterHandler, &controller);
  err = pthread_create(&pt_receiver, NULL, MessageReceiver, &controller);
  err = pthread_create(&pt_sender, NULL, MessageSender, &controller);

  char buf[6];
  while(!controller.server_stop){
    fgets(buf, 6, stdin);
    if(strncmp(buf, "/exit", 5) != 0){
      printf("please type /exit for exit\n");
      int c;
      while ((c = getchar()) != '\n' && c != EOF);
    } else {
      controller.server_stop = 1;
    }
  }
  
  pthread_join(pt_registration, NULL);
  pthread_join(pt_receiver, NULL);
  pthread_join(pt_sender, NULL);
  
  ControllerDestroy(&controller);

  exit(EXIT_SUCCESS);
}