#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H
#pragma once
#include <stdio.h>
#include <stdlib.h>

//operacje edycyjne (tablica 3)
typedef struct
{
    char *content;
} editing_operation;

//blok operacji edycyjnych (tablica 2)
typedef struct
{
    int size;
    editing_operation **operations;
} block_of_operations;

//handler na tablice głowna (tablice 1)
typedef struct
{
    int size;
    block_of_operations **blocks;
} main_table;

void create_main_table(int size);   //utworzenie tablicy glownej
void define_file_pairs(char *files);       //definiowanie sekwencji par porownywanych plików
void compare_pairs();                      //wykonanie porownania (par plikow) i zapisanie wyniku porównania w pliku tymczasowym
int create_block_of_operations();           //utworzenie bloku operacji edycyjnych(wkaźników na operacje edycyjne) na podstawie pliku tymczasowego, ustawienie w tablicy glownej wskaznika na ten blok; zwraca indeks tablicy glownej, pod ktorym znajduje sie wskaznik na ten blok
int get_number_of_operations_in_block(int block_index);     //zwraca informacje o ilosci operacji w danym bloku edycyjnym
void delete_block_of_operations(int block_index);           //usuniecie bloku operacji edycyjnych o zadanym indeksie
void delete_main_table();                  //?usuniecie calej tablicy glownej?
void delete_operation(int block_index, int operation_index); //usuniecie okreslonej operacji dla podanego bloku operacji edycyjnych (o indeksach)
//char *get_operation(int block_idx, int operation_idx);   //?wyciagniecie operacji po indeksach

#endif //LAB1_LIBRARY_H