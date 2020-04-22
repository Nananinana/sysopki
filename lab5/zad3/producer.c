  
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc != 4)
    {
        printf("Wrong number of arguments, should be: [pipe_path] [file_path] [N]");
        exit(1);
    }
    FILE *pipe = fopen(argv[1], "w+");
    if (pipe == NULL)
    {
        printf("Can't open pipe");
        exit(1);
    }
    FILE *file = fopen(argv[2], "r");
    if (file == NULL)
    {
        printf("Can't open file");
        exit(1);
    }
    int N = atoi(argv[3]);
    char *buffer = calloc(N + 1, sizeof(char));
    while (fread(buffer, 1, N, file) > 0) 
        fwrite(buffer, 1, N, pipe);

    free(buffer);
    fclose(file);
    fclose(pipe);
}