#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

void deep_search(char *root)
{
    if (root == NULL) return;
    DIR *dir = opendir(root);
    if (dir == NULL)
    {
        printf("error while opening directory");
        return;
    }
    struct dirent *file;
    while ((file = readdir(dir)) != NULL) 
    {
        char *new_path = malloc(sizeof(char) * (strlen(root) + strlen(file->d_name)) + 2);
        struct stat file_status;
        strcpy(new_path, root);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);
        if (lstat(new_path, &file_status) < 0)
        {
            printf("unable to lstat file %s\n", new_path);
            return;
        }
        if (S_ISDIR(file_status.st_mode) && fork()==0)
        {
            printf("\npid(%i) path(%s)\n", getpid(), new_path);
            static const char cmd[] = "ls -l %s";
            char *ls = calloc(sizeof(cmd), sizeof(char));
            sprintf(ls, cmd, new_path);
            system(ls);
            free(ls);
            exit(0);
        }
        else
        {
            wait(NULL);
        }
        free(new_path);
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    char *root;
    if (argc>1)
        root = argv[1];
    else
        root = ".";
    deep_search(root);
    return 0;
}