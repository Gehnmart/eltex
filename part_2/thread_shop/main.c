#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef COLOR
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define STOP "\e[0m"
#else
#define RED
#define GRN
#define YEL
#define STOP
#endif

#define SHOP_COUNT 5
#define SHOP_PRODUCT_MAX 10000
#define SHOP_PRODUCT_MIN 8000

#define CLIENT_COUNT 3
#define HUNGER_MAX 100000
#define HUNGER_MIN 80000

#define LOAD_COUNT 5000

#define SET_FREE 0x0
#define SET_BUSY 0x1

typedef struct {
  pthread_mutex_t mutex;
  int is_busy;
  int product;
} Shop;

typedef struct {
  pthread_t thread;
  int hunger;
  int id;
} Client;

Client clients[CLIENT_COUNT];
Shop shops[SHOP_COUNT];
pthread_t loader;
int g_run = 1;

int GetRandom(int min, int max) { return rand() % (max - min) + min + 1; }

void *ClientBuyer(void *args) {
  Client *client = (Client *)args;
  int temp = 0;
  while (client->hunger > 0) {
    char shop_is_found = 0;
    for (int i = 0; i < SHOP_COUNT; i++) {
      if (shops[i].is_busy == SET_FREE && shops[i].product > 0) {
        pthread_mutex_lock(&shops[i].mutex);
        shops[i].is_busy = SET_BUSY;
        temp = shops[i].product;
        shops[i].product = 0;
        client->hunger -= temp;

        printf(YEL "INFO  : CLIENT[%d] BUY - %d IN SHOP[%d]\n" STOP, client->id, temp, i);
        printf(YEL "INFO  : CLIENT[%d] CURRENT HUNGER %d\n" STOP,client->id, client->hunger);
        shop_is_found = 1;
        printf(YEL "INFO  : CLIENT[%d] START SLEEP\n" STOP, client->id);
        sleep(2);
        printf(YEL "INFO  : CLIENT[%d] STOP SLEEP\n" STOP, client->id);

        shops[i].is_busy = SET_FREE;
        pthread_mutex_unlock(&shops[i].mutex);
      }
    }
    if(shop_is_found){
      printf(GRN "STATUS: CLIENT[%d] FOUND SUCCESS\n" STOP, client->id);
    } else {
      printf(RED "STATUS: CLIENT[%d] FOUND FAIL\n" STOP, client->id);
      printf(YEL "INFO  : CLIENT[%d] START SLEEP\n" STOP, client->id);
      sleep(2);
      printf(YEL "INFO  : CLIENT[%d] STOP SLEEP\n" STOP, client->id);
    }
  }
  printf(YEL "INFO  : CLIENT[%d] DIED\n" STOP, client->id);

  return NULL;
}

void *Loader(void *args) {
  while (g_run) {
    char shop_is_found = 0;
    for (int i = 0; i < SHOP_COUNT; i++) {
      if (shops[i].is_busy == SET_FREE) {
        pthread_mutex_lock(&shops[i].mutex);
        shops[i].is_busy = SET_BUSY;
        shops[i].product += LOAD_COUNT;
        printf(YEL "INFO  : LOADER[0] LOAD IN SHOP[%d] %d PRODUCT\n" STOP, i, LOAD_COUNT);
        printf(YEL "INFO  : LOADER[0] SHOP[%d] CONTAIN %d PRODUCT\n" STOP, i, shops[i].product);
        shop_is_found = 1;
        printf(YEL "INFO  : LOADER[0] START SLEEP\n" STOP);
        sleep(1);
        printf(YEL "INFO  : LOADER[0] END   SLEEP\n" STOP);
        shops[i].is_busy = SET_FREE;
        pthread_mutex_unlock(&shops[i].mutex);
      }
    }
    if(shop_is_found){
      printf(GRN "STATUS: LOADER[0] FOUND SUCCESS\n" STOP);
    } else {
      printf(RED "STATUS: LOADER[0] FOUND FAIL\n" STOP);
      printf(YEL "INFO  : LOADER[0] START SLEEP\n" STOP);
      sleep(1);
      printf(YEL "INFO  : LOADER[0] END   SLEEP\n" STOP);
    }
  }
  printf(YEL "INFO  : LOADER[0] DIED\n" STOP);

  return NULL;
}

int main() {
  srand(time(NULL));
  for (int i = 0; i < SHOP_COUNT; i++) {
    pthread_mutex_init(&shops[i].mutex, NULL);
    shops[i].product = GetRandom(SHOP_PRODUCT_MIN, SHOP_PRODUCT_MAX);
  }

  for (int i = 0; i < CLIENT_COUNT; i++) {
    clients[i].hunger = GetRandom(HUNGER_MIN, HUNGER_MAX);
    clients[i].id = i;
    pthread_create(&clients[i].thread, NULL, ClientBuyer, (void *)&clients[i]);
  }

  pthread_create(&loader, NULL, Loader, NULL);

  int *status;
  for (int i = 0; i < CLIENT_COUNT; i++) {
    pthread_join(clients[i].thread, NULL);
  }
  g_run = 0;

  pthread_join(loader, NULL);

  for (int i = 0; i < SHOP_COUNT; i++) {
    pthread_mutex_destroy(&shops[i].mutex);
  }
}