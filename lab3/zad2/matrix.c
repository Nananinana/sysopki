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
#include "matrix_helper.c"

typedef struct
{
    int set_index;
    int column_index;
} task;

int number_of_sets = 0;

task get_task()
{
    Task task;
    task.column_index = -1;
    task.pair_index = -1;
    for (int i = 0; i < number_of_sets; i++)
    {
        char *task_filename = calloc(100, sizeof(char));
        sprintf(task_filename, ".tmp/tasks%d", i);
        FILE *tasks_file = fopen(task_filename, "r+");
        int fd = fileno(tasks_file);
        flock(fd, LOCK_EX);
        char *tasks = calloc(1000, sizeof(char));
        fseek(tasks_file, 0, 0);
        fread(tasks, 1, 1000, tasks_file);
        char *task_first_zero = strchr(tasks, '0');
        int task_index = task_first_zero != NULL ? task_first_zero - tasks : -1;
        if (task_index >= 0)
        {
            char *end_of_line = strchr(tasks, '\0');
            int size = end_of_line - tasks;

            char *tasks_with_good_size = calloc(size + 1, sizeof(char));
            for (int j = 0; j < size; j++)
            {
                tasks_with_good_size[j] = tasks[j];
            }
            tasks_with_good_size[task_index] = '1';
            fseek(tasks_file, 0, 0);
            fwrite(tasks_with_good_size, 1, size, tasks_file);
            task.pair_index = i;
            task.column_index = task_index;
            flock(fd, LOCK_UN);
            fclose(tasks_file);
            break;
        }

        flock(fd, LOCK_UN);
        fclose(tasks_file);
    }
    return task;
}

void multiply_column(char *fileA, char *fileB, int column_index, int set_index)
{
    matrix matrixA = load_matrix_from_file(fileA);
    matrix matrixB = load_matrix_from_file(fileB);
    char *result_fragment_filename = calloc(20, sizeof(char));
    sprintf(result_fragment_filename, "result_fragment%d%04d", set_index, column_index); //.tmp/part%d%04d
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

void multiply_column_to_one_file(char *a_file, char *b_file, int column_index, char *result_file)
{
    matrix A = load_matrix_from_file(a_file);
    matrix B = load_matrix_from_file(b_file);
    FILE *file = fopen(result_file, "r+");
    int fd = fileno(file);
    flock(fd, LOCK_EX);
    matrix C = load_matrix_from_file(result_file);
    for (int y = 0; y < A.rows; y++)
    {
        int result = 0;
        for (int x = 0; x < A.columns; x++)
        {
            result += A.values[y][x] * B.values[x][column_index];
        }
        C.values[y][column_index] = result;
    }
    print_matrix_to_file(file, C);
    flock(fd, LOCK_UN);
    fclose(file);
}

int worker_function(char **a, char **b, int timeout, int mode, char **result_file)
{
    time_t start_time = time(NULL);
    int multiplies_count = 0;
    while (1)
    {
        if ((time(NULL) - start_time) >= timeout)
        {
            puts("timeout");
            break;
        }
        Task task = get_task();
        if (task.column_index == -1)
        {
            break;
        }
        if (mode == 1)
            multiply_column_to_one_file(a[task.pair_index], b[task.pair_index], task.column_index, result_file[task.pair_index]);
        else
            multiply_column(a[task.pair_index], b[task.pair_index], task.column_index, task.pair_index);

        multiplies_count++;
    }
    return multiplies_count;
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

    char **a_filenames = calloc(100, sizeof(char *));
    char **b_filenames = calloc(100, sizeof(char *));
    char **c_filenames = calloc(100, sizeof(char *));
    system("rm -rf .tmp");
    system("mkdir -p .tmp");
    FILE *input_file = fopen(argv[1], "r");
    char input_line[PATH_MAX * 3 + 3];
    int pair_counter = 0;
    while (fgets(input_line, PATH_MAX * 3 + 3, input_file) != NULL)
    {
        a_filenames[pair_counter] = calloc(PATH_MAX, sizeof(char));
        b_filenames[pair_counter] = calloc(PATH_MAX, sizeof(char));
        c_filenames[pair_counter] = calloc(PATH_MAX, sizeof(char));

        strcpy(a_filenames[pair_counter], strtok(input_line, " "));
        strcpy(b_filenames[pair_counter], strtok(NULL, " "));
        strcpy(c_filenames[pair_counter], strtok(NULL, " "));

        matrix a = load_matrix_from_file(a_filenames[pair_counter]);
        matrix b = load_matrix_from_file(b_filenames[pair_counter]);
        if (mode == 1)
            create_empty_matrix(a.rows, b.columns, c_filenames[pair_counter]);

        char *task_filename = calloc(100, sizeof(char));
        sprintf(task_filename, ".tmp/tasks%d", pair_counter);
        FILE *tasks_file = fopen(task_filename, "w+");
        char *tasks = calloc(b.columns + 1, sizeof(char));
        sprintf(tasks, "%0*d", b.columns, 0);
        fwrite(tasks, 1, b.columns, tasks_file);
        free(tasks);
        free(task_filename);
        fclose(tasks_file);

        pair_counter++;
    }
    number_of_sets = pair_counter;

    pid_t *processes = calloc(processes_number, sizeof(int));
    for (int i = 0; i < processes_number; i++)
    {
        pid_t worker = fork();
        if (worker == 0)
        {
            return worker_function(a_filenames, b_filenames, timeout, mode, c_filenames);
        }
        else
        {
            processes[i] = worker;
        }
    }

    for (int i = 0; i < processes_number; i++)
    {
        int status;
        waitpid(processes[i], &status, 0);
        printf("Proces %d wykonal %d mnozen macierzy\n", processes[i],
               WEXITSTATUS(status));
    }
    free(processes);

    if (mode == 2)
    {
        for (int i = 0; i < pair_counter; i++)
        {
            char *buffer = calloc(1000, sizeof(char));
            sprintf(buffer, "paste .tmp/part%d* > %s", i, c_filenames[i]);
            system(buffer);
        }
    }

    return 0;
}