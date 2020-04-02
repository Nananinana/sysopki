CC = gcc -Wall -std=c11 -ggdb

all: 
	make test

clean:
	rm -f -r .fragments matrixes helper matrix lista

helper:
	rm -f helper
	$(CC) helper.c -o helper

test:
	make clean
	$(CC) helper.c -o helper
	$(CC) matrix.c -o matrix
	./helper generate 5 15 6
	./matrix lista 5 4 1
	./help check lista

paste:
	make clean
	$(CC) helper.c -o helper
	$(CC) matrix.c -o matrix
	./helper generate 5 15 6
	./matrix lista 5 4 2

shared:
	make clean
	$(CC) helper.c -o helper
	$(CC) matrix.c -o matrix
	./helper generate 5 15 6
	./matrix lista 5 4 1
