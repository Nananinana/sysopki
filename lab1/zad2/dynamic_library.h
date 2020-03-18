#ifndef DYNAMIC_LIBRARY_H
#define DYNAMIC_LIBRARY_H
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static void *handle = NULL;

//char *(*_get_operation)(int block_block_index, int operation_block_index);
int (*_create_main_table)(int size);
void (*_define_file_pairs)(char *files);
void (*_compare_files)();
int (*_get_number_of_operations_in_block)(int block_index);
int (*_create_block_of_operations)();
void (*_delete_block_of_operations)(int block_index);
void (*_delete_main_table)();
void (*_delete_operation)(int block_index, int operation_index);

void init()
{
    library_handler = dlopen("library.so", RTLD_NOW);
    if (library_handler == NULL)
    {
        fprintf(stderr, "Can't open dynamic library");
        return;
    }
    //_get_operation = dlsym(handle, "get_operation");
    _create_main_table = dlsym(library_handler, "create_main_table");
    _define_file_pairs = dlsym(library_handler, "define_file_pairs");
    _compare_pairs = dlsym(library_handler, "compare_pairs");
    _get_number_of_operations_in_block = dlsym(library_handler, "get_number_of_operations_in_block");
    _create_block_of_operations = dlsym(library_handler, "create_block_of_operations");
    _delete_block_of_operations = dlsym(library_handler, "delete_block_of_operations");
    _delete_main_table = dlsym(library_handler, "delete_main_table");
    _delete_operation = dlsym(library_handler, "delete_operation");

    char *error;
    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
}

int create_main_table(int size)
{
    return (*_create_main_table)(size);
}
void define_file_pairs(char *files)
{
    return (*_define_file_pairs)(files);
}
void compare_pairs()
{
    return (*_compare_pairs)();
}
int get_number_of_operations_in_block(int block_index)
{
    return (*_get_number_of_operations_in_block)(block_index);
}
int create_block_of_operations()
{
    return (*_create_block_of_operations)();
}
void delete_block_of_operations(int block_index)
{
    return (*_delete_block_of_operations)(block_index);
}
void delete_main_table()
{
    return (*_delete_main_table)();
}
void delete_operation(int block_index, int operation_index)
{
    return (*_delete_operation)(block_index, operation_index);
}
/*char *get_operation(int block_index, int operation_index)
{
    return (*_get_operation)(block_block_index, operation_block_index);
}*/

#endif