
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

const char format[] = "%Y-%m-%d %H:%M:%S";
char follow_mode;
char *command;
time_t follow_date;
int maxdepth;

void date(time_t time, char *buffer){
    struct tm *times = localtime(&time);
    strftime(buffer, 255*sizeof(char), "%c", times);
}

void show_file_status(const char *path, const struct stat *file_status)
{
    char *type = "undefined";

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

int file_info(const char *filename, const struct stat *file_status,
              int fileflags, struct FTW *pfwt)
{

    if (maxdepth != -1 && pfwt->level > maxdepth)
    {

        return 0;
    }
    if (strcmp(command, "maxdepth") == 0)
    {
        show_file_status(filename, file_status);

        return 0;
    }
    else if (strcmp(command, "mtime") == 0)
    {

        time_t modif_time = file_status->st_mtime;

        if (follow_mode == '-')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;
            ;

            show_file_status(filename, file_status);
        }
        else if (follow_mode == '+')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;

            show_file_status(filename, file_status);
        }
        else if (follow_mode == '=')
        {

            int diff_modif = abs(difftime(modif_time, follow_date)) / (60 * 60 * 24);
            int diff_modif2 = difftime(follow_date, modif_time);

            if ((diff_modif == 0 && follow_mode == '=') && !(diff_modif2 < 0 && follow_mode == '='))
                show_file_status(filename, file_status);
            return 0;
        }
    }
    else if (strcmp(command, "atime") == 0)
    {
        time_t modif_time = file_status->st_atime;

        if (follow_mode == '-')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;
            ;

            show_file_status(filename, file_status);
        }
        else if (follow_mode == '+')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;

            show_file_status(filename, file_status);
        }
        else if (follow_mode == '=')
        {

            int diff_modif = abs(difftime(modif_time, follow_date)) / (60 * 60 * 24);
            int diff_modif2 = difftime(follow_date, modif_time);

            if ((diff_modif == 0 && follow_mode == '=') && !(diff_modif2 < 0 && follow_mode == '='))
                show_file_status(filename, file_status);
            return 0;
        }
    }
    return 0;
}

void print_files_handler(char *root_path, char *comm, char mode, int count, int maxdep)
{

    int flags = FTW_PHYS;
    int fd_limit = 2;
    follow_mode = mode;
    command = comm;
    maxdepth = maxdep;

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_mday -= count;
    if (mode == '+')
    {
        timeinfo->tm_mday -= 1;
    }
    follow_date = mktime(timeinfo);

    nftw(root_path,
         file_info,
         fd_limit, flags);
}

int main(int argc, char *argv[])
{

    if (argc != 6)
    {
        fprintf(stderr, "wrong arguments\n");
        exit(-1);
    }
    char *dir = argv[1];
    char *comman = argv[2];
    char *mod = argv[3];
    int day = atoi(argv[4]);
    int mdep = atoi(argv[5]);

    print_files_handler(dir, comman, mod[0], day, mdep);

    return 0;
}