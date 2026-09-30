#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_PRODUCTORES 3
#define NUM_CONSUMIDORES 3
#define PRODUCCIONES 4
#define TAM_BUFFER 10

int buffer[TAM_BUFFER];
int entrada = 0;
int salida = 0;

sem_t espacios_libres;
sem_t elementos_disponibles;
pthread_mutex_t mutex_buffer;

void *productor(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < PRODUCCIONES; i++)
    {
        sem_wait(&espacios_libres);

        pthread_mutex_lock(&mutex_buffer);

        buffer[entrada] = i;

        printf("[P%d] Producing %d ...\n", id, i);
        fflush(stdout);

        entrada = (entrada + 1) % TAM_BUFFER;

        pthread_mutex_unlock(&mutex_buffer);

        sem_post(&elementos_disponibles);

        usleep((rand() % 500 + 100) * 1000);
    }

    return NULL;
}

void *consumidor(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < PRODUCCIONES; i++)
    {
        int producto;

        sem_wait(&elementos_disponibles);

        pthread_mutex_lock(&mutex_buffer);

        producto = buffer[salida];

        printf("------> [C%d] consumed %d\n", id, producto);
        fflush(stdout);

        salida = (salida + 1) % TAM_BUFFER;

        pthread_mutex_unlock(&mutex_buffer);

        sem_post(&espacios_libres);

        usleep((rand() % 700 + 100) * 1000);
    }

    return NULL;
}

int main()
{
    pthread_t productores[NUM_PRODUCTORES];
    pthread_t consumidores[NUM_CONSUMIDORES];

    int id_productores[NUM_PRODUCTORES];
    int id_consumidores[NUM_CONSUMIDORES];

    srand(time(NULL));

    sem_init(&espacios_libres, 0, TAM_BUFFER);
    sem_init(&elementos_disponibles, 0, 0);

    pthread_mutex_init(&mutex_buffer, NULL);

    for (int i = 0; i < NUM_PRODUCTORES; i++)
    {
        id_productores[i] = i;
        pthread_create(&productores[i], NULL, productor, &id_productores[i]);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++)
    {
        id_consumidores[i] = i;
        pthread_create(&consumidores[i], NULL, consumidor, &id_consumidores[i]);
    }

    for (int i = 0; i < NUM_PRODUCTORES; i++)
    {
        pthread_join(productores[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++)
    {
        pthread_join(consumidores[i], NULL);
    }

    sem_destroy(&espacios_libres);
    sem_destroy(&elementos_disponibles);
    pthread_mutex_destroy(&mutex_buffer);

    return 0;
}
