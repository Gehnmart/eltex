#ifndef CLIENT_THREADS_H
#define CLIENT_THREADS_H

#include <ncurses.h>

#include "../../additional.h"
#include "../client_controller.h"
#include "../helpers/helpers.h"

void *UserReceiver(void *argv);
void *MessageReceiver(void *argv);
void *MessageSender(void *argv);

#endif  // CLIENT_THREADS_H