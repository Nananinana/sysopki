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
client* clients_on_server[MAX_CLIENTS] = {NULL};
int clients_no = 0;
int next_client_id = 0;

void init_client(msg* incoming_message) {
    int client_queue_id = atoi(incoming_message->text);
    client* new_client = calloc(1, sizeof(client));
    new_client -> id = next_client_id;
    new_client -> queue_id = client_queue_id;
    new_client -> connected_to_client = -1;
    clients_on_server[clients_no] = new_client;
    clients_no++;
    next_client_id++;
    msg msg_to_client;
    msg_to_client.type = INIT;
    sprintf(msg_to_client.text, "%d", new_client->id);
    msgsnd(client_queue_id, &msg_to_client, MAX_MSG_SIZE, 0);  
}

client* get_client_by_id(int client_id) {
    for (int i = 0; i < clients_no; i++) 
    {
        if (clients_on_server[i]->id == client_id) 
            return clients_on_server[i];
    }
    return NULL;
}

void connect_clients(msg* incoming_message) {
    int client1_id = atoi(strtok(incoming_message->text, " "));
    client* client1 = get_client_by_id(client1_id);
    int client2_id = atoi(strtok(NULL, " "));    
    client* client2 = get_client_by_id(client2_id);
    client1->connected_to_client = client2->id;
    client2->connected_to_client = client1->id;
    msg msg_to_client;
    msg_to_client.type = CONNECT;
    sprintf(msg_to_client.text, "%d", client1->queue_id);
    msgsnd(client2->queue_id, &msg_to_client, MAX_MSG_SIZE, 0);
    sprintf(msg_to_client.text, "%d", client2->queue_id);
    msgsnd(client1->queue_id, &msg_to_client, MAX_MSG_SIZE, 0);
}

void disconnect_clients(msg* incoming_message) {
    int client1_id = atoi(incoming_message->text);
    client* client1 = get_client_by_id(client1_id);
    client* client2 = get_client_by_id(client1->connected_to_client);
    client1->connected_to_client = -1;
    client2->connected_to_client = -1;
    msg msg_to_client;
    msg_to_client.type = DISCONNECT;
    msgsnd(client2->queue_id, &msg_to_client, MAX_MSG_SIZE, 0);
}

void list_clients(msg* incoming_message) { //something wrong here
    int client_id = atoi(incoming_message->text);
    client* client = get_client_by_id(client_id);
    msg msg_to_client;
    msg_to_client.type = LIST;
    for (int i = 0; i < clients_no; i++)
    {
        if(clients_on_server[i]->connected_to_client == -1) 
            sprintf(msg_to_client.text, "client %d is free\n", clients_on_server[i]->id);
        else 
            sprintf(msg_to_client.text, "client %d is connected with another client\n", clients_on_server[i]->id);
    }
    msgsnd(client->queue_id, &msg_to_client, MAX_MSG_SIZE, 0);
    puts(msg_to_client.text);
}

void stop_client(msg* incoming_message) {
    int client_id = atoi(incoming_message->text);
    int index;
    for (int i = 0; i < clients_no; i++) 
    {
        if (clients_on_server[i]->id == client_id) 
            index = i;
    }
    client* to_delete = clients_on_server[index];
    for (int i = index; i < clients_no - 1; i++) 
    {
        clients_on_server[i] = clients_on_server[i + 1];
    }
    clients_on_server[clients_no - 1] = NULL;
    clients_no = clients_no-1;
    free(to_delete);
}

void handle_sigint() {
    msg msg_to_clients;
    msg_to_clients.type = STOP_SERVER;
    for (int i = 0; i < clients_no; i++) 
    {
        msgsnd(clients_on_server[i]->queue_id, &msg_to_clients, MAX_MSG_SIZE, 0);
    }     
    while (clients_no > 0) 
    {
        msg stop_client_msg;
        msgrcv(server_queue, &stop_client_msg, MAX_MSG_SIZE, STOP, 0);
        stop_client(&stop_client_msg);
    }
    msgctl(server_queue, IPC_RMID, NULL);
    exit(0);
}

int main() {
    char* path = getpwuid(getuid())->pw_dir;
    key_t queue_key = ftok(path, SERVER_ID); 
    server_queue = msgget(queue_key, IPC_CREAT | 0666);
    signal(SIGINT, handle_sigint);
    printf("Server ready, waiting for new clients... \n");
    while (1) {
        msg incoming_message;
        msgrcv(server_queue, &incoming_message, MAX_MSG_SIZE, ANY_MESSAGE, 0);
        puts("Got message from client: ");
        char to_print[MAX_MSG_SIZE * 2];
        if (incoming_message.type == STOP) 
        {
            stop_client(&incoming_message);
            sprintf(to_print, "STOP - client %s has quitted the server", incoming_message.text);
            puts(to_print);
            continue;
        }
        else if (incoming_message.type == DISCONNECT) 
        {
            disconnect_clients(&incoming_message);
            sprintf(to_print, "DISCONNECT - client %s has disconnected", incoming_message.text);
            puts(to_print);
            continue;
        }
        else if (incoming_message.type == LIST) 
        {
            sprintf(to_print, "LIST - listing clients:", incoming_message.text);
            puts(to_print);
            list_clients(&incoming_message);
            continue;
        }
        else if (incoming_message.type == CONNECT) 
        {
            sprintf(to_print, "CONNECT - connecting clients: %s", incoming_message.text);
            puts(to_print);
            connect_clients(&incoming_message);
            continue;
        }
        if (incoming_message.type == INIT)
        {
            init_client(&incoming_message);
            sprintf(to_print, "INIT - new client joined the server");
            puts(to_print);
            continue;
        }  
    }
}