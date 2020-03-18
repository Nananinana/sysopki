#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "library.h"

int main()
{
    define_file_pairs("txt/a.txt:txt/b.txt txt/c.txt:txt/d.txt txt/a.txt:txt/c.txt");
    create_main_table(10);
    compare_pairs();
    create_block_of_operations();

    printf("%s", Table->blocks[0]->operations[2]->content);
    delete_block_of_operations(0);
    printf("%s", Table->blocks[2]->operations[0]->content);
    compare_pairs();
    create_block_of_operations();
    delete_operation(0, 0);
    delete_operation(0, 0);
    delete_operation(0, 23);
    delete_operation(0, 3);
    create_block_of_operations();

    compare_pairs();
    printf("%d\n", get_number_of_operations_in_block(0));

//     Operations *ope = get_block(0);
//     for (int i = 0; i < ope->count; i++)
//     {
//         printf("%d: \n%s\n\n", i, ope->operations[i]->content);
//     }

//     // printf("\n\n\n\n\n%s\n\n", get_operation(0, 1));
    delete_main_table();
 }