#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAX_COMMANDS_IN_LINE 10
#define MAX_ARGS 10

char *load_file_to_buffer(FILE *file)
{
    if(file == NULL){
        printf("can't open file");
        exit(1); 
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(file_size + 1);
    if (fread(buffer, 1, file_size, file) != file_size)
    {
        printf("can't read from file \n");
        exit(1);
    }
    fclose(file);
    return buffer;
}

int main (int argc, char ** argv){

    if(argc <2){
        printf("Wrong number of arguments");
        return 1;
    }

    char *file_path = argv[1];
    FILE *file = fopen(file_path, "r");
    char *buffer = load_file_to_buffer(file);
    char *buffer_copy = buffer;
    char *line = strtok_r(buffer, "\n", &buffer_copy);
    int tasks_number;
    
    while(line != NULL){
        tasks_number = 0;
        char *tasks[MAX_COMMANDS_IN_LINE][MAX_ARGS];
        for (int i = 0; i < MAX_COMMANDS_IN_LINE; ++i)
        {
            for (int j = 0; j < MAX_ARGS; ++j)
                tasks[i][j] = NULL;
        }   
        char *line_copy = line;
        char *one_task = strtok_r(line, "|", &line_copy);
        tasks_number = 0;

        while (one_task != NULL)
        {
            char *task_copy = one_task;
            char *one_argument = strtok_r(one_task, " ", &task_copy);
            int arguments_number = 0;
            while (one_argument != NULL)
            {
                printf(one_argument);
                tasks[tasks_number][arguments_number++] = one_argument;
                one_argument = strtok_r(NULL, " ", &task_copy);
            }
            one_task = strtok_r(NULL, "|", &line_copy);
            tasks_number++;
        }
        int fd1[2], fd2[2];
        pipe(fd1);

        for(int i=0;i<tasks_number;i++){
            pipe(fd2);
            if (fork()== 0) {
                if(i!=0){
                    dup2(fd1[0],STDIN_FILENO);
                    close(fd1[1]);
                }
                if(i!=tasks_number-1){
                    close(fd2[0]);
                    dup2(fd2[1],STDOUT_FILENO);
                }
                    execvp(tasks[i][0],tasks[i]);
                    perror("error");
                    for(int j=0;j<MAX_ARGS;j++)
                        free(tasks[i][j]);
                    free(tasks[i]);
                    exit(0);
            } 
            close(fd1[0]);
            close(fd1[1]);
            fd1[0]=fd2[0];
            fd1[1]=fd2[1];
        }
        close(fd2[0]);
        close(fd2[1]);

        while((wait(NULL))!=-1);
        line = strtok_r(NULL, "\n", &buffer_copy);
    }
    free(buffer);
}