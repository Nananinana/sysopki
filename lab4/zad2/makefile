all:
	make fork
	make exec

exec:
	gcc -Wall -std=c99 -o  fork fork.c
	gcc -Wall -std=c99  -o exec exec.c

fork:
	gcc -Wall -std=c99  -o fork fork.c

clean:
	rm -rf fork exec