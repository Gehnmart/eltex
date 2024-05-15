#include "../additional.h"

int main() {
  pthread_t   th_sender;
  pthread_t   th_receiver;
  MessagerCtl ctl = {0};

  /*messager controller initialization*/
  ctl.shmfd = shm_open(SHARED_DATA_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (ctl.shmfd == -1)
    errExit("shm_open");
  
  if (ftruncate(ctl.shmfd, sizeof(*ctl.shared_data)) == -1)
    errExit("ftruncate");

  ctl.shared_data = mmap(NULL, sizeof(*ctl.shared_data),
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED, ctl.shmfd, 0);
  if (ctl.shared_data == MAP_FAILED)
    errExit("mmap");
  
  ctl.sem_server = sem_open(SEM_SERVER_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
  if(ctl.sem_server == SEM_FAILED)
    errExit("sem_open");
  
  ctl.sem_client = sem_open(SEM_CLIENT_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
  if(ctl.sem_client == SEM_FAILED)
    errExit("sem_open");
  /*running all threads*/


  getchar();
  /*free resources*/
  munmap(ctl.shared_data, sizeof(*ctl.shared_data));
  shm_unlink(SHARED_DATA_NAME);

  sem_close(ctl.sem_server);
  sem_close(ctl.sem_client);
  sem_unlink(SEM_SERVER_NAME);
  sem_unlink(SEM_CLIENT_NAME);

  /*exited*/
  printf("exit success\n");
  exit(EXIT_SUCCESS);
}