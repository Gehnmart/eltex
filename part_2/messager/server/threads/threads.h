#ifndef SERVER_THREADS_H
#define SERVER_THREADS_H

#include "../../additional.h"
#include "../server_controller.h"

void *RegisterHandler(void *argv);
void *MessageReceiver(void *argv);
void *MessageSender(void *argv);

int AddUser(ServerController *controller, char *username, mqd_t user_mq);
int DelUser(ServerController *controller, char *username);
int AddMessage(ServerController *controller, Message *message);

#endif  // SERVER_THREADS_H