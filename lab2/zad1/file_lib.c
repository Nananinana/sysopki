#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

void generate(char *filename, int records_number, int record_size)
{
    FILE * file = fopen(filename, "w");
    /*int seed;
    time_t t;
    seed = time(&t);*/
    srand(time(NULL));

    for(int i = 0; i < records_number; i++)
    {
        for(int j = 0; j < record_size; j++)
        {
           int offset = rand()%26;
           int character_case = rand()%2;
           if (character_case == 0)
           { fputc('A'+ offset, file); }
           else
           { fputc('a'+ offset, file); }           
        }
        fputc('\n', file);
    }
    fclose(file);
}

void lib_swap_records(FILE *file, int records_number, int record_size, int record_index1, int record_index2)
{

    char *buffer1 = malloc(record_size);
    char *buffer2 = malloc(record_size);
    fseek(file, record_index1 * record_size, SEEK_SET);  //znajduje i odczytuje do dwoch buforow zadane rekordy
    fread(buffer1, 1, record_size, file);
    fseek(file, record_index2 * record_size, SEEK_SET);
    fread(buffer2, 1, record_size, file);
    fseek(file, record_index1 * record_size, SEEK_SET);  //potem z buforow wpivotsuje je w zamienione miejsca
    fwrite(buffer2, 1, record_size, file);
    fseek(file, record_index2*record_size, SEEK_SET);
    fwrite(buffer1, 1, record_size, file);
    free(buffer1);
    free(buffer2);
}

int lib_partition(FILE *file, int records_number, int record_size, int start, int end)
{
    char *buffer1 = malloc(record_size);
    char *buffer2 = malloc(record_size);
    fseek(file, (end)*record_size, SEEK_SET);
    fread(buffer1, 1, record_size, file);
    int i = start - 1;
    char min = buffer1[0];

    for (int j = start; j < end; j++)
    {
        fseek(file, j * record_size, SEEK_SET);
        fread(buffer2, 1, record_size, file);
        if (buffer2[0] < min)
        {
            i++;
            lib_swap_records(file, records_number, record_size, i, j);
        }
    }
    lib_swap_records(file, records_number, record_size, i + 1, end);
    free(buffer1);
    free(buffer2);
    return (i + 1);
}

void lib_quicksort(FILE *file, int records_number, int record_size, int start, int end)
{

    if (start < end)
    {
        int pivot = lib_partition(file, records_number, record_size, start, end);
        lib_quicksort(file, records_number, record_size, start, pivot - 1);
        lib_quicksort(file, records_number, record_size, pivot + 1, end);
    }
}

void lib_sort(char *filename, int records_number, int record_size)
{

    FILE *file = fopen(filename, "r+");
    lib_quicksort(file, records_number, record_size, 0, records_number - 1);
    fclose(file);
}

void sys_swap_records(int file, int records_number, int record_size, int record_index1, int record_index2)
{
    char *buffer1 = malloc(record_size);
    char *buffer2 = malloc(record_size);
    lseek(file, record_index1 * record_size, SEEK_SET);
    read(file, buffer1, record_size);
    lseek(file, record_index2 * record_size, SEEK_SET);
    read(file, buffer2, record_size);
    lseek(file, record_index1 * record_size, SEEK_SET);
    write(file, buffer2, record_size);
    lseek(file, record_index2 * record_size, SEEK_SET);
    write(file, buffer1, record_size);
    free(buffer1);
    free(buffer2);
}

int sys_partition(int file, int records_number, int record_size, int start, int end)
{
    char *buffer1 = malloc(record_size);
    char *buffer2 = malloc(record_size);
    lseek(file, end * record_size, SEEK_SET);
    read(file, buffer1, record_size);
 
    int i = start - 1;
    char min = buffer1[0];

    for (int j = start; j < end; j++)
    {
        lseek(file, j * record_size, SEEK_SET);
        read(file, buffer2, record_size);
        if (buffer2[0] < min)
        {
            i++;
            sys_swap_records(file, records_number, record_size, i, j);  }
    }
    sys_swap_records(file, records_number, record_size, i + 1, end);
    free(buffer1);
    free(buffer2);
    return (i + 1);
}

void sys_quicksort(int file, int records_number, int record_size, int start, int end)
{
    if (start < end)
    {
        int pivot = sys_partition(file, records_number, record_size, start, end);
        sys_quicksort(file, records_number, record_size, start, pivot - 1);
        sys_quicksort(file, records_number, record_size, pivot + 1, end);
    }
}

void sys_sort(char *filename, int records_number, int record_size)
{
    int file = open(filename, O_RDWR);  
    sys_quicksort(file, records_number, record_size, 0, records_number - 1);
    close(file);
}

void sys_copy(char *file1, char *file2, int records_number, int record_size)
{
    int from = open(file1, O_RDONLY);
    if (from < 0)
    {
        fprintf(stderr, "cant open source file copy sys: %s\n", strerror(errno));
        exit(-1);
    }
    int to = open(file2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (to < 0)
    {
        fprintf(stderr, "cant open destination file copy sys: %s\n", strerror(errno));
        exit(-1);
    }

 char *holder = malloc(record_size);
    for (int i = 0; i < records_number; ++i)
    {

        if (read(from, holder, record_size) < 0)
        {
            fprintf(stderr, "cant read from source file copy sys: %s\n", strerror(errno));
            exit(-1);
        }
        if (write(to, holder, record_size) < 0)
        {
            fprintf(stderr, "cant write to dst file copy sys: %s\n", strerror(errno));
            exit(-1);
        }
    }
    free(holder);
    close(from);
    close(to);
}

void lib_copy(char *file1, char *file2, int records_number, int record_size)
{
    FILE *src_file = fopen(file1, "r");
    if (src_file == NULL)
    {
        fprintf(stderr, "cant open source file copy lib: %s\n", strerror(errno));
        exit(-1);
    }
    FILE *dst_file = fopen(file2, "w");
    if (dst_file == NULL)
    {
        fprintf(stderr, "cant open dst file copy lib: %s\n", strerror(errno));
        exit(-1);
    }

 char *holder = malloc(record_size);
    for (int i = 0; i < records_number; i++)
    {
        if (fread(holder, 1, record_size, src_file) != record_size)
        {
            fprintf(stderr, "cant read from source file copy lib: %s\n", strerror(errno));
            exit(-1);
        }
        if (fwrite(holder, 1, record_size, dst_file) != record_size)
        {
            fprintf(stderr, "cant write to dst file copy lib: %s\n", strerror(errno));
            exit(-1);
        }
    }
    free(holder);
    fclose(src_file);
    fclose(dst_file);
}
