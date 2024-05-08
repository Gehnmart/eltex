#include "client_controller.h"
#include "registration.h"
#include "threads/threads.h"

void SigWinch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char **argv) {
  ClientController controller;
  pthread_t message_sender;
  pthread_t message_receiver;
  pthread_t user_receiver;

  if (argc > 1) {
    ControllerInit(&controller);

    strncpy(controller.user->name, argv[1], USERNAME_MAX);
    int status = Register(controller.user);
    if (status != FAILURE) {
      initscr();
      signal(SIGWINCH, SigWinch);
      cbreak();

      pthread_create(&message_sender, NULL, MessageSender, &controller);
      pthread_create(&message_receiver, NULL, MessageReceiver, &controller);
      pthread_create(&user_receiver, NULL, UserReceiver, &controller);

      pthread_join(message_sender, NULL);
      pthread_join(message_receiver, NULL);
      pthread_join(user_receiver, NULL);

      endwin();
    }

    ControllerDestroy(&controller);
  } else {
    fprintf(stderr, "Please input ./client 'name'\n");
  }

  exit(EXIT_SUCCESS);
}