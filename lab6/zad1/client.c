#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "header.h"

//Klient bezpośrednio po uruchomieniu tworzy kolejkę z unikalnym kluczem IPC i wysyła jej klucz komunikatem do serwera (komunikat INIT). 

int own_id;
int client_queue;
int server_queue;
int other_queue = -1;

void stop_client() {
    msg message;
    message.type = STOP;
    sprintf(message.text, "%d", own_id);

    msgsnd(server_queue, &message, MAX_MSG_SIZE, 0);

    puts("Deleting queue...");
    msgctl(client_queue, IPC_RMID, NULL);
    exit(0);
}

void get_replies(union sigval sv) {
    (void)sv;
    msg reply;
    while (msgrcv(client_queue, &reply, MAX_MSG_SIZE, -TYPES_COUNT, IPC_NOWAIT) !=
           -1) {
        if (reply.type == CONNECT) {
            other_queue = atoi(reply.text);
        } else if (reply.type == SEND) {
            printf("MESSAGE: %s", reply.text);
        } else if (reply.type == DISCONNECT) {
            other_queue = -1;
        } else if (reply.type == STOP_SERVER) {
            stop_client();
        } else {
            puts(reply.text);
        }
    }
}

int starts_with(char *s, char *prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

void sigint_handler() { stop_client(); }

void set_timer() {
    timer_t timer;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = get_replies;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &event, &timer);
    struct timespec ten_ms = {0, 10000000};
    struct itimerspec timer_value = {ten_ms, ten_ms};
    timer_settime(timer, 0, &timer_value, NULL);
}

int main() {
    char *home_path = getpwuid(getuid())->pw_dir;

    key_t server_queue_key = ftok(home_path, SERVER_KEY_ID);
    server_queue = msgget(server_queue_key, 0666);

    key_t client_queue_key = ftok(home_path, getpid());
    client_queue = msgget(client_queue_key, IPC_CREAT | 0666);

    signal(SIGINT, sigint_handler);

    msg init;
    init.type = INIT;
    sprintf(init.text, "%d", client_queue);
    msgsnd(server_queue, &init, MAX_MSG_SIZE, 0);

    //Po otrzymaniu identyfikatora, klient może wysłać zlecenie do serwera(zlecenia są czytane ze standardowego wyjścia w postaci typ_komunikatu).

    msg init_ack;
    msgrcv(client_queue, &init_ack, MAX_MSG_SIZE, INIT, 0);
    own_id = atoi(init_ack.text);

    set_timer();
    char line[MAX_MSG_SIZE];

    printf("Your options:\n'list' - list all users\n'connect [id]' - connect with specific user\n'send [message]' - send message to user that you are connected with\n'disconnect' - disconnect from chat\n'exit' - exit from app\n");
    printf("\nown_id: %d\n", own_id);

    while (fgets(line, sizeof(line), stdin)) {
        msg message;
        message.type = -1;
        int is_msg_to_client = 0;

        if (starts_with(line, "list")) {
            message.type = LIST;
            sprintf(message.text, "%d", own_id);
        }

        if (starts_with(line, "connect")) {
            message.type = CONNECT;

            (void)strtok(line, " ");
            int second_id = atoi(strtok(NULL, " "));
            sprintf(message.text, "%d %d", own_id, second_id);
        }

        if (starts_with(line, "send") && other_queue != -1) {
            message.type = SEND;

            sprintf(message.text, "%s", strchr(line, ' ') + 1);
            is_msg_to_client = 1;
        }

        if (starts_with(line, "disconnect")) {
            message.type = DISCONNECT;
            sprintf(message.text, "%d", own_id);
            other_queue = -1;
        }

        if (starts_with(line, "exit")) {
            stop_client();
        }

        if (message.type != -1) {
            int destination = is_msg_to_client ? other_queue : server_queue;
            msgsnd(destination, &message, MAX_MSG_SIZE, 0);
        }
    }

    stop_client();
}