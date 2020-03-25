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

void show_file_status(char *path, struct stat *file_status)
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
        strcpy(new_path, root);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);
        if (S_ISDIR(file_status.st_mode))
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
                continue;
            maxdepth(new_path, depth - 1);
        }
        show_file_status(new_path, &file_status);
    }
    closedir(dir);
}

void mtime(char *root, char mode, int number_of_days, time_t current_date)
{
    //if (count == 0) return;
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
        if (S_ISDIR(file_status.st_mode))
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
                continue;
            mtime(new_path, mode, number_of_days, current_date);
        }
        time_t modification_time = file_status.st_mtime;
        int days_from_modification = abs(difftime(modification_time, current_date)) / (60 * 60 * 24);
        if (mode == '-' && days_from_modification < number_of_days)
            show_file_status(new_path, &file_status);
        else if (mode == '+' && days_from_modification > number_of_days)
            show_file_status(new_path, &file_status);
        else if (mode == '=' && days_from_modification == number_of_days)
                show_file_status(new_path, &file_status);
    }
    closedir(dir);
}

void atime(char *root, char mode, int number_of_days, time_t current_date)
{
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

        if (S_ISDIR(file_status.st_mode))
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            {
                continue;
            }
            atime(new_path, mode, number_of_days, current_date);
        }
        time_t access_time = file_status.st_atime;
        int days_from_access = abs(difftime(access_time, current_date)) / (60 * 60 * 24);
        if (mode == '-' && days_from_access < number_of_days)
            show_file_status(new_path, &file_status);
        else if (mode == '+' && days_from_access > number_of_days)
            show_file_status(new_path, &file_status);
        else if (mode == '=' && days_from_access == number_of_days)
                show_file_status(new_path, &file_status);
    }
    closedir(dir);
}

/*void print_files_handler(char *root, char *command, char mode, int count, int maxdep)
{

    if (strcmp(command, "maxdepth") == 0)
    {
        maxdepth(root, count);
    }
    else if (strcmp(command, "mtime") == 0)
    {
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_mday -= count;
        if (mode == '+')
        {
            timeinfo->tm_mday -= 1;
        }

        if (maxdep >= 0)
        {
            mtime(root, mode, maxdep, mktime(timeinfo));
        }
        else
        {
            mtime(root, mode, -1, mktime(timeinfo));
        }
    }
    else if (strcmp(command, "atime") == 0)
    {
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_mday -= count;
        if (mode == '+')
        {
            timeinfo->tm_mday -= 1;
        }
        if (maxdep >= 0)
        {
            atime(root, mode, maxdep, mktime(timeinfo));
        }
        else
        {
            atime(root, mode, -1, mktime(timeinfo));
        }
    }
}
*/

int main(int argc, char *argv[])
{
    /*if (argc != 6)
    {
        fprintf(stderr, "wrong arguments\n");
        exit(-1);
    }*/
    char *dir = argv[1];
    char *command = argv[2];
    printf(command);
    if (strcmp(command, "maxdepth"))
        maxdepth(dir, atoi(argv[3]));
    if (strcmp(command, "mtime"))
    {
        time_t rawtime;
        struct tm *timeinfo;
        time (&rawtime);
        timeinfo = localtime(&rawtime);
        char *mode = argv[3];
        printf(mode[0]);
        printf(" doing mtime ");
        mtime(dir, mode[0], atoi(argv[4]), mktime(timeinfo));
    }
    if (strcmp(command, "atime"))
    {
        time_t rawtime;
        struct tm *timeinfo;
        time (&rawtime);
        timeinfo = localtime(&rawtime);
        char *mode = argv[3];
        atime(dir, mode[0], atoi(argv[4]), mktime(timeinfo));
    }
    return 0;
}