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


//int maxs=0; //?max arguments/commands?

/*char **find_args(char *command){
    char **args=(char**)calloc(128, sizeof(char*));

    int p=0, it=0, flag=0;
    for(int i=0;command[i]!='\0';i++){
        if((command[i]=='\''|| command[i]=='\"'|| command[i]=='{')&& flag==0)
            flag=1;
        else if((command[i]=='\''  || command[i]=='\"'|| command[i]=='}')&& flag==1)
            flag=0;
        
        
        if(command[i]==' ' && flag==0){

            args[it]=(char*)calloc(i-p+64, sizeof(char));
            strncpy(args[it++], command+p,i-p);
            p=i+1;
        }
        else if(command[i+1]=='\0'){

            args[it]=(char*)calloc(i-p+64, sizeof(char));
            strncpy(args[it++], command+p,i+1-p+1);
        }

    }
    args[it]=NULL;
    
        maxs=it;

    return args;
}*/


/*void find_arguments(char *task_arguments, char *one_task)
{
    
}*/

/*
int parse_line (char *line, char ***tasks)
{
    char *line_copy = line;
    char *one_task = strtok_r(line, "|", &line_copy);
    int tasks_number = 0;

    while (one_task != NULL)
    {
        //find_arguments(tasks[tasks_number++], one_task);
        char *task_copy = one_task;
        char *one_argument = strtok_r(one_task, " ", &task_copy);
        int arguments_number = 0;
        while (one_argument != NULL)
        {
            tasks[tasks_number][arguments_number++] = one_argument;
            one_argument = strtok_r(NULL, " ", &task_copy);
        }
        one_task = strtok_r(NULL, "|", &line_copy);
    }     
    return tasks_number; 
}
*/


int main (int argc, char ** argv){

    if(argc <2){
        printf("Wrong number of arguments");
        return 1;
    }

    char *file_path = argv[1];
    FILE *file = fopen(file_path, "r");
    if(file == NULL){
        printf("can't open file");
        return 1;
    }
    char line[2048];
    int tasks_number;

    
    while(fgets(line,2048,file)!=NULL){
        tasks_number = 0;
        char *tasks[MAX_COMMANDS_IN_LINE][MAX_ARGS];
        for (int i = 0; i < MAX_COMMANDS_IN_LINE; ++i)
        {
            for (int j = 0; j < MAX_ARGS; ++j)
                tasks[i][j] = NULL;
        }   
        //int tasks_number = parse_line(line, tasks);

        char *line_copy = line;
        char *one_task = strtok_r(line, "|", &line_copy);
        printf (one_task);
        tasks_number = 0;

        while (one_task != NULL)
        {
        //find_arguments(tasks[tasks_number++], one_task);
            char *task_copy = one_task;
            char *one_argument = strtok_r(one_task, " ", &task_copy);
            printf (one_argument);
            int arguments_number = 0;
            while (one_argument != NULL)
            {
                tasks[tasks_number][arguments_number++] = one_argument;
                one_argument = strtok_r(NULL, " ", &task_copy);
            }
            one_task = strtok_r(NULL, "|", &line_copy);
        }
        printf (tasks_number);
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
                    //args=find_args(tasks[i]);
                    execvp(tasks[i][0],tasks[i]);
                    /*perror("error");

                    for(int i=0;i<maxs;i++){
                        free(args[i]);
                    }
                    free(args);*/
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
    }

    fclose(file); 
}