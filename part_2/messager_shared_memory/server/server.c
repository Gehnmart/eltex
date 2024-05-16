#include "../additional.h"

void *Server(void *argv) {
  MessagerCtl *ctl = (MessagerCtl *)argv;
  int prev_usrlen = 0;
  

  Message message = {0};
  strncpy(message.username, ctl->selfname, USERNAME_MAX);
  char msg_buf[MESSAGE_LEN_MAX] = {0};
  while (!ctl->th_stop) {
    usleep(100000);
    for (int i = 0; i < USER_MAX; ++i) {
      User *user = &ctl->shared_data->usr_list.list[i];
      if (user->status == STAT_JOIN) {
        sprintf(msg_buf, "user %s connected\n", user->username);
        strncpy(message.text, msg_buf, MESSAGE_LEN_MAX);
        sem_wait(ctl->sem_msglist);
        memcpy((void *)&ctl->shared_data->msg_list.list[ctl->shared_data->msg_list.len],
              (void *)&message, sizeof(message));
        user->status = STAT_EXEC;
        ctl->shared_data->msg_list.len++;
        sem_post(ctl->sem_msglist);
        printf("INFO USER '%s' ADDED\n", user->username);
      } else if (user->status == STAT_EXIT) {
        sprintf(msg_buf, "user %s disconnected\n", user->username);
        strncpy(message.text, msg_buf, MESSAGE_LEN_MAX);
        sem_wait(ctl->sem_msglist);
        memcpy((void *)&ctl->shared_data->msg_list.list[ctl->shared_data->msg_list.len],
              (void *)&message, sizeof(message));
        user->status = STAT_FREE;
        ctl->shared_data->msg_list.len++;
        sem_post(ctl->sem_msglist);
        printf("INFO USER '%s' DELETED\n", user->username);
      }
    }
  }

  pthread_exit(NULL);
}

int main() {
  pthread_t th_server;
  MessagerCtl ctl = {0};
  strncpy(ctl.selfname, "/server", 8);

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
  
  ctl.sem_usrlist = sem_open(SEM_SERVER_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
  if(ctl.sem_usrlist == SEM_FAILED)
    errExit("sem_open");
  
  ctl.sem_msglist = sem_open(SEM_CLIENT_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
  if(ctl.sem_msglist == SEM_FAILED)
    errExit("sem_open");
  sem_post(ctl.sem_msglist);
  
  /*running all threads*/

  pthread_create(&th_server, NULL, Server, (void *)&ctl);

  getchar(); /*maybe fix... but its more comfortable*/
  ctl.th_stop = 1;
  pthread_join(th_server, NULL);

  /*free resources*/
  munmap(ctl.shared_data, sizeof(*ctl.shared_data));
  shm_unlink(SHARED_DATA_NAME);

  sem_close(ctl.sem_usrlist);
  sem_close(ctl.sem_msglist);
  sem_unlink(SEM_SERVER_NAME);
  sem_unlink(SEM_CLIENT_NAME);

  /*exited*/
  printf("exit success\n");
  exit(EXIT_SUCCESS);
}
