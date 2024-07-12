#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

void handler(int signal) {
  printf("my signal - %d\n", signal);
}

int main() {
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGUSR1, &sa, NULL);
  while(1);

  return 0;
}