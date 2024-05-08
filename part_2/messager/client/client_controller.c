#include "client_controller.h"

int ControllerDestroy(ClientController *controller) {
  pthread_mutex_destroy(&controller->user_list->mutex);
  pthread_mutex_destroy(&controller->message_list->mutex);
  pthread_mutex_destroy(&controller->ncurses_mutex);
  free(controller->user);
  free(controller->user_list);
  free(controller->message_list);
}

int ControllerInit(ClientController *controller) {
  controller->client_stop = 0;
  controller->user = calloc(sizeof(User), 1);
  controller->user_list = calloc(sizeof(UserList), 1);
  controller->message_list = calloc(sizeof(MessageList), 1);
  pthread_mutex_init(&controller->user_list->mutex, NULL);
  pthread_mutex_init(&controller->message_list->mutex, NULL);
  pthread_mutex_init(&controller->ncurses_mutex, NULL);
}