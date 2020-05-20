#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h> 

#include "header.h"

int id;
mqd_t queue;
mqd_t server_queue;
mqd_t connected_client_queue = -1;
char filename[MAX_FILENAME_SIZE+ 1];

void stop_client() 
{
    char msg_to_server[MAX_MSG_SIZE + 1] = {0};
    sprintf(msg_to_server, "%d", id);
    send_to_queue(server_queue, STOP, msg_to_server);
    mq_unlink(filename);
    puts("Client queue deleted");
    exit(0);
}

void proceed_msg(union sigval sv);

void register_msg() 
{
    struct sigevent sigevent;
    sigevent.sigev_notify = SIGEV_THREAD;
    sigevent.sigev_notify_function = proceed_msg;
    sigevent.sigev_notify_attributes = NULL;
    sigevent.sigev_value.sival_ptr = NULL;
    mq_notify(queue, &sigevent);
}

void proceed_msg(union sigval sv)
{ 
    (void)sv;
    register_msg();
    char *msg_text;
    char type;
    while ((msg_text = read_from_queue(queue, &type)) != NULL) {
        switch (type) 
        {
            case CONNECT:
                connected_client_queue = mq_open(msg_text, O_RDWR, 0666, NULL);
                break;
            case SEND:
                printf(msg_text);
                break;
            case DISCONNECT:
                if (connected_client_queue) {
                    mq_close(connected_client_queue);
                }
                connected_client_queue = -1;
                break;
            case STOP_SERVER:
                stop_client();
                break;
            default:
                puts(msg_text);
        }
    }
}

int main() 
{
    sprintf(filename, "/%d", getpid());
    queue = mq_open(filename, O_RDWR | O_CREAT, 0666, NULL);
    server_queue = mq_open("/server", O_RDWR, 0666, NULL);
    signal(SIGINT, stop_client);
    send_to_queue(server_queue, INIT, filename);
    char *id_from_queue = read_from_queue(queue, NULL);
    id = atoi(id_from_queue);
    free(id_from_queue);
    printf("\n Your id: %d\n", id);
    printf("Type a command:\n LIST / CONNECT [id] / SEND [message] / DISCONNECT / STOP \n");
    register_msg();
    struct mq_attr attr;
    mq_getattr(queue, &attr);
    attr.mq_flags = O_NONBLOCK;
    mq_setattr(queue, &attr, NULL);
    char command[256];

    while (fgets(command, sizeof(command), stdin)) 
    {
        char msg_to_send[MAX_MSG_SIZE + 1] = {0};
        int type = -1;
        bool send_to_client = 0;
        if (strncmp(command, "LIST", strlen("LIST")) == 0) 
        {
            type = LIST;
            sprintf(msg_to_send, "%d", id);
        }
        if (strncmp(command, "CONNECT", strlen("CONNECT")) == 0) {
            type = CONNECT;
            (void)strtok(command, " ");
            int client2_id = atoi(strtok(NULL, " "));
            sprintf(msg_to_send, "%d %d", id, client2_id);
        }
        if ((strncmp(command, "SEND", strlen("SEND")) == 0) && connected_client_queue != -1) {
            type = SEND;
            sprintf(msg_to_send, "%s", strchr(command, ' ') + 1);
            send_to_client = 1;
        }
        if (strncmp(command, "DISCONNECT", strlen("DISCONNECT")) == 0) {
            type = DISCONNECT;
            sprintf(msg_to_send, "%d", id);
            connected_client_queue = -1;
        }
        if (strncmp(command, "STOP", strlen("STOP")) == 0) {
            stop_client();
        }
        if (type != -1) {
            mqd_t where_to_send = send_to_client ? connected_client_queue : server_queue;
            send_to_queue(where_to_send, type, msg_to_send);
            sleep(1);
        }
    }
    stop_client();
}
