#include "../additional.h"
#include <ncurses.h>
#include <getopt.h>

void SigWinch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

void ParceArgv(int argc, char **argv, char *selfname) {
  int opt, status = FAILURE;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [-n name(max len 32)]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  while((opt = getopt(argc, argv, "n:")) != -1) {
    switch(opt) {
      case 'n':
        if(strlen(optarg) > USERNAME_MAX - 1) {
          fprintf(stderr, "username max len = 32\n");
          exit(EXIT_FAILURE);
        } else {
          strncpy(selfname, optarg, USERNAME_MAX);
          break;
        }
      default:
        fprintf(stderr, "Usage: %s [-n name(max len 32)]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv) {
  MessagerCtl ctl = {0};

  ParceArgv(argc, argv, ctl.selfname);

  /*messager controller initialization*/
  ctl.shmfd = shm_open(SHARED_DATA_NAME, O_RDWR, 0);
  if (ctl.shmfd == -1)
    errExit("shm_open");
  
  if (ftruncate(ctl.shmfd, sizeof(*ctl.shared_data)) == -1)
    errExit("ftruncate");

  ctl.shared_data = mmap(NULL, sizeof(*ctl.shared_data),
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED, ctl.shmfd, 0);
  if (ctl.shared_data == MAP_FAILED)
    errExit("mmap");
  
  ctl.sem_server = sem_open(SEM_SERVER_NAME, O_RDWR);
  if(ctl.sem_server == SEM_FAILED)
    errExit("sem_open");
  ctl.sem_client = sem_open(SEM_CLIENT_NAME, O_RDWR);
  if(ctl.sem_client == SEM_FAILED)
    errExit("sem_open");
  /*running all threads*/

  

  /*free resources*/
  munmap(ctl.shared_data, sizeof(*ctl.shared_data));

  sem_close(ctl.sem_client);
  sem_close(ctl.sem_server);
  
  /*exited*/
  printf("exit success\n");
  exit(EXIT_SUCCESS);
}