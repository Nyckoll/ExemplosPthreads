#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define THREAD_NUM 3
#define BUFFER_SIZE 10
#define RUN_TIME 5

typedef struct {
    int clock[THREAD_NUM];
} VectorClock;

VectorClock clockQueue[BUFFER_SIZE];
int taskCount = 0;
int running = 1;

pthread_mutex_t mutex;
pthread_cond_t condFull;
pthread_cond_t condEmpty;

int produced_count = 0;
int consumed_count = 0;

void produceVectorClock(VectorClock *clock, int id) {
    for (int i = 0; i < THREAD_NUM; i++) {
        clock->clock[i] = rand() % 100;
    }
    printf("(Produtor %d): ", id);
    for (int i = 0; i < THREAD_NUM; i++) {
        printf("%d ", clock->clock[i]);
    }
    printf("\n");
}

void consumeVectorClock(VectorClock *clock, int id) {
    printf("(Consumidor %d): ", id);
    for (int i = 0; i < THREAD_NUM; i++) {
        printf("%d ", clock->clock[i]);
    }
    printf("\n");
}

VectorClock getVectorClock() {
    pthread_mutex_lock(&mutex);
    while (taskCount == 0 && running) {
        pthread_cond_wait(&condEmpty, &mutex);
    }
    VectorClock clock;
    if (running) {
        clock = clockQueue[0];
        for (int i = 0; i < taskCount - 1; i++) {
            clockQueue[i] = clockQueue[i + 1];
        }
        taskCount--;
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condFull);
    return clock;
}

void submitVectorClock(VectorClock clock) {
    pthread_mutex_lock(&mutex);
    while (taskCount == BUFFER_SIZE && running) {
        pthread_cond_wait(&condFull, &mutex);
    }
    if (running) {
        clockQueue[taskCount] = clock;
        taskCount++;
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condEmpty);
}



// Cenário 1: Fila ficará cheia (Taxa alta do produtor, taxa baixa do consumidor)
void *producer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock;
        produceVectorClock(&clock, id);
        submitVectorClock(clock);
        produced_count++;
        sleep(1);  // Taxa alta: Produz 1 relógio por segundo
    }
    return NULL;
}

void *consumer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock = getVectorClock();
        if (running) {
            consumeVectorClock(&clock, id);
            consumed_count++;
        }
        sleep(2);  // Taxa baixa: Consome 1 relógio a cada 2 segundos
    }
    return NULL;
}

// Alternativa para o Cenário 2: Fila ficará vazia (Taxa baixa do produtor, taxa alta do consumidor)
// Descomente as funções abaixo para o Cenário 2
/*
void *producer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock;
        produceVectorClock(&clock, id);
        submitVectorClock(clock);
        produced_count++;
        sleep(3);  // Taxa baixa: Produz 1 relógio a cada 2 segundos
    }
    return NULL;
}

void *consumer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock = getVectorClock();
        if (running) {
            consumeVectorClock(&clock, id);
            consumed_count++;
        }
        sleep(0.5);  // Taxa alta: Consome 1 relógio por segundo
    }
    return NULL;
}

*/
int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condEmpty, NULL);
    pthread_cond_init(&condFull, NULL);

    pthread_t producers[THREAD_NUM];
    pthread_t consumers[THREAD_NUM];

    for (long i = 0; i < THREAD_NUM; i++) {
        pthread_create(&producers[i], NULL, &producer, (void *)i);
        pthread_create(&consumers[i], NULL, &consumer, (void *)i);
    }

    sleep(RUN_TIME);

    running = 0;

    pthread_cond_broadcast(&condEmpty);
    pthread_cond_broadcast(&condFull);

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condEmpty);
    pthread_cond_destroy(&condFull);

    printf("\nFim da execução. Tempo de execução: %ds.\n", RUN_TIME);
    printf("Total produtores: %d\n", produced_count);
    printf("Total consumidores: %d\n", consumed_count);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define THREAD_NUM 3
