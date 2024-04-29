#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SHOP_COUNT 5
#define SHOP_PRODUCT_MAX 10000

#define CLIENT_COUNT 3
#define HUNGER_MAX 100000

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

int GetRandom(int limit) { return rand() % limit; }

void *ClientBuyer(void *args) {
  Client *client = (Client *)args;
  int temp = 0;
  while (client->hunger > 0) {
    for (int i = 0; i < SHOP_COUNT && client->hunger > 0; i++) {
      if (shops[i].is_busy == SET_FREE && shops[i].product > 0) {
        pthread_mutex_lock(&shops[i].mutex);
        shops[i].is_busy = SET_BUSY;

        temp = shops[i].product;
        shops[i].product = 0;
        printf("client nomber %d\n  buy %d\n  in shop %d\n  my hunger is %d\n",
               client->id, temp, i, client->hunger - temp);

        client->hunger -= temp;
        shops[i].is_busy = SET_FREE;
        pthread_mutex_unlock(&shops[i].mutex);
        sleep(2);
      }
    }
  }
  return NULL;
}

void *Loader(void *args) {
  while (g_run) {
    for (int i = 0; i < SHOP_COUNT; i++) {
      if (shops[i].is_busy == SET_FREE) {
        pthread_mutex_lock(&shops[i].mutex);
        shops[i].is_busy = SET_BUSY;
        printf(
            "loader current shop %d\n  product count %d\n  load %d\n  current "
            "product count %d\n",
            i, shops[i].product, LOAD_COUNT, shops[i].product + LOAD_COUNT);
        shops[i].product += LOAD_COUNT;
        shops[i].is_busy = SET_FREE;
        pthread_mutex_unlock(&shops[i].mutex);
        sleep(1);
      }
    }
  }
  return NULL;
}

int main() {
  srand(time(NULL));
  for (int i = 0; i < SHOP_COUNT; i++) {
    pthread_mutex_init(&shops[i].mutex, NULL);
    shops[i].product = GetRandom(SHOP_PRODUCT_MAX);
  }

  for (int i = 0; i < CLIENT_COUNT; i++) {
    clients[i].hunger = GetRandom(HUNGER_MAX);
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