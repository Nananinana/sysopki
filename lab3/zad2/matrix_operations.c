#define _XOPEN_SOURCE 700
#define MAX_ROW_SIZE 1000
#define MAX_LINE_LENGTH (MAX_ROW_SIZE * 5)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef struct
{
    int rows;
    int columns;
    int **values;
} matrix;

void print_matrix(matrix matrix)
{
    for (int i = 0; i < matrix.rows; i++)
    {
        for (int j = 0; j < matrix.columns; j++)
            printf("%d ", matrix.values[i][j]);
        printf("\n");
    }
}

void free_matrix(matrix *matrix)
{
    for (int i = 0; i < matrix->rows; i++)
        free(matrix->values[i]);
    free(matrix->values);
}

matrix load_matrix_from_file(char *filename)
{   FILE *file;
    if ((file = fopen(filename, "r")) == NULL)
    {
        printf("Couldnt open file %s", filename);
            exit(1);
    }
    matrix matrix;
    matrix.rows = 0;
    matrix.columns = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
        matrix.rows++;
    fseek(file, 0, 0);
    fgets(line, MAX_LINE_LENGTH, file);
    if ( strtok(line, " ") != NULL) 
    {
        matrix.columns = 1;
        while(strtok(NULL, " ") != NULL) 
            matrix.columns++;
    }
    fseek(file, 0, 0);
    matrix.values = calloc(matrix.rows, sizeof(int *));
    for (int i = 0; i < matrix.rows; i++)
        matrix.values[i] = calloc(matrix.columns, sizeof(int));
    int i = 0;
    int j = 0;
     while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {  
        i = 0;
        char *value = strtok(line, " \t\n");
        while (value != NULL)
        {
            matrix.values[j][i++] = atoi(value);
            value = strtok(NULL, " \t\n");  }
        j++;
    }
    fseek(file, 0, 0);
    fclose(file);
    return matrix;
}

matrix multiply_matrixes(matrix matrixA, matrix matrixB)
{
    matrix matrix_result;
    matrix_result.rows = matrixA.rows;
    matrix_result.columns = matrixB.columns;
    matrix_result.values = calloc(matrixA.rows, sizeof(int *));
    for (int i = 0; i < matrixA.rows; i++)
        matrix_result.values[i] = calloc(matrixB.columns, sizeof(int));
    for (int i = 0; i < matrixA.rows; i++) 
    {
        for (int j = 0; j < matrixB.columns; j++)
        {
            int result = 0;
            for (int k = 0; k < matrixA.columns; k++)
            {
                result += (matrixA.values[i][k] * matrixB.values[k][j]);
            }
            matrix_result.values[i][j] = result;
        }
    }
    return matrix_result;
}

void generate_matrix_to_file(int rows, int columns, char *filename) 
{
    FILE *file = fopen(filename, "w+");
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            if (x > 0)
                fprintf(file, " ");
            fprintf(file, "%d", rand() % (200 + 1) - 100);
        };
        fprintf(file, "\n");
    }
    fclose(file);
}

void create_empty_matrix(int rows, int columns, char *filename)
{
    FILE *file = fopen(filename, "w+");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (j > 0)
                fprintf(file, " ");
            fprintf(file, "%d", 0);
        };
        fprintf(file, "\n");
    }
    fclose(file);
}

void print_matrix_to_file(FILE *file, matrix matrix)
{
    fseek(file, 0, 0);
    for (int i = 0; i < matrix.rows; i++)
    {
        for (int j = 0; j < matrix.columns; j++)
        {
            if (j > 0)
                fprintf(file, " ");
            fprintf(file, "%d", matrix.values[i][j]);
        };
        fprintf(file, "\n");
    }
}

bool check_multiplication_result(char *fileA, char *fileB, char *result_file) 
{
    matrix matrixA = load_matrix_from_file(fileA);
    matrix matrixB = load_matrix_from_file(fileB);
    matrix matrix_result = load_matrix_from_file(result_file);
    matrix correct_matrix = multiply_matrixes(matrixA, matrixB);
    if (correct_matrix.rows != matrix_result.rows || correct_matrix.columns != matrix_result.columns)
        return false;
    for (int i = 0; i < correct_matrix.rows; i++)
    {
        for (int j = 0; j < correct_matrix.columns; j++)
        {
            if (correct_matrix.values[i][j] != matrix_result.values[i][j])
                return false;
        }
    }
    free_matrix(&matrixA);
    free_matrix(&matrixB);
    free_matrix(&matrix_result);
    free_matrix(&correct_matrix);
    return true;
}