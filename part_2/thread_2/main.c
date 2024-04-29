#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 6

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

volatile long int g_value;

void *ThreadIndexPrint(void *args) {
    (void)args;
    int i;
    for(i = 0; i < 1000000000; i++);

    pthread_mutex_lock(&m1);
    g_value += i;
    pthread_mutex_unlock(&m1);

    return NULL;
}

int main(int argc, char **argv) {
    (void)argc;

    if(argc > 1 && argv[1][0] == 'm'){
        pthread_t thread[THREAD_COUNT];
        int *status;
        int i;
        for(i = 0; i < THREAD_COUNT; i++){
            pthread_create(&thread[i], NULL, ThreadIndexPrint, NULL);
        }
        for(i = 0; i < THREAD_COUNT; i++){
            pthread_join(thread[i], (void **)&status);
        }
    } else {
        for(;g_value < 6000000000; ++g_value);
    }
    printf("g_value = %ld\n", g_value);
}