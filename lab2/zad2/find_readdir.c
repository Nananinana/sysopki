#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

void date(time_t time, char *buffer){
    struct tm *times = localtime(&time);
    strftime(buffer, 255*sizeof(char), "%c", times);
}

void show_file_status(const char *path, const struct stat *file_status)
{
    char *type = "uknown";

    if (S_ISREG(file_status->st_mode) != 0)
        type = "file";
    else if (S_ISDIR(file_status->st_mode) != 0)
        type = "dir";
    else if (S_ISFIFO(file_status->st_mode) != 0)
        type = "fifo";
    else if (S_ISSOCK(file_status->st_mode) != 0)
        type = "socket";
    else if (S_ISLNK(file_status->st_mode) != 0)
        type = "slink";
    else if (S_ISCHR(file_status->st_mode) != 0)
        type = "char dev";
    else if (S_ISBLK(file_status->st_mode) != 0)
        type = "block dev";
    
    char mtime[255];
    char atime[255];
    time_t modification_time = file_status->st_mtime;
    time_t access_time = file_status->st_atime;
    date(modification_time, mtime);
    date(access_time, atime);

    printf("path: %s \n links: %ld \n file type: %s \n file size: %ld \n access time %s\n modification time: %s \n\n\n",
           path,
           file_status->st_nlink,
           type,
           file_status->st_size,
           atime,
           mtime
    );
}

void maxdepth(char *root, int depth)
{
    if (depth == 0) return;
    if (root == NULL) return;
    DIR *dir = opendir(root);
    if (dir == NULL)
    {
        fprintf(stderr, "error while opening directory: %s\n", strerror(errno));
        exit(-1);
    }
    struct dirent *file;
    char new_path[256];
    while ((file = readdir(dir)) != NULL) 
    {
        struct stat file_status;
        if (lstat(new_path, &file_status) < 0)
        {
            fprintf(stderr, "unable to lstat file %s: %s\n", new_path, strerror(errno));
            exit(-1);
        }
        strcpy(new_path, root);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);
        if (S_ISDIR(file_status.st_mode))
        {
            //if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            //    continue;
            maxdepth(new_path, depth - 1);
        }
        show_file_status(new_path, &file_status);
    }
    closedir(dir);
}

void mtime(char *root, char mode, struct tm searched_day, int depth)
{
    if (depth == 0) return;
    if (root == NULL) return;
    DIR *dir = opendir(root);
    if (dir == NULL)
    {
        fprintf(stderr, "error while opening directory: %s\n", strerror(errno));
        exit(-1);
    }
    struct dirent *file; 
    char new_path[256];
    while ((file = readdir(dir)) != NULL)
    {
        strcpy(new_path, root);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);
        struct stat file_status;
        if (lstat(new_path, &file_status) < 0)
        {
            fprintf(stderr, "unable to lstat file %s: %s\n", new_path, strerror(errno));
            exit(-1);
        }
        if (S_ISDIR(file_status.st_mode))
        {
            //if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            //    continue;
            mtime(new_path, mode, searched_day, depth-1);
        }
        time_t access_time = file_status.st_atime;
        struct tm access_tm = *localtime(&access_time);
        if (mode == '-' && access_tm.tm_mday > searched_day.tm_mday)
            show_file_status(new_path, &file_status);
        else if (mode == '+' && access_tm.tm_mday < searched_day.tm_mday)
            show_file_status(new_path, &file_status);
        else if (mode == '=' && access_tm.tm_mday < searched_day.tm_mday)
            show_file_status(new_path, &file_status);
    }
    closedir(dir);
}

void atime(char *root, char mode, struct tm searched_day, int depth)
{
    if (depth == 0) return;
    if (root == NULL) return;
    DIR *dir = opendir(root);
    if (dir == NULL)
    {
        fprintf(stderr, "error while opening directory: %s\n", strerror(errno));
        exit(-1);
    }
    struct dirent *file;
    char new_path[256];
    while ((file = readdir(dir)) != NULL)
    {
        strcpy(new_path, root);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);
        struct stat file_status;
        if (lstat(new_path, &file_status) < 0)
        {
            fprintf(stderr, "unable to lstat file %s: %s\n", new_path, strerror(errno));
            exit(-1);
        }
        if (S_ISDIR(file_status.st_mode))
        {
            //if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            //{
            //    continue;
            //}
            atime(new_path, mode, searched_day, depth-1);
        }
        time_t access_time = file_status.st_atime;
        struct tm access_tm = *localtime(&access_time);
        if (mode == '-' && access_tm.tm_mday > searched_day.tm_mday)
            show_file_status(new_path, &file_status);
        else if (mode == '+' && access_tm.tm_mday < searched_day.tm_mday)
            show_file_status(new_path, &file_status);
        else if (mode == '=' && access_tm.tm_mday < searched_day.tm_mday)
            show_file_status(new_path, &file_status);
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    char *path  = argv[1];
    char *command = argv[2];
    int max_depth = -1;

    if (argc == 3)
    {
        if(strcmp(command, "maxdepth") == 0){
        max_depth = atoi(argv[3]);
        maxdepth(path, max_depth);
    }
    else
        printf("Wrong command \n");
    }
    else if (argc == 4)
    {
    printf("4 arguments \n");
    if (strcmp(command, "atime") == 0)
    {
        char mode = argv[3];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        tm.tm_mday -= atoi(argv[4]);
        //currentDate = mktime(&tm);
        atime(path, mode, tm, max_depth);
    }
    else if (strcmp(command, "mtime") == 0)
    {
        char mode = argv[3];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        tm.tm_mday -= atoi(argv[4]);
        //currentDate = mktime(&tm);
        mtime(path, mode, tm, max_depth);
    }
    else 
        printf("Wrong command \n");
    }
    else if (argc == 6)
    {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        //currentDate = mktime(&tm);
        if(strcmp(command, "maxdepth") == 0){
            max_depth = atoi(argv[3]);
            char *command2 = argv[4];
            char mode = argv[5];
            if (strcmp(command2, "atime")==0)
            {
                tm.tm_mday -= atoi(argv[6]);
                atime(path, mode, tm, max_depth);
            }
            else if (strcmp(command2, "mtime")==0))
            {
                tm.tm_mday -= atoi(argv[6]);
                mtime(path, mode, tm, max_depth);
            }
            else 
                printf ("Wrong command \n");
        }
        else if (strcmp(command, "atime"==0))
        {
            char mode = argv[3];
            tm.tm_mday -= atoi(argv[4]);
            char *command2 = argv[5];
            if (strcmp(command, "maxdepth") == 0)
            {
                max_depth = atoi(argv[6]);
                atime(path, mode, tm, max_depth);
            }
            else
                printf ("Wrong command \n");
        }
        else if (strcmp(command, "mtime"==0))
         {
            char mode = argv[3];
            tm.tm_mday -= atoi(argv[4]);
            char *command2 = argv[5];
            if (strcmp(command, "maxdepth") == 0)
            {
                max_depth = atoi(argv[6]);
                mtime(path, mode, tm, max_depth);
            }
            else
                printf ("Wrong command \n");
        }
        else
            printf ("Wrong command \n");
    }
    else 
        printf ("Wrong command \n");
    return 0;
}