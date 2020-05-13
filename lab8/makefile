CC=gcc
CFLAGS=-Wall -o

main: main.c
	$(CC) $(CFLAGS) main main.c -lpthread -lm

sign:
	./main 1 sign casablanca.ascii.pgm result.txt
	./main 2 sign casablanca.ascii.pgm result.txt
	./main 4 sign casablanca.ascii.pgm result.txt
	./main 8 sign casablanca.ascii.pgm result.txt

block:
	./main 1 block casablanca.ascii.pgm result.txt
	./main 2 block casablanca.ascii.pgm result.txt
	./main 4 block casablanca.ascii.pgm result.txt
	./main 8 block casablanca.ascii.pgm result.txt

interleaved:
	./main 1 interleaved casablanca.ascii.pgm result.txt
	./main 2 interleaved casablanca.ascii.pgm result.txt
	./main 4 interleaved casablanca.ascii.pgm result.txt
	./main 8 interleaved casablanca.ascii.pgm result.txt

clean:
	rm -f main result.txt Times.txt

all:
	make clean
	make main
	make sign
	make block
	make interleaved