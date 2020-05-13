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

int threads_count;
int **histogram;

int width;
int height;
int **picture;

struct thread_info
{
    int thread_number;
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

void *perform_thread(void *info)
{
    struct thread_info *t_info = info;
    if (strcmp(t_info->mode, "sign") == 0)
        return sign(t_info->thread_number);
    else if (strcmp(t_info->mode, "block") == 0)
        return block(t_info->thread_number);
    else if (strcmp(t_info->mode, "interleaved") == 0)
        return interleaved(t_info->thread_number);
    else
    {
        printf("Wrong command. Mode can be sign, block or interleaved\n");
        exit(-1);
    }
}

void save_histogram_to_file(char *file_name) {
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

void print_thread_time(pthread_t *threads, FILE *times_file)
{
    for (int i = 0; i < threads_count; i++) { //print time?
        double *thread_time;
        pthread_join(threads[i], (void *)&thread_time);
        printf("Thread number %d , thread time: %lf microseconds\n", i, *returned_value);
        fprintf(times_file, "Thread number: %d, thread time: %lf microseconds\n", i, *thread_time);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Wrong number of arguments!\n");
        exit(-1);
    }
    threads_count = atoi(argv[1]);
    char *mode = argv[2];
    char *input_file_name = argv[3];
    char *output_file_name = argv[4];
    load_picture_from_file(input_file_name);
    histogram = calloc(threads_count, sizeof(int *));
    for (int i = 0; i < threads_count; i++) {
        histogram[i] = calloc(MAX_SIZE, sizeof(int));
    }

    FILE *times_file = fopen("Times.txt", "a");
    fprintf(times_file, "Mode: %s , number of threads: %d\n", mode, threads_count);
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    pthread_t *threads = calloc(threads_count, sizeof(pthread_t));
    struct thread_info *threads_info = calloc(threads_count, sizeof(struct thread_info));
    for (int i = 0; i < threads_count; i++) {
        struct thread_info info;
        info.thread_number = i;
        info.mode = mode;
        threads_info[i] = info;
        pthread_create(&threads[i], NULL, perform_thread, (void *)&threads_info[i]);
    }

    print_thread_time(threads, times_file);
        /*
        double *thread_time;
        pthread_join(threads[i], (void *)&thread_time);
        printf("Thread number %d , thread time: %lf microseconds\n", i, *returned_value);
        fprintf(times_file, "Thread number: %d, thread time: %lf microseconds\n", i, *thread_time);
        */
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    double full_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_nsec - start_time.tv_nsec) / 1000.0;
    printf("\nFull time: %f\n", full_time);
    fprintf(times_file, "Full time: %f\n\n", full_time);
    save_histogram_to_file(output_file_name);

    for (int i = 0; i < height; i++) {
        free(picture[i]);
    }
    free(picture);
    for (int i = 0; i < threads_count; i++) {
        free(histogram[i]);
    }
    free(histogram);
    fclose(times_file);
    free(threads);
    free(threads_info);
    return 0;
}
