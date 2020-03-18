#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H
#pragma once
#include <stdio.h>
#include <stdlib.h>

//operacje edycyjne (tablica 3)
typedef struct
{
    char *content;
} Operation;

//blok operacji edycyjnych (tablica 2)
typedef struct
{
    int count;
    Operation **operations;
} Operations;

//handler na tablice głowna (tablice 1)
typedef struct
{
    int size;
    Operations **block_array;
} Diff_handler;

int create_table(unsigned int size);   //utworzenie tablicy glownej
void define_pair_sequence(char *files);  //definiowanie sekwencji par porownywanych plików
void compare_pairs();                  //wykonanie porownania (par plikow) i zapisanie wyniku porównania w pliku tymczasowym
int get_operations_count(int idx);     //zwraca informacje o ilosci operacji w danym bloku edycyjnym
int tmp_to_array();                   //utworzenie bloku operacji edycyjnych(wkaźników na operacje edycyjne) na podstawie pliku tymczasowego, ustawienie w tablicy glownej wskaznika na ten blok; zwraca indeks tablicy glownej, pod ktorym znajduje sie wskaznik na ten blok
void delete_block(int idx);           //usuniecie bloku operacji edycyjnych o zadanym indeksie
void delete_array();                  //?usuniecie calej tablicy glownej?
void remove_operation(int block_idx, int operation_idx); //usuniecie okreslonej operacji dla podanego bloku operacji edycyjnych (o indeksach)
char *get_operation(int block_idx, int operation_idx);   //?wyciagniecie operacji po indeksach

#endif //LAB1_LIBRARY_H