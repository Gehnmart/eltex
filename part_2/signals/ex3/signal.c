#include <signal.h>
#include <stdio.h>

void handler(int signal) {
  printf("my signal - %d\n", signal);
}

int main() {
  sigset_t set;
  int sig;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sigprocmask(SIG_BLOCK, &set, NULL);
  sigwait(&set, &sig);
  printf("Finish\n");

  return 0;
}