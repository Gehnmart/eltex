#include <signal.h>
#include <stdio.h>

void handler(int signal) {
  printf("my signal - %d\n", signal);
}

int main() {
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sigprocmask(SIG_BLOCK, &set, NULL);
  while(1);

  return 0;
}