#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)


int main() {
  struct sockaddr_in serv = {0};
  ServerCtl          ctl = {0};
  int                sfd;

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sfd != -1) {
    handle_error("socket error:");
  }
  
  int addr;
  inet_pton(AF_INET, SERV_ADDR, &addr);
  serv.sin_port = htons(SERV_PORT);
  serv.sin_addr.s_addr = addr;
  serv.sin_family = AF_INET;

  if(bind(sfd, (struct sockaddr *) &serv, sizeof(serv)) == -1) {
    handle_error("bind error:");
  }


  exit(EXIT_SUCCESS);
}