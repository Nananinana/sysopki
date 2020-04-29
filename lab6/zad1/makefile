CC = gcc
FLAGS = -std=c99 -Wall

server:
	$(CC) $(FLAGS) -o server server.c

client:
	$(CC) $(FLAGS) -o client client.c

test:
	make clean
	$(CC) $(FLAGS) -o server server.c 
	$(CC) $(FLAGS) -o client client.c
	./server

clean:
	rm -rf client server