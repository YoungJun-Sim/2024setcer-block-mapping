#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define MAX 529
#define PLUS 640   // 추가 섹터의 양
#define PLUSB 20
#define SECTOR 6144
#define SPARESECTORONE 6144
#define SPARESECTORTWO 6464
#define SPAREBLOCKONE 192
#define SPAREBLOCKTWO 202
#define ALLSECTOR 6784
#include<stdio.h>
#include<string.h>

//3,145,728 B + 98,304 B = 3,244,032 B
//3,244,032B / 528B = 6,144
//spare block = 5
//6144 + 32*5 = 6,304  << 2B
// 6784 /32 = 212 << 1B 
// 6,304/32 = 192 << 1B


void init();
void Flash_read(unsigned short index, char* str);
void Flash_write(unsigned short index, char* str, char* spare);
void Flash_erase(unsigned short block_index);