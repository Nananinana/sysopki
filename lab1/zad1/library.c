#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "library.h"

char *files_sequence = NULL;  //sekwencja par plikow, ktore beda porownywane
main_table *Table;   //tablica glowna
char *tmp_file = "tmp.txt";    //tymczasowy plik na wyniki operacji

void create_main_table(int size)
{
    Table = calloc(1, sizeof(Table));  //alokujemy tablice glowna
    Table->size = size; 
    Table->blocks = calloc(size, sizeof(block_of_operations *)); //ustawiamy wskaznik na blok operacji edycyjnych, ktory juz jest odpowiedniego rozmiaru
}

int get_file_size(FILE *file_pointer)  //znajduje rozmiar pliku, nie zdefiniowane w pliku naglowkowym bo nie jest potrzebne "na zewnatrz"
{
    fseek(file_pointer, 0L, SEEK_END);  //ustawia pointer na koniec pliku (ustawia na seek_end i stamtąd przesuwa o 0)
    int file_size = ftell(file_pointer);      //zwraca aktualna pozycje pointera
    rewind(file_pointer);    //to samo co fseek(fp, 0,0)
    return file_size;
}

void define_file_pairs(char *files)
{
    files_sequence = files;          //ustawia wskaznik (globalne files_sequence) na podane pliki
}

void compare_files(char *file1, char *file2)  //porownuje pliki i zwraca plik tymczasowy
{
    char buff[256];//?
    snprintf(buff, sizeof buff, "diff %s %s > %s", file1, file2, tmp_file);  //snprintf zapisuje stringa do bufora, w formacie diff plik jeden i plik2 > plik wynikowy, s to po prostu lancuch znakow
    system(buff);   //powoduje wywolanie w powloce (linuxowej) odpowiedniego polecenia, w tym przypadku polecenia porownania i zapisania do pliku wynikowego
}

void load_buffer_into_block(char *buffer, editing_operation ***tab, int number_of_operations) 
{
    char *tmp_array[number_of_operations]; //tablica wskaznikow podanej wielkosci
    char *buffer_pointer = buffer; //wskaznik na bufor ktory dostalismy
    int index = 0;
    tmp_array[index] = buffer_pointer; //ustawiamy pierwszy wskazik z tablicy (stosu) na pierwszy element bufora(?)
 
    while (*buffer_pointer != '\0') //znak null
    {
        if (*buffer_pointer == '\n' && (*(++buffer_pointer) > '0' && *(buffer_pointer) < '9'))  //n-przejscie do nastepnej linii, ???
        {
            index++;
            tmp_array[index] = buffer_pointer;   //to przesuwamy sie do nastepego elementu tablicy
        }
        buffer_pointer++;  //a jesli nie, to tylko przesuwamy sie po buforze
    }
//czyli elementami tablicy są wskaźniki na miejsca w buforze, gdzie zaczynaja sie kolejne operacje
    for (int j = 0; j < number_of_operations; j++)
    {
        if (j == number_of_operations - 1)
        {
            (*tab)[j]->content = calloc(strlen(tmp_array[j]) + 1, sizeof(char));
            memcpy((*tab)[j]->content, tmp_array[j], strlen(tmp_array[j]));
            return;
        }
        (*tab)[j]->content = calloc(strlen(tmp_array[j]) - strlen(tmp_array[j + 1]) + 1, sizeof(char));
        memcpy((*tab)[j]->content, tmp_array[j], strlen(tmp_array[j]) - strlen(tmp_array[j + 1]));
    }
}

int load_file(block_of_operations **new_block)
{
            char *buffer = NULL;
            FILE *file_to_convert = fopen(tmp_file, "rb");
            if (!file_to_convert)
            {
                fprintf(stderr, "Can't read the file");
                return -1;
            }
            int file_size = get_file_size(file_to_convert);
            buffer = calloc(file_size, sizeof(char));
            fread(buffer, sizeof(char), file_size, file_to_convert);
            fclose(file_to_convert);

            //sprawdza liczbe operacji w buforze
            char *buffer_pointer = buffer;
            int number_of_operations = 0;
            bool buffer_not_empty = 0;
            while (*buffer_pointer != '\0')
            {
                if (*buffer_pointer == '\n' && (*(++buffer_pointer) > '0' && *(buffer_pointer) < '9'))
                    number_of_operations++;
                buffer_pointer++;
                buffer_not_empty = 1;
            }
            if (buffer_not_empty)
                number_of_operations++;
            
            (*new_block) = calloc(1, sizeof(block_of_operations));
            (*new_block)-> size = number_of_operations;
            (*new_block)->operations = calloc(number_of_operations, sizeof(editing_operation *));
            for (int i = 0; i < number_of_operations; i++)
            {
                (*new_block)->operations[i] = calloc(1, sizeof(editing_operation));
            }

            load_buffer_into_block(buffer, &((*new_block)->operations), number_of_operations);
            return 0;
}


