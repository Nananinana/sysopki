CC = gcc
FLAGS = -std=c99 -Wall
SOURCE_NAME = file_lib
PROGRAM_NAME  = program

static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o lib$(SOURCE_NAME).o
	ar rcs lib$(SOURCE_NAME).a lib$(SOURCE_NAME).o
	make clean

program:
	make static
	$(CC) $(FLAGS) $(PROGRAM_NAME).c -o $(PROGRAM_NAME) lib$(SOURCE_NAME).a

test:
	make program 
	rm -f wyniki.txt

	for records_number in 5000 7000 10000; do \
		for record_size in 1 4 512  1024  4096 8192 ; do \
			./$(PROGRAM_NAME) generate data $$records_number $$record_size ; \
			./$(PROGRAM_NAME) copy data data.$$records_number.$$record_size.lib $$records_number $$record_size lib ; \
			./$(PROGRAM_NAME) copy data data.$$records_number.$$record_size.sys $$records_number $$record_size sys ; \
		done; \
	done

	for records_number in 5000 7000 10000; do \
		for record_size in 1 4 512  1024  4096 8192 ; do \
			./$(PROGRAM_NAME) sort data.$$records_number.$$record_size.lib $$records_number $$record_size lib ; \
			./$(PROGRAM_NAME) sort data.$$records_number.$$record_size.sys $$records_number $$record_size sys ; \
		done; \
	done

	rm -rf data*


clean:
	rm -f *.o