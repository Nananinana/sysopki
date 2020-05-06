CC=gcc -Wall

all: build test clean

program: 
	$(CC) -o program program.c common.h

worker1:
	$(CC) -o worker1 worker1.c common.h

worker2:
	$(CC) -o worker1 worker1.c common.h

worker3:
	$(CC) -o worker3 worker3.c common.h

build:
	$(CC) -o program program.c common.h
	$(CC) -o worker1 worker1.c common.h
	$(CC) -o worker2 worker2.c common.h
	$(CC) -o worker3 worker3.c common.h

test:
	./program

clean:
	rm -f program worker1 worker2 worker3