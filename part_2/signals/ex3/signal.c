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
  while(1) {
    sigwait(&set, &sig);
    printf("Ura\n");
  }

  return 0;
}
