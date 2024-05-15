#ifndef CLIENT_THREADS_H
#define CLIENT_THREADS_H

#include <ncurses.h>
#include "../../additional.h"

void *MessageSender(void *argv);
void *MessageReceiver(void *argv);
void *UserReceiver(void *argv);

#endif  // CLIENT_THREADS_H