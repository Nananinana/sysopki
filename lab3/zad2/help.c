#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <linux/limits.h>
#include "matrix_helper.c"

bool check_multiplication_result(char *fileA, char *fileB, char *result_file) //pomocniczy - sprawdza poprawnosc
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

int main(int argc, char **argv)
{
    char *command = calloc(10, sizeof(char));
    command = argv[1]; //?
    if (strcmp(command, "generate") == 0)
    {
        srand(time(NULL));
        int min = atoi(argv[2]);
        int max = atoi(argv[3]);
        int number = atoi(argv[4]);
        //system("mkdir files");
        for (int i = 0; i < number; i++)
        {
            int rows_matrixA = rand() % (max - min + 1) + min;
            int columns_matrixA = rand() % (max - min + 1) + min;
            int columns_matrixB = rand() % (max - min + 1) + min;
            char *name_matrixA = calloc(100, sizeof(char));
            sprintf(name_matrixA, "matrixA%d.txt", i);
            char *name_matrixB = calloc(100, sizeof(char));
            sprintf(name_matrixB, "matrixB%d.txt", i);
            char *name_matrix_result = calloc(100, sizeof(char));         
            sprintf(name_matrix_result, "matrix_result%d.txt", i);
            generate_matrix_to_file(rows_matrixA, columns_matrixA, name_matrixA);
            generate_matrix_to_file(columns_matrixA, columns_matrixB, name_matrixB);
            char *to_print = calloc(1000, sizeof(char));
            sprintf(to_print, "echo \"%s %s %s\" >> lista", name_matrixA, name_matrixB, name_matrix_result);
            system(to_print);
        }
    }
    else if (strcmp(command, "check") == 0)
    {

        char **namefileA = calloc(100, sizeof(char *));
        char **namefileB = calloc(100, sizeof(char *));
        char **result_namefile = calloc(100, sizeof(char *));
        int line_number = 0;
        FILE *input_file = fopen(argv[2], "r");
        char current_line[300]; 
        while (fgets(current_line, 300, input_file) != NULL)
        {
            namefileA[line_number] = calloc(100, sizeof(char));
            namefileB[line_number] = calloc(100, sizeof(char));
            result_namefile[line_number] = calloc(100, sizeof(char));
            strcpy(namefileA[line_number], strtok(current_line, " "));
            strcpy(namefileB[line_number], strtok(NULL, " "));
            strcpy(result_namefile[line_number], strtok(NULL, " "));

            bool multiplication_correct = check_multiplication_result(namefileA[line_number], namefileB[line_number], result_namefile[line_number]);
            if (multiplication_correct)
                printf("correct multiplication result \n");
            else
                printf("correct multiplication result \n");
            line_number++;
        }
    }
    else
    {
        printf("Wrong command");
    }
    return 0;
}