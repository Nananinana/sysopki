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
    
    int seed;
    time_t t;
    seed = time(&t);
    srand(seed);

    for(int i = 0; i < records_number; i++)
    {
        for(int i = 0; i < record_size; i++)
        {
           int shift = rand()%26;
           int choice = rand()%2;
           if (choice == 0)
           { fputc('A'+shift, file); }
           else
           { fputc('a'+shift, file); }           
        }
        fputc('\n', file);
    }
    fclose(file);

    /*char buff[64];

    snprintf(buff, sizeof buff, "</dev/urandom tr -dc 'A-Z0-9a-z' | head -c %d > %s", records_number * record_size, filename);
    int find_status = system(buff);
    if (find_status != 0)
    {
        fprintf(stderr, "error while generating: %s\n", strerror(errno));
        exit(-1);
    }*/
}

void lib_swap_in_file(FILE *f, int records_number, int record_size, int i, int j)
{

    char *tmp1 = malloc(record_size);
    char *tmp2 = malloc(record_size);
    if (fseek(f, i * record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "1cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (fread(tmp1, 1, record_size, f) != record_size)
    {
        fprintf(stderr, "2cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fseek(f, (j)*record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "3cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (fread(tmp2, 1, record_size, f) != record_size)
    {
        fprintf(stderr, "4cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fseek(f, i * record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "5cant fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fwrite(tmp2, 1, record_size, f) != record_size)
    {
        fprintf(stderr, "error while writing to file sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fseek(f, (j)*record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "error while fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fwrite(tmp1, 1, record_size, f) != record_size)
    {
        fprintf(stderr, "error while fwrite sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    free(tmp1);
    free(tmp2);
}
int lib_partition(FILE *f, int records_number, int record_size, int low, int high)
{

    char *tmp1 = malloc(record_size);
    char *tmp2 = malloc(record_size);
    if (fseek(f, (high)*record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fread(tmp1, 1, record_size, f) != record_size)
    {

        fprintf(stderr, "6cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

 char min_char = tmp1[0];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (fseek(f, j * record_size, SEEK_SET) < 0)
        {
            fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
            exit(-1);
        }
        if (fread(tmp2, 1, record_size, f) != record_size)
        {
            fprintf(stderr, "7cant read sort lib: %s\n", strerror(errno));
            exit(-1);
        }

        if (tmp2[0] < min_char)
        {
            i++;
            lib_swap_in_file(f, records_number, record_size, i, j);
        }
    }
    lib_swap_in_file(f, records_number, record_size, i + 1, high);
    free(tmp1);
    free(tmp2);
    return (i + 1);
}
void lib_qsort(FILE *f, int records_number, int record_size, int low, int high)
{

    if (low < high)
    {
        int pi = lib_partition(f, records_number, record_size, low, high);
        if (pi != 0)
            lib_qsort(f, records_number, record_size, low, pi - 1);
        if (pi != records_number)
            lib_qsort(f, records_number, record_size, pi + 1, high);
    }
}

void lib_sort(char *filename, int records_number, int record_size)
{

    FILE *f = fopen(filename, "r+");

    if (f == NULL)
    {
        fprintf(stderr, "Can't open file sort lib: %s \n", strerror(errno));
        exit(-1);
    }
    lib_qsort(f, records_number, record_size, 0, records_number - 1);

    fclose(f);
}

void sys_swap_in_file(int f, int records_number, int record_size, int i, int j)
{

    char *tmp1 = malloc(record_size);
    char *tmp2 = malloc(record_size);
    if (lseek(f, i * record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "1cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (read(f, tmp1, record_size) < 0)
    {
        fprintf(stderr, "2cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (lseek(f, (j)*record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "3cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (read(f, tmp2, record_size) < 0)
    {

        fprintf(stderr, "4cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (lseek(f, i * record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "5cant fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (write(f, tmp2, record_size) < 0)
    {
        fprintf(stderr, "error while writing to file sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (lseek(f, (j)*record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "error while fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (write(f, tmp1, record_size) < 0)
    {
        fprintf(stderr, "error while fwrite sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    free(tmp1);
    free(tmp2);
}

int sys_partition(int f, int records_number, int record_size, int low, int high)
{

    char *tmp1 = malloc(record_size);
    char *tmp2 = malloc(record_size);
    if (lseek(f, (high)*record_size, SEEK_SET) < 0)
    {
        fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (read(f, tmp1, record_size) < 0)
    {

        fprintf(stderr, "6cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

 char min_char = tmp1[0];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (lseek(f, j * record_size, SEEK_SET) < 0)
        {
            fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
            exit(-1);
        }
        if (read(f, tmp2, record_size) < 0)
        {
            fprintf(stderr, "7cant read sort lib: %s\n", strerror(errno));
            exit(-1);
        }

        if (tmp2[0] < min_char)
        {
            i++;
            sys_swap_in_file(f, records_number, record_size, i, j);
        }
    }
    sys_swap_in_file(f, records_number, record_size, i + 1, high);
    free(tmp1);
    free(tmp2);
    return (i + 1);
}

void sys_qsort(int f, int records_number, int record_size, int low, int high)
{

    if (low < high)
    {
        int pi = sys_partition(f, records_number, record_size, low, high);
        if (pi != 0)
            sys_qsort(f, records_number, record_size, low, pi - 1);
        if (pi != records_number)
            sys_qsort(f, records_number, record_size, pi + 1, high);
    }
}

void sys_sort(char *filename, int records_number, int record_size)
{
    int f = open(filename, O_RDWR);
    if (f < 0)
    {
        fprintf(stderr, "cant open file sort sys: %s\n", strerror(errno));
        exit(-1);
    }
    sys_qsort(f, records_number, record_size, 0, records_number - 1);

    close(f);
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
