#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Límites de tiempo
#define MAXPRODUCING 10
#define MAXAPPENDING 10
#define MAXTAKING 5
#define MAXCONSUMING 5

// Funciones auxiliares
#define _wait(a) sleep(rand() % a)

#define out(s) \
    printf(s); \
    fflush(stdout)

#define outi(s, n) \
    printf(s, n); \
    fflush(stdout)

// Variables compartidas
int n;
sem_t s, delay;

// Prototipos de funciones
void produce();
void append();
void take();
void consume();

// Hilo productor
void *producer(void *data)
{
    while (1)
    {
        produce();

        sem_wait(&s);

        append();

        n = n + 1;

        outi("[P] \t \t item: %d\n", n);

        // Despierta al consumidor cuando aparece el primer elemento
        if (n == 1)
            sem_post(&delay);

        sem_post(&s);
    }

    pthread_exit(0);
}

// Hilo consumidor
void *consumer(void *data)
{
    while (1)
    {
        sem_wait(&s);

        take();

        outi("[C] \t \t item: %d\n", n);

        n = n - 1;

        sem_post(&s);

        consume();

        // Si ya no quedan elementos, el consumidor espera
        if (n == 0)
            sem_wait(&delay);
    }

    pthread_exit(0);
}

// Función principal
int main(int argc, char **argv)
{
    pthread_t *consumer_pt;
    pthread_t *producer_pt;

    // Inicializar números aleatorios
    srand(time(NULL));

    // Inicializar semáforos
    sem_init(&s, 0, 1);
    sem_init(&delay, 0, 0);

    // Reservar memoria para los hilos
    consumer_pt = (pthread_t *)malloc(sizeof(pthread_t));
    producer_pt = (pthread_t *)malloc(sizeof(pthread_t));

    // Crear hilo consumidor
    pthread_create(
        consumer_pt,
        NULL,
        consumer,
        NULL
    );

    // Crear hilo productor
    pthread_create(
        producer_pt,
        NULL,
        producer,
        NULL
    );

    // Mantener el programa ejecutándose
    pthread_exit(0);
}

// Función para producir
void produce()
{
    out("[P] Producing\n");

    _wait(MAXPRODUCING);

    out("[P] Produced\n");
}

// Función para agregar el producto
void append()
{
    out("[P] \t Appending\n");

    _wait(MAXAPPENDING);

    out("[P] \t Appended\n");
}

// Función para tomar el producto
void take()
{
    out("[C] Taking\n");

    _wait(MAXTAKING);

    out("[C] Taked\n");
}

// Función para consumir el producto
void consume()
{
    out("[C] \t Consuming\n");

    _wait(MAXCONSUMING);

    out("[C] \t Consumed\n");
}
