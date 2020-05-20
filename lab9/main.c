#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h> 
#include <math.h>
#include <sys/times.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t is_asleep_cond = PTHREAD_COND_INITIALIZER;
pthread_t client_to_shave;
pthread_t *clients_on_chairs;

int clients_no;
int chairs_no;
int first_empty_chair = 0;
int empty_chairs;
int next_client_to_shave = 0;
bool is_asleep = 0;

void *client() {
    pthread_t client_id = pthread_self();
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (is_asleep) {
            printf("Klient: budze golibrode; %ld\n", client_id);
            client_to_shave = client_id;
            pthread_cond_broadcast(&is_asleep_cond);
            break;
        }
        else if (empty_chairs > 0) {
            clients_on_chairs[first_empty_chair] = client_id;
            first_empty_chair = (first_empty_chair + 1) % chairs_no;
            empty_chairs--;
            printf("Klient: poczekalnia, wolne miejsca %d; %ld\n", empty_chairs, client_id);
            break;
        }
        else {
            printf("Klient: zajete; %ld\n", client_id);
            pthread_mutex_unlock(&mutex);
            sleep((rand() % 5 + 1) * 3);
        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit((void *)0);
}

void *golibroda() {
    int shaved_clients = 0;
    while (shaved_clients<clients_no)
    {
        pthread_mutex_lock(&mutex);
        if (empty_chairs == chairs_no) {
            printf("Golibroda: ide spac\n");
            is_asleep = 1;
            pthread_cond_wait(&is_asleep_cond, &mutex);
            is_asleep = 0;
        }
        else {
            empty_chairs++;
            client_to_shave = clients_on_chairs[next_client_to_shave];
            next_client_to_shave = (next_client_to_shave + 1) % chairs_no;
        }
        printf("Golibroda: czeka %d klientow, gole klienta %ld\n", chairs_no - empty_chairs, client_to_shave);
        pthread_mutex_unlock(&mutex);
        sleep((rand() % 5 + 1));
        pthread_mutex_lock(&mutex);
        shaved_clients++;
        if (shaved_clients == clients_no) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void *)0);
}

void shaving() {
    pthread_t *clients = calloc(clients_no + 1, sizeof(pthread_t));
    pthread_create(&clients[0], NULL, golibroda, NULL);
    for (int i = 1; i < clients_no + 1; i++)
    {
        sleep(rand() % 5 + 1);
        pthread_create(&clients[i], NULL, client, NULL);  }
    for (int i = 0; i < clients_no + 1; i++)
        pthread_join(clients[i], NULL);
}

int main(int args, char **argv)
{
    if (args < 3)
    {
        printf("Wrong number of argumets, should be [chairs number] [clients number] \n");
        exit(-1);
    }
    empty_chairs = chairs_no = atoi(argv[1]);
    clients_no = atoi(argv[2]);
    srand(time(NULL));
    clients_on_chairs = calloc(chairs_no, sizeof(pthread_t));
    shaving();
    return 0;
}