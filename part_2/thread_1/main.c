#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 5

void *ThreadIndexPrint(void *args) {
    int index = *((int *)args);
    printf("thread index = %d\n", index);

    return NULL;
}

int main() {
    pthread_t thread[THREAD_COUNT];
    int thread_data[THREAD_COUNT];
    int *status;
    int i;
    for(i = 0; i < THREAD_COUNT; i++){
        thread_data[i] = i;
        pthread_create(&thread[i], NULL, ThreadIndexPrint, (void *)&thread_data[i]);
    }
    for(i = 0; i < THREAD_COUNT; i++){
        pthread_join(thread[i], (void **)&status);
    }
}