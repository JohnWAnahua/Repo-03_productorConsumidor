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

// Macros auxiliares
#define _wait(a) sleep(rand() % a)

#define out(s) \
    printf(s); \
    fflush(stdout)

#define outi(s, n) \
    printf(s, n); \
    fflush(stdout)

// Variables compartidas
int n;

sem_t s;
sem_t delay;

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

        // Despierta al consumidor cuando hay un nuevo elemento
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
        // Espera mientras no haya elementos disponibles
        while (n == 0)
        {
            sem_wait(&delay);
        }

        sem_wait(&s);

        take();

        outi("[C] \t \t item: %d\n", n);

        n = n - 1;

        sem_post(&s);

        consume();
    }

    pthread_exit(0);
}

// Función principal
int main(int argc, char **argv)
{
    pthread_t *consumer_pt;
    pthread_t *producer_pt;

    // Inicializa la semilla para números aleatorios
    srand(time(NULL));

    // Semáforo para proteger la sección crítica
    sem_init(&s, 0, 1);

    // Semáforo para controlar la espera del consumidor
    sem_init(&delay, 0, 0);

    // Reserva memoria para los identificadores de los hilos
    consumer_pt = (pthread_t *)malloc(sizeof(pthread_t));
    producer_pt = (pthread_t *)malloc(sizeof(pthread_t));

    // Crea el hilo consumidor
    pthread_create(
        consumer_pt,
        NULL,
        consumer,
        NULL
    );

    // Crea el hilo productor
    pthread_create(
        producer_pt,
        NULL,
        producer,
        NULL
    );

    pthread_exit(0);
}

// Produce un producto
void produce()
{
    out("[P] Producing\n");

    _wait(MAXPRODUCING);

    out("[P] Produced\n");
}

// Agrega el producto al buffer
void append()
{
    out("[P] \t Appending\n");

    _wait(MAXAPPENDING);

    out("[P] \t Appended\n");
}

// Toma un producto del buffer
void take()
{
    out("[C] Taking\n");

    _wait(MAXTAKING);

    out("[C] Taked\n");
}

// Consume el producto
void consume()
{
    out("[C] \t Consuming\n");

    _wait(MAXCONSUMING);

    out("[C] \t Consumed\n");
}
