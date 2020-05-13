#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#define MAX_SIZE 256

int width;
int height;
int **picture;


int threads_count;
int **histogram;

struct arg_struct
{
    int arg1;
    char *mode;
};


void load_picture_from_file(char *file_name) {
    int value;
    FILE *pgm_file = fopen(file_name, "r");
    if (pgm_file == NULL) 
    {
        printf("Cant open file \n");
        exit(-1);
    }
    while (getc(pgm_file) != '\n'); //skip header
    while (getc(pgm_file) == '#') //skip comments
        while (getc(pgm_file) != '\n');
    fseek(pgm_file, -1, SEEK_CUR);
    fscanf(pgm_file, "%d %d", &width, &height);
    printf("width: %d, height:  %d\n", width, height);

    picture = calloc(height, sizeof(int *));
    for(int i = 0; i < height; i++)
        picture[i] = calloc(width, sizeof(int));
    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column++)
        {
            fscanf(pgm_file, "%d", &value);
            picture[row][column] = value;  }
    }
    fclose(pgm_file);
}

void *sign(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = 255 / threads_count;
    for (int w = 0; w < height; w++)
    {
        for (int c = 0; c < width; c++)
        {
            if (picture[w][c] / size == index)
            {
                histogram[index][picture[w][c]]++;
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}

void *block(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = width / threads_count;
    for (int col = index * size; col < (index + 1) * size; col++)
    {
        for (int w = 0; w < height; w++)
        {
            histogram[index][picture[w][col]]++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}
void *interleaved(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int col = index; col < width; col += threads_count)
    {
        for (int w = 0; w < height; w++)
        {
            histogram[index][picture[w][col]]++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}
void *write_to_histogram(void *arg)
{
    struct arg_struct *args = arg;
    if (strcmp(args->mode, "sign") == 0)
    {
        return sign(args->arg1);
    }
    else if (strcmp(args->mode, "block") == 0)
    {
        return block(args->arg1);
    }
    else if (strcmp(args->mode, "interleaved") == 0)
    {
        return interleaved(args->arg1);
    }
    else
    {
        printf("unknow mode\n");
        exit(EXIT_FAILURE);
    }
}
void save_histogram(char *file_name) {
    FILE *output_file = fopen(file_name, "w");
    if (output_file == NULL) 
    {
        printf("Cant open file \n");
        exit(-1);
    }
    fprintf(output_file, "gray scale - count\n");
    int count;
    for (int i = 0; i < MAX_SIZE; i++)
    {
        count = 0;
        for (int j = 0; j < threads_count; j++)
        {
            count += histogram[j][i];
        }
        fprintf(output_file, "%d - %d\n", i, count);
    }
    fclose(output_file);
}

int main(int argc, char *argv[])
{

    threads_count = atoi(argv[1]);
    char *mode = argv[2];
    char *input_file = argv[3];
    char *output_file = argv[4];
    char buffer[256];
    load_picture_from_file(input_file);

    FILE *txt = fopen("Times.txt", "a");
    fprintf(txt, "Mode: %s | threads: %d\n", mode, threads_count);

    histogram = calloc(threads_count, sizeof(int *));
    for (int i = 0; i < threads_count; i++)
        histogram[i] = calloc(256, sizeof(int));

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t *thread_ids = calloc(threads_count, sizeof(pthread_t));
    struct arg_struct *args = calloc(threads_count, sizeof(struct arg_struct));
    for (int i = 0; i < threads_count; i++)
    {
        struct arg_struct arg;
        arg.arg1 = i;
        arg.mode = mode;

        args[i] = arg;

        pthread_create(&thread_ids[i], NULL, write_to_histogram, (void *)&args[i]);
    }

    for (int i = 0; i < threads_count; i++)
    {
        double *y;
        pthread_join(thread_ids[i], (void *)&y);
        printf("Thread %d ------- %lf microseconds\n", i, *y);
        fprintf(txt, "Thread %d ------- %lf microseconds\n", i, *y);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    printf("\nFULL TIME: %f\n", time);
    fprintf(txt, "FULL TIME: %f\n\n", time);

    save_histogram(output_file);

    // clean
    for (int i = 0; i < height; i++)
        free(picture[i]);
    free(picture);
    for (int i = 0; i < threads_count; i++)
        free(histogram[i]);
    free(histogram);
    fclose(txt);
    return 0;
}