all:
	make fork
	make exec
	make clean

exec:
	gcc -Wall -std=c99 -o  fork fork.c
	gcc -Wall -std=c99  -o exec exec.c
	./fork ignore exec
	./fork pending exec
	./fork mask exec
	./fork handler exec

fork:
	gcc -Wall -std=c99  -o fork fork.c
	./fork ignore n
	./fork pending n
	./fork mask n
	./fork handler n

clean:
	rm -rf fork exec