#define BUFFER_SIZE 10
#define RUN_TIME 5

typedef struct {
    int clock[THREAD_NUM];
} VectorClock;

VectorClock clockQueue[BUFFER_SIZE];
int taskCount = 0;
int running = 1;

pthread_mutex_t mutex;
pthread_cond_t condFull;
pthread_cond_t condEmpty;

int produced_count = 0;
int consumed_count = 0;

void produceVectorClock(VectorClock *clock, int id) {
    for (int i = 0; i < THREAD_NUM; i++) {
        clock->clock[i] = rand() % 100;
    }
    printf("(Produtor %d): ", id);
    for (int i = 0; i < THREAD_NUM; i++) {
        printf("%d ", clock->clock[i]);
    }
    printf("\n");
}

void consumeVectorClock(VectorClock *clock, int id) {
    printf("(Consumidor %d): ", id);
    for (int i = 0; i < THREAD_NUM; i++) {
        printf("%d ", clock->clock[i]);
    }
    printf("\n");
}

VectorClock getVectorClock() {
    pthread_mutex_lock(&mutex);
    while (taskCount == 0 && running) {
        pthread_cond_wait(&condEmpty, &mutex);
    }
    VectorClock clock;
    if (running) {
        clock = clockQueue[0];
        for (int i = 0; i < taskCount - 1; i++) {
            clockQueue[i] = clockQueue[i + 1];
        }
        taskCount--;
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condFull);
    return clock;
}

void submitVectorClock(VectorClock clock) {
    pthread_mutex_lock(&mutex);
    while (taskCount == BUFFER_SIZE && running) {
        pthread_cond_wait(&condFull, &mutex);
    }
    if (running) {
        clockQueue[taskCount] = clock;
        taskCount++;
    }
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condEmpty);
}



// Cenário 1: Fila ficará cheia (Taxa alta do produtor, taxa baixa do consumidor)
void *producer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock;
        produceVectorClock(&clock, id);
        submitVectorClock(clock);
        produced_count++;
        sleep(1);  // Taxa alta: Produz 1 relógio por segundo
    }
    return NULL;
}

void *consumer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock = getVectorClock();
        if (running) {
            consumeVectorClock(&clock, id);
            consumed_count++;
        }
        sleep(2);  // Taxa baixa: Consome 1 relógio a cada 2 segundos
    }
    return NULL;
}

// Alternativa para o Cenário 2: Fila ficará vazia (Taxa baixa do produtor, taxa alta do consumidor)
// Descomente as funções abaixo para o Cenário 2
/*
void *producer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock;
        produceVectorClock(&clock, id);
        submitVectorClock(clock);
        produced_count++;
        sleep(3);  // Taxa baixa: Produz 1 relógio a cada 2 segundos
    }
    return NULL;
}

void *consumer(void *args) {
    long id = (long)args;
    while (running) {
        VectorClock clock = getVectorClock();
        if (running) {
            consumeVectorClock(&clock, id);
            consumed_count++;
        }
        sleep(0.5);  // Taxa alta: Consome 1 relógio por segundo
    }
    return NULL;
}

*/
int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condEmpty, NULL);
    pthread_cond_init(&condFull, NULL);

    pthread_t producers[THREAD_NUM];
    pthread_t consumers[THREAD_NUM];

    for (long i = 0; i < THREAD_NUM; i++) {
        pthread_create(&producers[i], NULL, &producer, (void *)i);
        pthread_create(&consumers[i], NULL, &consumer, (void *)i);
    }

    sleep(RUN_TIME);

    running = 0;

    pthread_cond_broadcast(&condEmpty);
    pthread_cond_broadcast(&condFull);

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condEmpty);
    pthread_cond_destroy(&condFull);

    printf("\nFim da execução. Tempo de execução: %ds.\n", RUN_TIME);
    printf("Total produtores: %d\n", produced_count);
    printf("Total consumidores: %d\n", consumed_count);

    return 0;
}
