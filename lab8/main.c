#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#define MAX_LINE_LENGTH 128

int width;
int height;
int **picture;


int threads;
int **histogram;

struct arg_struct
{
    int arg1;
    char *mode;
};

/*void get_dimensions(char *buffer) {
    char *rest = buffer;
    char *new_height;    
    new_height = strtok_r(rest, " \t\r\n", &rest);
    height = atoi(new_height);
    char *new_width;
    new_width = strtok_r(rest, " \t\r\n", &rest);
    width = atoi(new_width);
}

./*void load_row_to_array(char *line, int r)
{
    char *i;
    char *rest = line;
    for (int col = 0; col < width; col++)
    {
        i = strtok_r(rest, " \t\r\n", &rest);
        image[r][col] = atoi(i);
    }
}*/
void load_image_to_array(char *filename)
{
    int value;
    FILE *pgm_file = fopen(filename, "r");
    if (pgm_file == NULL) 
    {
        printf("Cant open file \n");
        exit(-1);
    }
    /*
    char buffer[MAX_LINE_LENGTH + 1];
    // skip header
    fgets(buffer, MAX_LINE_LENGTH, f);
    // skip comments
    */

    while (getc(pgm_file) != '\n'); //skip to the end of line
    while (getc(pgm_file) == '#') //skip comments
        while (getc(pgm_file) != '\n');
    /*
    do
    {
        fgets(buffer, MAX_LINE_LENGTH, f);
    } while (buffer[0] == '#');
    */
    fseek(pgm_file, -1, SEEK_CUR);

    fscanf(pgm_file, "%d %d", &width, &height);
    //fscanf(pgm_file, "%d", &max_val);
    printf("%d  %d\n", width, height);

    picture = calloc(height, sizeof(int *));
    for(int i = 0; i < height; i++){
        picture[i] = calloc(width, sizeof(int));
    }
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            fscanf(pgm_file, "%d", &value);
            picture[row][col] = value;
        }
    }

    fclose(pgm_file);
}

/*
    get_dimensions(buffer);
    // skip M
    fgets(buffer, MAX_LINE_LENGTH, f);

    picture = calloc(height, sizeof(int *));
    for (int i = 0; i < height; i++)
        image[i] = calloc(width, sizeof(int));

    char *line = NULL;
    size_t len = 0;
    for (int r = 0; r < height; r++)
    {
        getline(&line, &len, f);
        char *i;
        char *rest = line;
        for (int col = 0; col < width; col++)
        {
            i = strtok_r(rest, " \t\r\n", &rest);
            image[r][col] = atoi(i);
        }
        //load_row_to_array(line, r);
    }
    fclose(f);
}*/

void *sign(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = 255 / threads;
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
    int size = width / threads;
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

    for (int col = index; col < width; col += threads)
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
void save_histogram(char *output_file)
{
    FILE *f = fopen(output_file, "w");

    for (int i = 0; i < 256; i++)
    {
        int sum = 0;
        for (int j = 0; j < threads; j++)
        {
            sum += histogram[j][i];
        }
        fprintf(f, "%d -> %d\n", i, sum);
    }
    fclose(f);
}
int main(int argc, char *argv[])
{

    threads = atoi(argv[1]);
    char *mode = argv[2];
    char *input_file = argv[3];
    char *output_file = argv[4];
    char buffer[256];
    load_image_to_array(input_file);

    FILE *txt = fopen("Times.txt", "a");
    fprintf(txt, "Mode: %s | threads: %d\n", mode, threads);

    histogram = calloc(threads, sizeof(int *));
    for (int i = 0; i < threads; i++)
        histogram[i] = calloc(256, sizeof(int));

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t *thread_ids = calloc(threads, sizeof(pthread_t));
    struct arg_struct *args = calloc(threads, sizeof(struct arg_struct));
    for (int i = 0; i < threads; i++)
    {
        struct arg_struct arg;
        arg.arg1 = i;
        arg.mode = mode;

        args[i] = arg;

        pthread_create(&thread_ids[i], NULL, write_to_histogram, (void *)&args[i]);
    }

    for (int i = 0; i < threads; i++)
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
    for (int i = 0; i < threads; i++)
        free(histogram[i]);
    free(histogram);
    fclose(txt);
    return 0;
}