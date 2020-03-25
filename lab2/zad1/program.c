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

void start_timer()
{
    start_time = times(&start_cpu);
}

void end_timer()
{
    end_time = times(&end_cpu);
}

/*void write_file_header(FILE *file)
{
    fprintf(file, "%40s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "real [s]",
            "user [s]",
            "system [s]");
}

void save_timer(char *name, FILE *f)
{
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(end_time - start_time) / clk_tics;
    double user_time = (double)(end_cpu.tms_utime - start_cpu.tms_utime) / clk_tics;
    double system_time = (double)(end_cpu.tms_stime - start_cpu.tms_stime) / clk_tics;

    fprintf(f, "%40s:\t\t%15f\t%15f\t%15f\n",
            name,
            real_time,
            user_time,
            system_time);
}*/

/*
int parse_generate(int argc, char *argv[], int i)
{
    if (i + 3 > argc)
    {
        fprintf(stderr, "wrong arguments for generate command\n");
        exit(-1);
    }
    char *filename = argv[i + 1];
    int records_number = atoi(argv[i + 2]);
    int record_size = atoi(argv[i + 3]);
    generate(filename, records_number, record_size);
    return i + 4;
}

int parse_sort(int argc, char *argv[], int i)
{
    if (i + 4 >= argc)
    {
        fprintf(stderr, "wrong arguments for sort command\n");
        exit(-1);
    }
    char *filename = argv[i + 1];
    int records_number = atoi(argv[i + 2]);
    int record_size = atoi(argv[i + 3]);
    char *lib_sys = argv[i + 4];

    if (!strcmp(lib_sys, "lib"))
    {
        start_timer();
        lib_sort(filename, records_number, record_size);
        end_timer();
    }
    else if (!strcmp(lib_sys, "sys"))
    {
        start_timer();
        sys_sort(filename, records_number, record_size);
        end_timer();
    }
    char buff[64];
    snprintf(buff, sizeof buff, "sort %s, records: %d, bytes: %d", lib_sys, records_number, record_size);
    save_timer(buff, wyniki);

    return i + 5;
}

int parse_copy(int argc, char *argv[], int i)
{
    if (i + 5 >= argc)
    {
        fprintf(stderr, "sort command wrong args\n");
        exit(-1);
    }

    char *file1 = argv[i + 1];
    char *file2 = argv[i + 2];
    int records_number = atoi(argv[i + 3]);
    int record_size = atoi(argv[i + 4]);
    char *lib_sys = argv[i + 5];

    if (!strcmp(lib_sys, "lib"))
    {
        start_timer();
        lib_copy(file1, file2, records_number, record_size);
        end_timer();
    }
    else if (!strcmp(lib_sys, "sys"))
    {
        start_timer();
        sys_copy(file1, file2, records_number, record_size);
        end_timer();
    }

    char buff[64];
    snprintf(buff, sizeof buff, "copy %s, records: %d, bytes: %d", lib_sys, records_number, record_size);
    save_timer(buff, wyniki);

    return i + 6;
}

int main(int argc, char *argv[])
{
    char filename[] = "wyniki.txt";

    if (access(filename, F_OK) != 0)
    {
        wyniki = fopen(filename, "a");
        write_file_header(wyniki);
    }
    else
    {
        wyniki = fopen(filename, "a");
    }

    int i = 1;
    while (i < argc)
    {
        if (!strcmp(argv[i], "generate"))
        {
            i = parse_generate(argc, argv, i);
        }
        else if (!strcmp(argv[i], "sort"))
        {
            i = parse_sort(argc, argv, i);
        }
        else if (!strcmp(argv[i], "copy"))
        {
            i = parse_copy(argc, argv, i);
        }
        else
        {
            fprintf(stderr, "Wrong command\n");
            exit(-1);
        }
    }
    fclose(wyniki);
    return 0;
}
*/

char *available_commands[3] = {"generate", "sort", "copy"};

int check_command(char *command)
{
    for (int i = 0; i < 3; i++)
    {
        if (strcmp(command, available_commands[i]) == 0)
        {
            return i + 1;
        }
    }
    return 0;
}

double get_time(clock_t start, clock_t end)
{
    return (double)(end - start) / sysconf(_SC_CLK_TCK);
}

void write_time(FILE *wyniki, struct tms *start, struct tms *end)
{
    fprintf(wyniki, "\tuser: %fl\n", get_time(start->tms_utime, end->tms_utime));
    fprintf(wyniki, "\tsystem: %fl\n\n", get_time(start->tms_stime, end->tms_stime));
}

int main(int argc, char **argv)
{

    FILE *wyniki;
    if (argc < 2 || check_command(argv[1]) == 0)
    {
        printf("ewrong command\n ");
        exit(1);
    }

    struct tms *start = (struct tms *)calloc(1, sizeof(struct tms));
    struct tms *end = (struct tms *)calloc(1, sizeof(struct tms));

    wyniki = fopen("wyniki.txt", "a");
    if (wyniki == NULL)
    {
        perror("File wyniki couldn't be openned");
        exit(1);
    }

    int command = check_command(argv[1]);

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
        int record_size = (int)strtol(argv[4], NULL, 10);
        char *sort_lib_or_sys = argv[5];

        if (strcmp(sort_lib_or_sys, "lib"))
        {
            times(start);
            lib_sort(file, records_number, record_size);
            times(end);
            fprintf(wyniki, "Sort in mode lib file having %d records %d bytes each\n", records_number, record_size);
            write_time(wyniki, start, end);
        }
        else
        {
            times(start);
            sys_sort(file, records_number, record_size);
            times(end);
            fprintf(wyniki, "Sort in mode sys file having %d records %d bytes each\n", records_number, record_size);
            write_time(wyniki, start, end);
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
        printf(" %d %d \n", records_number, record_size);

        char *copy_lib_or_sys = argv[6];

        if (strcmp(copy_lib_or_sys, "lib"))
        {
            times(start);
            lib_copy(file1, file2, records_number, record_size);
            times(end);
            fprintf(wyniki, "Copy in mode lib file having %d records %d bytes each\n", records_number, record_size);
            write_time(wyniki, start, end);
        }
        else
        {
            times(start);
            sys_copy(file1, file2, records_number, record_size);
            times(end);
            fprintf(wyniki, "Copy in mode sys file having %d records %d bytes each\n", records_number, record_size);
            write_time(wyniki, start, end);
        }
    }

    fclose(wyniki);
    return 0;
}
