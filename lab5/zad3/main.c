#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void process_file(char* file_path, int producer) {
    if (fork() == 0) {
        if (producer) execlp("./producer", "./producer", "pipe", file_path, "5", NULL);
        else execlp("./consumer", "./consumer", "pipe", file_path, "5", NULL);
    }
}

int main() {

    mkfifo("pipe", S_IRUSR | S_IWUSR);
    process_file("input1.txt", 1);
    process_file("input2.txt", 1);
    process_file("input3.txt", 1);
    process_file("input4.txt", 1);
    process_file("input5.txt", 1);
    process_file("output.txt", 0);
    while((wait(NULL))!=-1);
}