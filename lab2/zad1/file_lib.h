#ifndef FILE_LIB_H
#define FILE_LIB_H

void generate(char *filename, int records_number, int record_size);

void lib_copy(char *file1, char *file2, int records_number, int record_size);

void sys_copy(char *file1, char *file2, int records_number, int record_size);

void sys_sort(char *filename, int records_number, int record_size);

void lib_sort(char *filename, int records_number, int record_size);

#endif