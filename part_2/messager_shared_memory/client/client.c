#include "../additional.h"
#include "threads/threads.h"
#include <ncurses.h>
#include <getopt.h>

pthread_mutex_t g_ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;

void SigWinch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

int AddSelf(MessagerCtl *ctl) {
  sem_wait(ctl->sem_client);
  strncpy(ctl->shared_data->usr_list.list[ctl->shared_data->usr_list.len].username,
          ctl->selfname, USERNAME_MAX);
  ctl->shared_data->usr_list.list[ctl->shared_data->usr_list.len].status = STAT_JOIN;
  ctl->shared_data->usr_list.len++;
  sem_post(ctl->sem_client);
}

void ParceArgv(int argc, char **argv, MessagerCtl *ctl) {
  int opt, status = FAILURE;
  if (argc != 3) {
    fprintf(stderr, "Usage: %s [-n name(max len 32)]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  while((opt = getopt(argc, argv, "n:")) != -1) {
    switch(opt) {
      case 'n':
        if(strlen(optarg) > USERNAME_MAX - 1 || strlen(optarg) < 2) {
          fprintf(stderr, "username max len = 32\n");
          exit(EXIT_FAILURE);
        } else {
          strncpy(ctl->selfname, optarg, USERNAME_MAX);
          break;
        }
      default:
        fprintf(stderr, "Usage: %s [-n name(max len 32)]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv) {
  pthread_t   th_sender; /*thread processes user input and send data to server*/
  pthread_t   th_msg_receiver; /*thread processes rendering message list in chat win*/
  pthread_t   th_usr_receiver; /*thread processes rendering user list in users win*/
  MessagerCtl ctl = {0};

  ParceArgv(argc, argv, &ctl);

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
  
  AddSelf(&ctl);
  /*init ncurses*/
  initscr();
  signal(SIGWINCH, SigWinch);
  cbreak();
  
  /*running all threads*/
  pthread_create(&th_sender, NULL, MessageSender, (void *)&ctl);
  pthread_create(&th_msg_receiver, NULL, MessageReceiver, (void *)&ctl);
  pthread_create(&th_usr_receiver, NULL, UserReceiver, (void *)&ctl);

  pthread_join(th_sender, NULL);
  pthread_join(th_msg_receiver, NULL);
  pthread_join(th_usr_receiver, NULL);
  /*free resources*/

  munmap(ctl.shared_data, sizeof(*ctl.shared_data));

  sem_close(ctl.sem_client);
  sem_close(ctl.sem_server);
  
  endwin();
  /*exited*/
  printf("exit success\n");
  exit(EXIT_SUCCESS);
}