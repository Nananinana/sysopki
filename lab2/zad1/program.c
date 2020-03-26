#include "file_lib.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

FILE *wyniki;
clock_t start_time, end_time;
struct tms start_cpu, end_cpu;
char *available_commands[3] = {"generate", "sort", "copy"};

double get_time(clock_t start, clock_t end)
{
    return (double)(end - start) / sysconf(_SC_CLK_TCK);
}

void print_time(FILE *wyniki, struct tms *start, struct tms *end)
{
    fprintf(wyniki, "\tuser: %fl\n", get_time(start->tms_utime, end->tms_utime));
    fprintf(wyniki, "\tsystem: %fl\n\n", get_time(start->tms_stime, end->tms_stime));
}

int parse_command(char *command)
{
    for (int i = 0; i < 3; i++)
    {
        if (strcmp(command, available_commands[i]) == 0)
            return i + 1;    }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *wyniki;
    if (argc < 2 || parse_command(argv[1]) == 0)
    {
        printf("Wrong command\n ");
        exit(1);
    }
    struct tms *start = (struct tms *)calloc(1, sizeof(struct tms));
    struct tms *end = (struct tms *)calloc(1, sizeof(struct tms));
    wyniki = fopen("wyniki.txt", "a");
    if (wyniki == NULL)
    {
        perror("File wyniki couldn't be opened");
        exit(1);
    }
    int command = parse_command(argv[1]);
    if (command == 1)
    {
        if (argc < 5)
        {
            printf("Wrong command\n ");
            fclose(wyniki);
            exit(1);
        }
        char *file = argv[2];
        int records_number = (int)strtol(argv[3], NULL, 10);
        int record_size = (int)strtol(argv[4], NULL, 10);
        generate(file, records_number, record_size);
    }
    else if (command == 2)
    { 
        if (argc < 6)
        {
            printf("Wrong command\n ");
            fclose(wyniki);
            exit(1);
        }
        char *file = argv[2];
        int records_number = (int)strtol(argv[3], NULL, 10);
        printf ("records number is: %d \n", records_number);
        int record_size = (int)strtol(argv[4], NULL, 10);
        printf ("record size is: %d \n", record_size);
        char *sort_lib_or_sys = argv[5];
        if (strcmp(sort_lib_or_sys, "lib"))
        {
            times(start);
            lib_sort(file, records_number, record_size);
            times(end);
            fprintf(wyniki, "Lib sort, records: %d, record size: %d\n", records_number, record_size);
            print_time(wyniki, start, end);
        }
        else
        {
            times(start);
            sys_sort(file, records_number, record_size);
            times(end);
            fprintf(wyniki, "Sys sort, records: %d, record size: %d\n", records_number, record_size);
            print_time(wyniki, start, end);
        }
    }
    else if (command == 3)
    {
        if (argc < 7)
        {
            printf("Wrong command\n ");
            fclose(wyniki);
            exit(1);
        }
        char *file1 = argv[2];
        char *file2 = argv[3];
        int records_number = (int)strtol(argv[4], NULL, 10);
        int record_size = (int)strtol(argv[5], NULL, 10);
        //printf(" %d %d \n", records_number, record_size);
        char *copy_lib_or_sys = argv[6];
        if (strcmp(copy_lib_or_sys, "lib"))
        {
            times(start);
            lib_copy(file1, file2, records_number, record_size);
            times(end);
            fprintf(wyniki, "Lib copy, records: %d, record size: %d\n", records_number, record_size);
            print_time(wyniki, start, end);
        }
        else
        {
            times(start);
            sys_copy(file1, file2, records_number, record_size);
            times(end);
            fprintf(wyniki, "Sys copy, records: %d, record size: %d\n", records_number, record_size);
            print_time(wyniki, start, end);
        }
    }
    fclose(wyniki);
    return 0;
}
