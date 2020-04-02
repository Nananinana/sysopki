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
	./helper generate 7 14 5
	./matrix lista 6 3 1
	./help check lista

paste:
	make clean
	$(CC) helper.c -o helper
	$(CC) matrix.c -o matrix
	./helper generate 7 14 5
	./matrix lista 6 3 2

flock:
	make clean
	$(CC) helper.c -o helper
	$(CC) matrix.c -o matrix
	./helper generate 7 14 5
	./matrix lista 6 3 1
