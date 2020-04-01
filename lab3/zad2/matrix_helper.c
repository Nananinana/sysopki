#define _XOPEN_SOURCE 700
#define MAX_ROW_SIZE 1000
#define MAX_LINE_LENGTH (MAX_ROW_SIZE * 5)
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
        }
        printf("\n");
    }
}

void free_matrix(matrix *matrix)
{
    for (int i = 0; i < matrix->rows; i++)
        free(matrix->values[i]);
    free(matrix->values);
}



/*int get_cols_number(char *row)
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
}*/
/*
void get_matrix_size(FILE *file, int *rows, int *columns)
{
    char * line = NULL;
    size_t line_size = 0;
    
    *rows = 0;
    *columns = 0;
    while(getline(&line, &line_size, file) >= 0)
    {
        *rows++;
    } 
    fseek(file, 0, 0);
    getline(&line, &line_size, file);
    if ( strtok(line, " ") != NULL) 
    {
        *columns = 1;
        while(strtok(NULL, " ") != NULL) 
            *columns++;
    }
    fseek(file, 0, 0);
}*/

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
    //get_matrix_size(file, &rows, &columns); //getDimensions(file, &rows, &cols);
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
    int **values = calloc(matrix.rows, sizeof(int *));
    for (int i = 0; i < matrix.rows; i++)
        values[i] = calloc(matrix.columns, sizeof(int));
    int i = 0;
    int j = 0;
     while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {  
        i = 0;
        char *value = strtok(line, " \t\n");
        while (value != NULL)
        {
            values[j][i++] = atoi(value);
            value = strtok(NULL, " \t\n");  }
        j++;
    }
    fseek(file, 0, 0);
    fclose(file);
    matrix.values = values;
    return matrix;
}



matrix multiply_matrixes(matrix matrixA, matrix matrixB)
{
    int **values = calloc(matrixA.rows, sizeof(int *));
    matrix matrix_result;
    for (int i = 0; i < matrixA.rows; i++)
        values[i] = calloc(matrixB.columns, sizeof(int));
    for (int i = 0; i < matrixA.rows; i++) 
    {
        for (int j = 0; j < matrixB.columns; j++)
        {
            int result = 0;
            for (int k = 0; k < matrixA.columns; k++)
            {
                result += (matrixA.values[i][k] * matrixB.values[k][j]);
            }
            values[i][j] = result;
        }
    }
    matrix_result.values = values;
    matrix_result.rows = A.rows;
    matrix_result.columns = B.columns;
    return matrix_result;
}

void generate_matrix_to_file(int rows, int columns, char *filename)
{
    FILE *file = fopen(filename, "w");

    for (int y = 0; y < columns; y++)
    {
        for (int x = 0; x < rows; x++)
        {
            if (x > 0)
            {
                fprintf(file, " ");
            }
            char str[7];
            sprintf(str,"%d ", (rand() % 201) - 100);
            fwrite(str, sizeof(char), strlen(str), file); 
            //fprintf(file, "%d", rand() % (200 + 1) - 100);
        };
        fseek(file, -1 * sizeof(char), SEEK_CUR);
        fwrite("\n", sizeof(char), strlen("\n"), file);
        //fprintf(file, "\n");
    }
    fclose(file);
}

void init_free_matrix(int rows, int columns, char *filename)
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