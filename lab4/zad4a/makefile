CC = gcc
FLAGS = -Wall -std=c99 

all: sender catcher

catcher: catcher.c
	$(CC) $(FLAGS) catcher.c -o catcher

sender: sender.c
	$(CC) $(FLAGS) sender.c -o sender

clean:
	rm -rf sender catcher
