CC = gcc
FLAGS = -std=c99 -Wall -pthread -g

server:
	$(CC) $(FLAGS) -o server server.c -lrt

client:
	$(CC) $(FLAGS) -o client client.c -lrt

test:
	make clean
	$(CC) $(FLAGS) -o server server.c -lrt
	$(CC) $(FLAGS) -o client client.c -lrt
	./server

clean:
	rm -rf client server