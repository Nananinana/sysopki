#include "file_lib.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

clock_t st_time, en_time;
struct tms st_cpu, en_cpu;
FILE *report_file;

void start_timer()
{
    st_time = times(&st_cpu);
}

void end_timer()
{
    en_time = times(&en_cpu);
}

void write_file_header(FILE *f)
{
    fprintf(f, "%40s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]");
}

void save_timer(char *name, FILE *f)
{
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(en_time - st_time) / clk_tics;
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tics;
    double system_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tics;

    fprintf(f, "%40s:\t\t%15f\t%15f\t%15f\n",
            name,
            real_time,
            user_time,
            system_time);
}

int parse_generate(int argc, char *argv[], int i)
{
    if (i + 3 > argc)
    {
        fprintf(stderr, "generate command wrong args\n");
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
        fprintf(stderr, "sort command wrong args\n");
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
    save_timer(buff, report_file);

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
    save_timer(buff, report_file);

    return i + 6;
}

int main(int argc, char *argv[])
{
    char filename[] = "wyniki.txt";

    if (access(filename, F_OK) != 0)
    {
        report_file = fopen(filename, "a");
        write_file_header(report_file);
    }
    else
    {
        report_file = fopen(filename, "a");
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
    fclose(report_file);
    return 0;
}