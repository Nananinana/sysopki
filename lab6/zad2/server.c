#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <fcntl.h>
#include <mqueue.h>
#include "header.h"
//	gcc -pthread -o"FreeRTOS_Posix" $(OBJS) $(USER_OBJS) $(LIBS) -lrt

mqd_t server_queue;
client* clients_on_server[MAX_CLIENTS] = {NULL};
int clients_no = 0;
int next_client_id = 0;

void init_client(char* msg_text) 
{
    mqd_t client_queue_id = mq_open(msg_text, O_RDWR, 0666, NULL);
    client* new_client = calloc(1, sizeof(client));
    new_client -> id = next_client_id;
    new_client -> queue_id = client_queue_id;
    new_client -> connected_to_client = -1;
    sprintf(new_client->queue_filename, "%s", msg_text);
    clients_on_server[clients_no] = new_client;
    clients_no++;
    next_client_id++;
    char msg_to_client[MAX_MSG_SIZE + 1];
    sprintf(msg_to_client, "%d", new_client->id);
    send_to_queue(new_client->queue_id, INIT, msg_to_client);
} 

client* get_client_by_id(int client_id) 
{
    for (int i = 0; i < clients_no; i++) 
    {
        if (clients_on_server[i]->id == client_id) 
            return clients_on_server[i];
    }
    return NULL;
}

void connect_clients(char* msg_text) 
{
    int client1_id = atoi(strtok(msg_text, " "));
    client* client1 = get_client_by_id(client1_id);
    int client2_id = atoi(strtok(NULL, " "));    
    client* client2 = get_client_by_id(client2_id);
    client1->connected_to_client = client2->id;
    client2->connected_to_client = client1->id;
    char msg_to_client[MAX_MSG_SIZE + 1] = {0};
    sprintf(msg_to_client, "%s", client2->queue_filename);
    send_to_queue(client1->queue_id, CONNECT, msg_to_client);
    sprintf(msg_to_client, "%s", client1->queue_filename);
    send_to_queue(client2->queue_id, CONNECT, msg_to_client);
}

void disconnect_clients(char* msg_text) 
{
    int client1_id = atoi(msg_text);
    client* client1 = get_client_by_id(client1_id);
    client* client2 = get_client_by_id(client1->connected_to_client);
    client1->connected_to_client = -1;
    client2->connected_to_client = -1;
    char msg_to_client[MAX_MSG_SIZE + 1] = {0};
    send_to_queue(client2->queue_id, DISCONNECT, msg_to_client);
}

void list_clients(char* msg_text) 
{ 
    int client_id = atoi(msg_text);
    client* client = get_client_by_id(client_id);
    char msg_to_client[MAX_MSG_SIZE + 1] = {0};
    for (int i = 0; i < clients_no; i++) 
    {
        sprintf(msg_to_client + strlen(msg_to_client), "%d: %d\n", clients_on_server[i]->id,
        clients_on_server[i]->connected_to_client == -1);
    }
    send_to_queue(client->queue_id, LIST, msg_to_client);
}

void stop_client(char* msg_text) {
    int client_id = atoi(msg_text);
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
    mq_close(to_delete->queue_id);
    free(to_delete->queue_filename);
    free(to_delete);
}

void handle_sigint() {
    char msg_to_clients[MAX_MSG_SIZE + 1] = {0};
    for (int i = 0; i < clients_no; i++) 
    {
        send_to_queue(clients_on_server[i]->queue_id, STOP_SERVER, msg_to_clients);
    }     
    while (clients_no > 0) 
    {   
        char type;
        char* stop_client_msg = read_from_queue(server_queue, &type);
        puts(stop_client_msg);

        if (type == STOP) {
            stop_client(stop_client_msg);
        }
        free(stop_client_msg);
    }
    mq_close(server_queue);
    mq_unlink("/server");
    exit(0);
}

int main() 
{
    server_queue = mq_open("/server", O_RDWR | O_CREAT, 0666, NULL);
    signal(SIGINT, handle_sigint);
    printf("Server ready, waiting for new clients... \n");
    while (1) {
        char type;
        char* msg_text = read_from_queue(server_queue, &type);
        puts("Got message from client: ");
        char to_print[MAX_MSG_SIZE * 2];
        if (type == STOP) 
        {
            stop_client(msg_text);
            sprintf(to_print, "STOP - client %s has quitted the server", msg_text);
            puts(to_print);
            continue;
        }
        else if (type == DISCONNECT) 
        {
            disconnect_clients(msg_text);
            sprintf(to_print, "DISCONNECT - client %s has disconnected", msg_text);
            puts(to_print);
            continue;
        }
        else if (type == LIST) 
        {
            sprintf(to_print, "LIST - listing clients:");
            puts(to_print);
            list_clients(msg_text);
            continue;
        }
        else if (type == CONNECT) 
        {
            connect_clients(msg_text);
            sprintf(to_print, "CONNECT - connected clients: %s", msg_text);
            puts(to_print);
            continue;
        }
        if (type == INIT)
        {
            init_client(msg_text);
            sprintf(to_print, "INIT - new client joined the server");
            puts(to_print);
            continue;
        }  
        free(msg_text);
    } 
}