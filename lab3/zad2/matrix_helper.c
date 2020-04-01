#define _XOPEN_SOURCE 500
#define MAX_set_column_and_rows_NUMBER 1000
#define MAX_LINE_LENGTH (MAX_set_column_and_rows_NUMBER * 5)
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct
{
    int **values;
    int rows;
    int cols;
} matrix;

int get_cols_number(char *row)
{
    int columns = 0;
    char *num = strtok(row, " ");
    while (num != NULL)
    {
        if (strcmp(num, "\n") != 0)
            columns++;
        num = strtok(NULL, " ");
    }
    return columns;
}

void set_cols_and_rows(FILE *f, int *rows, int *columns)
{
    char line[MAX_LINE_LENGTH];
    *rows = 0;
    *cols = 0;
    while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
    {
        if (*columns == 0)
        {
            *columns = get_cols_number(line);
        }
        *rows = *rows + 1;
    }
    fseek(f, 0, SEEK_SET);
}

matrix load_matrix(char *filename)
{
    FILE *file = fopen(filename, "r");
    int rows, columns;
    set_cols_and_rows(file, &rows, &columns);
    int **values = calloc(rows, sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        values[i] = calloc(columns, sizeof(int));
    }
    int x, y = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        x = 0;
        char *number = strtok(line, " \t\n");

        while (number != NULL)
        {
            values[y][x++] = atoi(number);
            number = strtok(NULL, " \t\n");
        }
        y++;
    }

    fclose(file);
    matrix m;
    m.values = values;
    m.rows = rows;
    m.columns = columns;
    return m;
}

void free_matrix(matrix *m)
{
    for (int y = 0; y < m->rows; y++)
    {
        free(m->values[y]);
    }
    free(m->values);
}

void print_matrix(matrix m)
{
    for (int i = 0; i < m.rows; i++)
    {
        for (int j = 0; j < m.columns; j++)
        {
            printf("%d ", m.values[i][j]);
        }
        printf("\n");
    }
}

matrix multiply_matrices(matrix A, matrix B)
{
    int **values = calloc(A.rows, sizeof(int *));
    for (int i = 0; i < A.rows; i++)
    {
        values[i] = calloc(B.columns, sizeof(int));
    }
    for (int i = 0; i < A.rows; i++)
    {
        for (int j = 0; j < B.columns; j++)
        {
            int result = 0;
            for (int k = 0; k < A.columns; k++)
            {
                result += (A.values[i][k] * B.values[k][j]);
            }
            values[i][j] = result;
        }
    }

    matrix m;
    m.values = values;
    m.rows = A.rows;
    m.columns = B.columns;

    return m;
}

void generate_matrix_to_file(int rows, int columns, char *filename)
{
    FILE *file = fopen(filename, "w+");

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            if (x > 0)
            {
                fprintf(file, " ");
            }

            fprintf(file, "%d", rand() % (200 + 1) - 100);
        };
        fprintf(file, "\n");
    }
    fclose(file);
}

void init_free_matrix(int rows, int cols, char *filename)
{
    FILE *file = fopen(filename, "w+");

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            if (x > 0)
            {
                fprintf(file, " ");
            }

            fprintf(file, "%d", 0);
        };
        fprintf(file, "\n");
    }
    fclose(file);
}

void write_matrix_to_file(FILE *file, matrix a)
{
    fseek(file, 0, SEEK_SET);
    for (int y = 0; y < a.rows; y++)
    {
        for (int x = 0; x < a.columns; x++)
        {
            if (x > 0)
            {
                fprintf(file, " ");
            }
            fprintf(file, "%d", a.values[y][x]);
        };
        fprintf(file, "\n");
    }
}