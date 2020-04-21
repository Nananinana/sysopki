CC = gcc
FLAGS = -std=c99 -Wall

all: main

main: main.c
	make clean
	$(CC) $(FLAGS) main.c -o main

clean:
	rm -f *.o *.a *.so
    
test: 
	./main tasks