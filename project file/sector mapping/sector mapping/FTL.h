//202120204 Ω…øµ¡ÿ
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"FlashMemory.h"

typedef struct {
	int read_count;
	int write_count;
	int erase_count;

}counting;

void Make_table(unsigned short* table);
void Print_table(unsigned short* table);

void FTL_read(unsigned short index, char* str, char* sector, char* spare, counting* count);
void FTL_wirte(unsigned short* table, unsigned short index, char* str, char* sector, char* spare, unsigned short* spare_index, counting* count);
void FTL_search_sapre_index(unsigned short* table, unsigned short* sparesector_index, char* str, unsigned short* spare_index, counting* count);
unsigned short FTL_save_original_info(unsigned short* table, unsigned short start_index, char* str, char* sector, char* spare, counting* count);
unsigned short FTL_index_to_short(char* str);
void FTL_index_to_char(unsigned short index, char* str);