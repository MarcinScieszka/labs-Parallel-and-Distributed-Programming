#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NR_OF_CLIENTS 4
#define NR_OF_MUGS 3
#define MAX_DRINKING_TIME 3 // in seconds
#define MAX_BEERS_DRANK 2

int mugs[NR_OF_MUGS];
int mugs_drank[NR_OF_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *drink(void *arg);

int main(int argc, char *argv[])
{
    int i;
    srand(time(NULL));

    for (i = 0; i < NR_OF_MUGS; i++)
    {
        mugs[i] = 0;
    }

    for (i = 0; i < NR_OF_CLIENTS; i++)
    {
        mugs_drank[i] = 0;
    }

    pthread_t *clients = malloc(sizeof(pthread_t) * NR_OF_CLIENTS);
    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < NR_OF_CLIENTS; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i + 1;

        if (pthread_create(&clients[i], NULL, &drink, id) != 0)
        {
            perror("Thread creation failed");
        }
    }

    for (i = 0; i < NR_OF_CLIENTS; i++)
    {
        if (pthread_join(clients[i], NULL) != 0)
        {
            perror("Failed to join thread");
        }
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}

void *drink(void *arg)
{
    int i = 0;
    int client_id = *(int *)arg;
    int mug_id, drinking_time;

    while (1)
    {
        drinking_time = rand() % 6 + 1;

        pthread_mutex_lock(&mutex);
        i = 0;
        while (mugs[i] != 0)
        {
            i++;
            if (i == NR_OF_MUGS)
            {
                i = 0;
            }
        }
        mugs[i] = 1;
        mug_id = i + 1;
        pthread_mutex_unlock(&mutex);

        printf("Klient nr %d zamawia piwo w kuflu nr %d\n", client_id, i + 1);
        sleep(drinking_time);
        printf("Klient nr %d oddaje kufel nr %d po czasie %d\n", client_id, mug_id, drinking_time);
        mugs_drank[client_id - 1]++;
        mugs[mug_id - 1] = 0;
        if (mugs_drank[client_id - 1] == MAX_BEERS_DRANK)
        {
            printf("Klient nr %d opuszcza pub\n", client_id);
            break;
        }
    }
    free(arg);
}
