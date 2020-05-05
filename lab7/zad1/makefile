CC=gcc -Wall

all: build test clean

build:
	$(CC) -o program program.c
	$(CC) -o worker1 worker1.c
	$(CC) -o worker2 worker2.c
	$(CC) -o worker3 worker3.c

test:
	./program

clean:
	rm -f program worker_1 worker_2 worker_3