#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "header.h"

/*Serwer może wysłać do klientów komunikaty:
inicjujący pracę klienta w trybie chatu (kolejka klientów)
wysyłający odpowiedzi do klientów (kolejki klientów)
informujący klientów o zakończeniu pracy serwera - po wysłaniu takiego sygnału i odebraniu wiadomości STOP od wszystkich klientów 
serwer usuwa swoją kolejkę i kończy pracę. (kolejki klientów) */

//Dla uproszczenia można przyjąć, że serwer przechowuje informacje o klientach w statycznej tablicy 
//(rozmiar tablicy ogranicza liczbę klientów, którzy mogą się zgłosić do serwera).


int server_queue;
int first_available_id = 0;
int clients_count = 0;
client* clients[MAX_CLIENTS] = {NULL};

client* get_client(int client_id) {
    for (int i = 0; i < clients_count; i++) 
        if (clients[i]->id == client_id) 
            return clients[i];

    return NULL;
}

//o otrzymaniu takiego komunikatu, serwer otwiera kolejkę klienta, przydziela klientowi identyfikator (np. numer w kolejności zgłoszeń) 
//i odsyła ten identyfikator do klienta (komunikacja w kierunku serwer->klient odbywa się za pomocą kolejki klienta).

void init_handler(msg* message) {
    int queue_id = atoi(message->text);

    client* new_client = calloc(1, sizeof(client));
    new_client -> id = first_available_id++;
    new_client -> queue_id = queue_id;
    new_client -> connected_client_id = -1;

    clients[clients_count++] = new_client;

    msg reply;
    reply.type = INIT;
    sprintf(reply.text, "%d", new_client->id);
    msgsnd(queue_id, &reply, MAX_MSG_SIZE, 0);
}

void list_handler(msg* message) {
    printf("Got to list function \n");
    int client_id = atoi(message->text);
    client* client = get_client(client_id);

    msg reply;
    reply.type = LIST;
    sprintf(reply.text, "");

    for (int i = 0; i < clients_count; i++) 
        if(clients[i]->connected_client_id == -1) 
            sprintf(reply.text + strlen(reply.text), "User %d is free\n", clients[i]->id);
        else 
            sprintf(reply.text + strlen(reply.text), "User %d is already chatting\n", clients[i]->id);
        
    
    msgsnd(client->queue_id, &reply, MAX_MSG_SIZE, 0);
    puts(reply.text);
}


//Serwer ma umożliwiać łączenie klientów w pary - klienci przechodząc do trybu chatu będą mogli wysyłać sobie bezpośrednio wiadomości bez udziału serwera.

void connect_handler(msg* message) {
    int client_id = atoi(strtok(message->text, " "));
    int second_id = atoi(strtok(NULL, " "));

    client* first = get_client(client_id);
    client* second = get_client(second_id);

    first->connected_client_id = second->id;
    second->connected_client_id = first->id;

    msg reply;
    reply.type = CONNECT;
    sprintf(reply.text, "%d", first->queue_id);
    msgsnd(second->queue_id, &reply, MAX_MSG_SIZE, 0);
    sprintf(reply.text, "%d", second->queue_id);
    msgsnd(first->queue_id, &reply, MAX_MSG_SIZE, 0);
}

void disconnect_handler(msg* message) {
    int client_id = atoi(message->text);

    client* first = get_client(client_id);
    client* second = get_client(first->connected_client_id);

    first->connected_client_id = -1;
    second->connected_client_id = -1;

    msg reply;
    reply.type = DISCONNECT;
    msgsnd(second->queue_id, &reply, MAX_MSG_SIZE, 0);
}

void stop_handler(msg* message) {
    int client_id = atoi(message->text);

    int client_offset;
    for (int i = 0; i < clients_count; i++) {
        if (clients[i]->id == client_id) {
            client_offset = i;
            break;
        }
    }

    client* client_to_be_deleted = clients[client_offset];

    for (int i = client_offset; i < clients_count - 1; i++) {
        clients[i] = clients[i + 1];
    }
    clients[clients_count - 1] = NULL;
    clients_count--;

    free(client_to_be_deleted);
}

void stop_server() {
    msg stop_server;
    stop_server.type = STOP_SERVER;
    for (int i = 0; i < clients_count; i++) 
        msgsnd(clients[i]->queue_id, &stop_server, MAX_MSG_SIZE, 0);

    while (clients_count > 0) {
        msg stop_client;
        msgrcv(server_queue, &stop_client, MAX_MSG_SIZE, STOP, 0);
        stop_handler(&stop_client);
    }

    msgctl(server_queue, IPC_RMID, NULL);
    exit(0);
}

/*void print_msg(msg* message) {
    char buffer[MAX_MSG_SIZE * 2];
    switch (message->type) {
            case INIT:
                sprintf(buffer, "New user!");
                break;
            case LIST:
                sprintf(buffer, "Listing:");
                break;
            case CONNECT:
                sprintf(buffer, "Connection between users: %s", message->text);
                break;
            case DISCONNECT:
                sprintf(buffer, "User %s disconnected.", message->text);
                break;
            case STOP:
                sprintf(buffer, "User %s quited.", message->text);
                break;
        }
    puts(buffer);
}*/

int main() {
//Klucze dla kolejek mają być generowane na podstawie ścieżki $HOME.
    char* path = getpwuid(getuid())->pw_dir;
    key_t queue_key = ftok(path, SERVER_ID); 
    server_queue = msgget(queue_key, IPC_CREAT | 0666); //tworzy kolejke serwera
    signal(SIGINT, stop_server);
    printf("Server ready, waiting for new clients... \n");
    while (1) {
        msg incoming_message;
        msgrcv(server_queue, &incoming_message, MAX_MSG_SIZE, ANY_MESSAGE, 0);
        //print_msg(&incoming_message);
        puts("Got message from client: ");
        char to_print[MAX_MSG_SIZE * 2];
        if (incoming_message.type == STOP) 
        {
            sprintf(to_print, "STOP - client %s has disconnected", incoming_message.text);
            puts(to_print);
            stop_handler(&incoming_message);
            continue;
        }
        else if (incoming_message.type == DISCONNECT) 
        {
            disconnect_handler(&incoming_message);
            continue;
        }
        else if (incoming_message.type == LIST) 
        {
            list_handler(&incoming_message);
            continue;
        }
        else if (incoming_message.type == CONNECT) 
        {
            connect_handler(&incoming_message);
            continue;
        }
        if (incoming_message.type == INIT)
        {
            init_handler(&incoming_message);
            continue;
        }  
    }
}