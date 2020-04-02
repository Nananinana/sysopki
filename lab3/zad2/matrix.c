#define _XOPEN_SOURCE 700
#define MAX_ROW_SIZE 1000
#define MAX_LINE_LENGTH (MAX_ROW_SIZE * 5)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "matrix_operations.c"

typedef struct
{
    int set_index;
    int column_index;
} fragment_to_compute;

int number_of_sets = 0;

fragment_to_compute get_fragment()
{
    fragment_to_compute fragment;
    fragment.column_index = -1;
    fragment.set_index = -1;
    for (int i = 0; i < number_of_sets; i++)
    {
        char *fragment_filename = calloc(100, sizeof(char));
        sprintf(fragment_filename, ".fragments/fragment%d", i);
        FILE *fragment_file = fopen(fragment_filename, "r+");
        int descriptor = fileno(fragment_file);
        flock(descriptor, LOCK_EX);
        char *fragments = calloc(1000, sizeof(char));
        fseek(fragment_file, 0, 0);
        fread(fragments, 1, 1000, fragment_file);
        char *first_zero = strchr(fragments, '0');
        int fragment_index;
        if (first_zero != NULL)
            first_zero= first_zero - fragments;
        else
            first_zero--;
        if (fragment_index >= 0)
        {
            char *end_of_line = strchr(fragments, '\0');
            int size = end_of_line - fragments;
            char *fragments_with_good_size = calloc(size + 1, sizeof(char));
            for (int j = 0; j < size; j++)
            {
                fragments_with_good_size[j] = fragments[j];
            }
            fragments_with_good_size[fragment_index] = '1';
            fseek(fragment_file, 0, 0);
            fwrite(fragments_with_good_size, 1, size, fragment_file);
            fragment.set_index = i;
            fragment.column_index = fragment_index;
            flock(descriptor, LOCK_UN);
            fclose(fragment_file);
            break;
        }
        flock(descriptor, LOCK_UN);
        fclose(fragment_file);
    }
    return fragment;
}

void multiply_column_paste(char *fileA, char *fileB, int column_index, int set_index)
{
    matrix matrixA = load_matrix_from_file(fileA);
    matrix matrixB = load_matrix_from_file(fileB);
    char *result_fragment_filename = calloc(20, sizeof(char));
    sprintf(result_fragment_filename, ".fragments/result_fragment%d%04d", set_index, column_index); 
    FILE *result_fragment_file = fopen(result_fragment_filename, "w+");
    for (int i = 0; i < matrixA.rows; i++)
    {
        int result_fragment = 0;
        for (int j = 0; j < matrixA.columns; j++)
            result_fragment += matrixA.values[i][j] * matrixB.values[i][column_index];
        if (i == matrixA.rows - 1)
            fprintf(result_fragment_file, "%d", result_fragment);
        else
            fprintf(result_fragment_file, "%d\n", result_fragment);
    }
    fclose(result_fragment_file);
}

void multiply_column_to_shared_file(char *fileA, char *fileB, int column_index, char *result_file)
{
    matrix matrixA = load_matrix_from_file(fileA);
    matrix matrixB = load_matrix_from_file(fileB);
    FILE *file = fopen(result_file, "r+");
    int descriptor = fileno(file);
    flock(descriptor, LOCK_EX);
    matrix matrix_result = load_matrix_from_file(result_file);
    for (int i = 0; i < matrixA.rows; i++)
    {
        int result = 0;
        for (int j = 0; j < matrixA.columns; j++)
        {
            result += matrixA.values[i][j] * matrixB.values[j][column_index];
        }
        matrix_result.values[i][column_index] = result;
    }
    print_matrix_to_file(file, matrix_result);
    flock(descriptor, LOCK_UN);
    fclose(file);
}

int worker_function(char **fileA, char **fileB, int timeout, int mode, char **result_file)
{
    time_t start_time = time(NULL);
    int multiplication_count = 0;
    while (1)
    {
        if ((time(NULL) - start_time) >= timeout)
        {
            printf("timeout \n");
            break;
        }
        fragment_to_compute fragment = get_fragment();
        if (fragment.column_index == -1)
            break;
        if (mode == 1)
            multiply_column_to_shared_file(fileA[fragment.set_index], fileB[fragment.set_index], fragment.column_index, result_file[fragment.set_index]);
        else
            multiply_column_paste(fileA[fragment.set_index], fileB[fragment.set_index], fragment.column_index, fragment.set_index);
        multiplication_count++;
    }
    return multiplication_count;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "./matrix [list] [process number] [timeout] [mode: 1 if flock else 2]");
        return 1;
    }
    int processes_number = atoi(argv[2]);
    int timeout = atoi(argv[3]);
    int mode = atoi(argv[4]);
    system("rm -rf .fragments");
    char **filenamesA = calloc(100, sizeof(char *));
    char **filenamesB = calloc(100, sizeof(char *));
    char **filenamesC = calloc(100, sizeof(char *));
    system("mkdir -p .fragments");
    FILE *input_file = fopen(argv[1], "r");
    char current_line[PATH_MAX * 3 + 3];
    int line_number = 0;
    while (fgets(current_line, PATH_MAX * 3 + 3, input_file) != NULL)
    {
        filenamesA[line_number] = calloc(PATH_MAX, sizeof(char));
        strcpy(filenamesA[line_number], strtok(current_line, " "));
        filenamesB[line_number] = calloc(PATH_MAX, sizeof(char));
        strcpy(filenamesB[line_number], strtok(NULL, " "));
        filenamesC[line_number] = calloc(PATH_MAX, sizeof(char));   
        strcpy(filenamesC[line_number], strtok(NULL, " "));
        matrix matrixA = load_matrix_from_file(filenamesA[line_number]);
        matrix matrixB = load_matrix_from_file(filenamesB[line_number]);
        if (mode == 1)
            create_empty_matrix(matrixA.rows, matrixB.columns, filenamesC[line_number]);
        char *fragment_filename = calloc(100, sizeof(char));
        sprintf(fragment_filename, ".fragments/fragment%d", line_number);
        FILE *fragment_file = fopen(fragment_filename, "w+");
        char *fragments = calloc(matrixB.columns + 1, sizeof(char));
        sprintf(fragments, "%0*d", matrixB.columns, 0);
        fwrite(fragments, 1, matrixB.columns, fragment_file);
        free(fragments);
        free(fragment_filename);
        fclose(fragment_file);
        line_number++;
    }
    number_of_sets = line_number;
    pid_t *processes = calloc(processes_number, sizeof(int));
    for (int i = 0; i < processes_number; i++)
    {
        pid_t worker = fork();
        if (worker == 0)
            return worker_function(filenamesA, filenamesB, timeout, mode, filenamesC);
        else
            processes[i] = worker;
    }
    for (int i = 0; i < processes_number; i++)
    {
        int status;
        waitpid(processes[i], &status, 0);
        printf("Proccess %d have done %d matrix multiplications\n", processes[i],
               WEXITSTATUS(status));
    }
    free(processes);
    if (mode == 2)
    {
        for (int i = 0; i < line_number; i++)
        {
            char *buffer = calloc(1000, sizeof(char));
            sprintf(buffer, "paste .fragments/result_fragment%d* > %s", i, filenamesC[i]);
            system(buffer);
        }
    }
    return 0;
}