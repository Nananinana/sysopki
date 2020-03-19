#ifdef DYNAMIC
#include "dynamic_library.h"
#else
#include "library.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

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
    fprintf(f, "%30s\t\t%15s\t%15s\t%15s\n",
            "name",
            "real_time [s]",
            "user_time [s]",
            "system_time [s]");
}

void save_timer(char *name, FILE *f)
{
    end_timer();
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(en_time - st_time) / clk_tics;
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tics;
    double system_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tics;
    fprintf(f, "%30s:\t\t%15f\t%15f\t%15f\t\n",
            name,
            real_time,
            user_time,
            system_time);
}
int parse_create_main_table(char *argv[], int i, int argc)
{
    if (i + 1 >= argc)
    {
        fprintf(stderr, "create table wrong number of arguments\n");
        return -1;
    }
    int size = atoi(argv[i + 1]);

    create_main_table(size);
    return 1;
}
int parse_end_timer(char *argv[], int i, int argc)
{
    if (i + 1 >= argc)
    {
        fprintf(stderr, "end timer wrong number of arguments\n");
        return -1;
    }
    end_timer();
    char *timer_name = argv[i + 1];

    save_timer(timer_name, report_file);
    return 0;
}

int parse_delete_block_of_operations(char *argv[], int i, int argc)
{

    if (i + 1 >= argc)
    {
        fprintf(stderr, "remove block wrong number of arguments\n");
        return -1;
    }
    int index = atoi(argv[i + 1]);
    delete_block_of_operations(index);
    return 1;
}

int parse_compare_pairs(char *argv[], int i, int argc)
{
    if (i + 3 >= argc)
    {
        fprintf(stderr, "search directory wrong number of arguments\n");
        return -1;
    }
    char *pairs = argv[i + 1];

    define_file_pairs(pairs);
    compare_pairs();
    return 1;
}

int parse_delete_operation(char *argv[], int i, int argc)
{
    int block_index = atoi(argv[i + 1]);
    int operation_index = atoi(argv[i + 2]);
    delete_operation(block_index, operation_index);
    return 1;
}

int main(int argc, char *argv[])
{
#ifdef DYNAMIC
    init();
#endif

    char file_name[] = "raport.txt";
    report_file = fopen(file_name, "a");

    write_file_header(report_file);

    int i = 1;
    while (i < argc)
    {
        if (!strcmp(argv[i], "create_main_table"))
        {
            int err = parse_create_main_table(argv, i, argc);
            if (err < 0)
            {
                fprintf(stderr, "function create_main_table returned error, stopping\n");
                return -1;
            }
            i += 2;
        }
        else if (!strcmp(argv[i], "compare_pairs"))
        {
            int err = parse_compare_pairs(argv, i, argc);
            if (err < 0)
            {
                fprintf(stderr, "function search_directory returned error, stopping\n");
                return -1;
            }

            i += 2;
        }
        else if (!strcmp(argv[i], "delete_operation"))
        {
            int err = parse_delete_operation(argv, i, argc);
            if (err < 0)
            {
                fprintf(stderr, "function delete_operation returned error, stopping\n");
                return -1;
            }
            i += 3;
        }
        else if (!strcmp(argv[i], "delete_block_of_operations"))
        {

            int err = parse_delete_block_of_operations(argv, i, argc);
            if (err < 0)
            {
                fprintf(stderr, "function delete_block_of_operations returned error, stopping\n");
                return -1;
            }
            i += 2;
        }
        else if (!strcmp(argv[i], "create_block_of_operations"))
        {
            create_block_of_operations();
            i++;
            parse_delete_block_of_operations(argv, i, argc);
            i += 2;
        }
        else if (!strcmp(argv[i], "start"))
        {
            start_timer();
            i += 1;
        }
        else if (!strcmp(argv[i], "end"))
        {
            parse_end_timer(argv, i, argc);
            i += 2;
        }
        else
        {
            fprintf(stderr, "Wrong command\n");
            return -1;
        }
    }

    delete_main_table();

    return 0;
}