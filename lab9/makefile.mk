main:
    gcc -o main barber.c -lpthread -lm
all:
	make clean
	gcc -o main barber.c -lpthread -lm
	./main 6 8
clean:
	rm -f main