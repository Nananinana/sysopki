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
        fprintf(stderr, "error while opening directory: %s\n", strerror(errno)); //?
        exit(-1);
    }
    struct dirent *file;
    char new_path[256];
    while ((file = readdir(dir)) != NULL) 
    {
        struct stat file_status;
        strcpy(new_path, root);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);
        if (lstat(new_path, &file_status) < 0)
        {
            fprintf(stderr, "unable to lstat file %s: %s\n", new_path, strerror(errno));
            exit(-1);
        }
        if (S_ISDIR(file_status.st_mode) && fork()==0)
        {
            //if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            //    continue;
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

/*
void search_dir(char *path)
{
    if (!path)
        return;
    DIR *directory = opendir(path);
    if (!directory)
        return;
    struct dirent *d;
    struct stat st;

    while ((d = readdir(directory)))
    {
        if (strcmp(d->d_name, "..") == 0)
            continue;
        if (strcmp(d->d_name, ".") == 0)
            continue;
        char *dir = malloc(sizeof(char) * (strlen(path) + strlen(d->d_name)) + 2);
        sprintf(dir, "%s/%s", path, d->d_name);
        if (lstat(dir, &st) < 0)
            continue;
        if (S_ISDIR(st.st_mode) && fork() == 0)
        {
            printf("\npid(%i) path(%s)\n", getpid(), dir);
            static const char cmd[] = "ls -la %s";
            char *ls = calloc(sizeof(cmd), sizeof(char));
            sprintf(ls, cmd, dir);
            system(ls);
            free(ls);
            exit(0);
        }
        else
            wait(NULL);
        free(dir);
    }
    closedir(directory);
}*/

int main(int argc, char **argv)
{
    char *path = argc > 1 ? argv[1] : ".";
    deep_search(path);
    return 0;
}