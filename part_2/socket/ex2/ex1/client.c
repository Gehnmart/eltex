#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  int port = 0;
  struct sockaddr_in serv, new_serv, client;
  int cfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (cfd == -1) handle_error("socket");

  int coking_addr = 0;
  inet_pton(AF_INET, ADDR, &coking_addr);
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(PORT);
  serv.sin_addr.s_addr = coking_addr;

  char buf[50] = {0};

  sendto(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&serv, sizeof(serv));
  memset(&new_serv, 0, sizeof(new_serv));
  int serv_size = sizeof(new_serv);
  recvfrom(cfd, &port, sizeof(port), 0, (struct sockaddr *)&new_serv,
           &serv_size);

  while (1) {
    scanf("%50s", buf);
    if (strncmp(buf, "exit", sizeof(buf)) == 0) {
      sendto(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&new_serv, serv_size);
      break;
    }
    if (strncmp(buf, "time", sizeof(buf)) == 0) {
      sendto(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&new_serv, serv_size);
      recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&new_serv,
               &serv_size);
      printf("now time = %s\n", buf);
    }
  }

  close(cfd);
  exit(EXIT_SUCCESS);
}