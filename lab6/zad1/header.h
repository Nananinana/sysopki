  
#ifndef HEADER_H
#define HEADER_H

#define MAX_CLIENTS 7
#define MAX_MSG_SIZE 256 //?
#define SERVER_KEY_ID 1
#define STOP_SERVER 1L
#define STOP 2L
#define DISCONNECT 3L
#define LIST 4L
#define CONNECT 5L
#define INIT 6L
#define SEND 7L
#define ANY_MESSAGE 7L

typedef struct {
    long type;
    char text[MAX_MSG_SIZE];
} msg;

typedef struct { 
    int id;
    int queue_id;
    int connected_client_id;
} client;

#endif

/*
 Małe liczby do wygenerowania kluczy oraz rodzaje komunikatów mają być zdefiniowane we wspólnym pliku nagłówkowym. 
Dla uproszczenia można założyć, że długość komunikatu jest ograniczona pewną stałą (jej definicja powinna znaleźć się w pliku nagłówkowym). */