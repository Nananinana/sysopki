#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h> 
#include <signal.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>


#include "header.h"

//Klient bezpośrednio po uruchomieniu tworzy kolejkę z unikalnym kluczem IPC i wysyła jej klucz komunikatem do serwera (komunikat INIT). 

int id;
int queue;
int connected_client_queue = -1;
int server_queue;

void stop_client() 
{
    msg msg_to_server;
    msg_to_server.type = STOP;
    sprintf(msg_to_server.text, "%d", id);
    msgsnd(server_queue, &msg_to_server, MAX_MSG_SIZE, 0);
    msgctl(queue, IPC_RMID, NULL);
    puts("Client queue deleted");
    exit(0);
}

void get_msg_from_queue(union sigval sv) 
{
    (void)sv;
    msg incoming_message;
    while (msgrcv(queue, &incoming_message, MAX_MSG_SIZE, ANY_MESSAGE, IPC_NOWAIT) != -1) 
    {
        if (incoming_message.type == CONNECT)
            connected_client_queue = atoi(incoming_message.text);
        else if (incoming_message.type == DISCONNECT) 
            connected_client_queue = -1;
        else if (incoming_message.type == SEND) 
            printf(incoming_message.text);
        else if (incoming_message.type == STOP_SERVER)
            stop_client();
        else
            puts(reply.text);
    }
}

void set_timer() 
{
    timer_t timer;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = get_msg_from_queue;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &event, &timer);
    struct timespec ten_ms = {0, 10000000};
    struct itimerspec timer_value = {ten_ms, ten_ms};
    timer_settime(timer, 0, &timer_value, NULL);
}



int main() 
{
    char *path = getpwuid(getuid())->pw_dir;
    key_t server_queue_key = ftok(path, SERVER_ID);
    server_queue = msgget(server_queue_key, 0666);
    key_t queue_key = ftok(path, getpid());
    queue = msgget(queue_key, IPC_CREAT | 0666);
    signal(SIGINT, stop_client);
    msg init_msg_to_server;
    init_msg_to_server.type = INIT;
    sprintf(init_msg_to_server.text, "%d", queue);
    msgsnd(server_queue, &init_msg_to_server, MAX_MSG_SIZE, 0);
    msg init_server_response;
    msgrcv(queue, &init_server_response, MAX_MSG_SIZE, INIT, 0);
    id = atoi(init_server_response.text);
    set_timer();
    printf("\n Your id: %d\n", id);
    printf("Type a command:\n LIST / CONNECT [id] / SEND [message] / DISCONNECT / STOP \n");
    char command[MAX_MSG_SIZE];

    while (fgets(command, sizeof(command), stdin)) 
    {
        msg msg_to_send;
        msg_to_send.type = -1;
        bool send_to_client = 0;
        if (strncmp(command, "LIST", strlen("LIST")) == 0) 
        {
            msg_to_send.type = LIST;
            sprintf(msg_to_send.text, "%d", id);
        }
        if (strncmp(command, "CONNECT", strlen("CONNECT")) == 0) {
            msg_to_send.type = CONNECT;
            (void)strtok(command, " ");
            int client2_id = atoi(strtok(NULL, " "));
            sprintf(msg_to_send.text, "%d %d", id, client2_id);
        }
        if ((strncmp(command, "SEND", strlen("SEND")) == 0) && connected_client_queue != -1) {
            msg_to_send.type = SEND;
            sprintf(msg_to_send.text, "%s", strchr(command, ' ') + 1);
            send_to_client = 1;
        }
        if (strncmp(command, "DISCONNECT", strlen("DISCONNECT")) == 0) {
            msg_to_send.type = DISCONNECT;
            sprintf(msg_to_send.text, "%d", id);
            connected_client_queue = -1;
        }
        if (strncmp(command, "STOP", strlen("STOP")) == 0) {
            stop_client();
        }
        if (msg_to_send.type != -1) {
            int where_to_send = send_to_client ? connected_client_queue : server_queue;
            msgsnd(where_to_send, &msg_to_send, MAX_MSG_SIZE, 0);
        }
    }
    stop_client();
}