int create_block_of_operations()  //tworzy blok operacji edycyjnych na podstawie pliku tymczasowgo, ustawia wskazniki, zwraca indeks elementu z tablicy glownej ktory wskazuje na stworzony blok
{
    for (int index = 0; index < Table->size; index++)
    {
        if (Table->blocks[index] == NULL) //jesli dany indeks w tablicy glownej jest pusty, to zaczyna tworzenie bloku edycyjnego
        {
            int load_status = load_file(&Table->blocks[index]);
            if (load_status < 0)
            {
                fprintf(stderr, "can't load into array\n");
                return load_status;
            }  
            return index;
        }
    }
    fprintf(stderr, "main table is already full\n");
    return -1;
}






void compare_pair(char *pair)
{
    char *string = strdup(pair);

    char *f1 = strtok(string, ":");
    char *f2 = strtok(NULL, "");

    compare_files(f1, f2);
    create_block_of_operations();
}

void compare_pairs()
{

    char *string = strdup(files_sequence);
    char *token = strtok(string, " ");
    int count = 0;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        count++;
    }

    char *pairs[count];
    int i = 0;
    char *string_2 = strdup(files_sequence);

    char *token_2 = strtok(string_2, " ");
    while (token_2 != NULL)
    {
        pairs[i] = token_2;
        i++;
        token_2 = strtok(NULL, " ");
    }

    for (int j = 0; j < count; j++)
    {
        compare_pair(pairs[j]);
    }
}

int get_number_of_operations_in_block(int index)
{
    if (Table->blocks[index] == NULL)
        return 0;
    return Table->blocks[index]->size;
}

void delete_block_of_operations(int index)
{
    if (Table->blocks[index] == NULL)
        return;
    for (int i = 0; i < Table->blocks[index]->size; i++)
    {
        free(Table->blocks[index]->operations[i]);
        Table->blocks[index]->operations[i] = NULL;
    }
    free(Table->blocks[index]->operations);
    Table->blocks[index]->operations = NULL;
    free(Table->blocks[index]);
    Table->blocks[index] = NULL;
}

void delete_operation(int block_index, int operation_index)
{
    if (Table->blocks[block_index] == NULL)
    {
        fprintf(stderr, "there is no such block of operations\n");
        return;     }
    if (Table->blocks[block_index]->operations[operation_index] == NULL)
    {
        fprintf(stderr, "there is no such operation\n");
        return;     }
    free(Table->blocks[block_index]->operations[operation_index]);
    Table->blocks[block_index]->operations[operation_index] = NULL;
    int block_size = Table->blocks[block_index]->size;
    if (block_size < 1)
        delete_block_of_operations(block_index);
    else
        Table->blocks[block_index]-> size = block_size - 1;
}

/*Operations *get_block(int idx)
{
    if (main_table->blocks[idx] == NULL)
    {
        fprintf(stderr, "index doesn't exist\n");
    }

    return main_table->blocks[idx];
}*/

/*
char *get_operation(int block_idx, int operation_idx)
{
    if (main_table->blocks[block_idx] == NULL)
    {
        fprintf(stderr, "index in array doesn't exist\n");
    }
    if (main_table->blocks[block_idx]->operations[operation_idx] == NULL)
    {
        fprintf(stderr, "index in operation array doesn't exist\n");
    }
    return main_table->blocks[block_idx]->operations[operation_idx]->content;
}
*/

void delete_main_table()
{
    for (int i = 0; i < Table->size; i++)
    {
        delete_block_of_operations(i);
    }
    free(Table->blocks);
    free(Table);
}

/*int main()
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
 }*/