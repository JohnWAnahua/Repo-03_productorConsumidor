#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXPRODUCING 10
#define MAXAPPENDING 10
#define MAXTAKING 5
#define MAXCONSUMING 5

#define _wait(a) sleep(rand() % a)

#define out(s)             \
    printf(s);             \
    fflush(stdout)

#define outi(s, n)         \
    printf(s, n);          \
    fflush(stdout)

/* Variables compartidas */
int n = 0;

sem_t s;
sem_t delay;

/* Prototipos */
void produce(void);
void append(void);
void take(void);
void consume(void);

/* Hilo productor */
void *producer(void *data)
{
    while (1)
    {
        produce();

        sem_wait(&s);

        append();

        n++;

        outi("[P] \t Item producido: %d\n", n);

        /* Si el buffer estaba vacío, despierta al consumidor */
        if (n == 1)
        {
            sem_post(&delay);
        }

        sem_post(&s);
    }

    return NULL;
}

/* Hilo consumidor */
void *consumer(void *data)
{
    int m;

    /* Espera hasta que exista al menos un elemento */
    sem_wait(&delay);

    while (1)
    {
        sem_wait(&s);

        take();

        outi("[C] \t Item tomado: %d\n", n);

        n--;

        m = n;

        sem_post(&s);

        consume();

        /* Si el buffer quedó vacío, espera nuevamente */
        if (m == 0)
        {
            sem_wait(&delay);
        }
    }

    return NULL;
}

/* Proceso de producción */
void produce(void)
{
    out("[P] Produciendo...\n");
    _wait(MAXPRODUCING);
    out("[P] Producido\n");
}

/* Proceso de almacenamiento */
void append(void)
{
    out("[P] \t Almacenando...\n");
    _wait(MAXAPPENDING);
    out("[P] \t Almacenado\n");
}

/* Proceso de extracción */
void take(void)
{
    out("[C] Extrayendo...\n");
    _wait(MAXTAKING);
    out("[C] Extraido\n");
}

/* Proceso de consumo */
void consume(void)
{
    out("[C] \t Consumiendo...\n");
    _wait(MAXCONSUMING);
    out("[C] \t Consumido\n");
}

/* Función principal */
int main(void)
{
    pthread_t producer_thread;
    pthread_t consumer_thread;

    /* Inicializar generador de números aleatorios */
    srand((unsigned int)time(NULL));

    /* Inicializar semáforos */
    sem_init(&s, 0, 1);
    sem_init(&delay, 0, 0);

    /* Crear hilos */
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    /* Esperar a los hilos */
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    /* Destruir semáforos */
    sem_destroy(&s);
    sem_destroy(&delay);

    return 0;
}
