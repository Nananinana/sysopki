CC = gcc
FLAGS = -std=c99 -Wall

main:
	$(CC) $(FLAGS) -o main main.c 

producer:
	$(CC) $(FLAGS) -o producer producer.c

consumer:
	$(CC) $(FLAGS) -o consumer consumer.c

test:
	make input
	make main
	make producer
	make consumer
	./main 
	

clean:
	rm producer main consumer *.txt pipe

inputs:
	echo -n "1111111111111111111111111111111111111111111111111111111111111111111111" > input1.txt
	echo -n "2222222222222222222222222222222222222222222222222222222222222222222222" > input2.txt
	echo -n "3333333333333333333333333333333333333333333333333333333333333333333333" > input3.txt
	echo -n "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" > input4.txt
	echo -n "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" > input5.txt