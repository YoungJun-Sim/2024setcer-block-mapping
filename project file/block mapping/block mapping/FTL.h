#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<string.h>
#include"FlashMemory.h"

typedef struct {
	int read_count;
	int write_count;
	int erase_count;

}counting;


void make_table(unsigned char* table);
void FTL_is_empty_spare_block(unsigned char* table, char* str, char* spare, counting* count);
void Print_table(unsigned char* table);
void FTL_read(unsigned short index, unsigned char* table, char* str, char* sector, char* spare, counting* count);
void FTL_write(unsigned short index, unsigned char* table, char* str, char* sector, char* spare, counting* count);
void FTL_overwrite(unsigned short index, unsigned char* table, char* str, char* sector, char* spare, counting* count);
void FTL_save(unsigned char index, unsigned char* table, char* str, char* sector, char* spare, counting* count);
unsigned short FTL_index_to_short(char* str);
void FTL_index_to_str(unsigned short index, char* str);
