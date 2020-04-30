#ifndef HEADER_H
#define HEADER_H

#include <mqueue.h>
#include <stdlib.h>

#define MAX_CLIENTS 7
#define MAX_MSG_SIZE 8192
#define MAX_FILENAME_SIZE 16
//#define SERVER_ID 1

#define STOP_SERVER 1L
#define STOP 2L
#define DISCONNECT 3L
#define LIST 4L
#define CONNECT 5L
#define INIT 6L
#define SEND 7L
#define ANY_MESSAGE -7L

char* read_from_queue(mqd_t source, char* type) {
    char from_queue[MAX_MSG_SIZE + 2] = {0};
    int is_msg_received = mq_receive(source, from_queue, MAX_MSG_SIZE + 1, NULL);
    if (is_msg_received == -1) return NULL;
    if (type) 
        *type = from_queue[0];
    char* msg_from_queue = calloc(MAX_MSG_SIZE + 1, sizeof(char));
    sprintf(msg_from_queue, "%s", from_queue + 1);
    return msg_from_queue;
}

void send_to_queue(mqd_t where_to_send, char type, char* msg) {
    int msg_size = strlen(msg);
    char* buffer = calloc(2 + msg_size, sizeof(char));
    buffer[0] = type;
    sprintf(buffer + 1, "%s", msg);
    mq_send(where_to_send, buffer, msg_size + 1, TYPES_COUNT - type + 1);
}

typedef struct 
{
    long type;
    char text[MAX_MSG_SIZE];
} msg;

typedef struct 
{
    int id;
    int queue_id;
    int connected_to_client;
    char* queue_filename;
} client;

#endif