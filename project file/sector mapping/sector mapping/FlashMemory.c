//202120204 심영준
#include "FlashMemory.h"


void init() {
    FILE* pointer = fopen("FlashMemory.txt", "a+");
    fseek(pointer, 0, SEEK_SET);
    if (pointer == NULL) {
        printf("오류 발생\n");
    }
    else if (fgetc(pointer) == EOF) {
        for (int i = 0; i < ALLSECTOR * 528; i++) {
            fputc(' ', pointer);
        }
        printf("파일을 생성했습니다.\n");
        printf("총 바이트 수:%d\n", ftell(pointer));
        printf("총 섹터 수  :%d\n", ftell(pointer) / 528);
        printf("총 블럭 수  :%d\n", ftell(pointer) / 528 / 32);
    }
    else {
        fseek(pointer, 0, SEEK_END);
        if (ftell(pointer) != ALLSECTOR * 528) {
            printf("경고 용량 크기가 다릅니다.\n");
            printf("기존 용량 = %dB, 현재 용량 = %dB\n", ALLSECTOR * 528, ftell(pointer));
        }
        else {
            printf("총 바이트 수:%d\n", ftell(pointer));
            printf("총 섹터 수  :%d\n", ftell(pointer) / 528);
            printf("총 블럭 수  :%d\n", ftell(pointer) / 528 / 32);
        }
    }


    fclose(pointer); //파일 포인터 닫기
}

void Flash_read(unsigned short index, char* str) {
    FILE* pointer = fopen("FlashMemory.txt", "r");
    fseek(pointer, (index * 528), SEEK_SET);
    fgets(str, 528, pointer);
    fclose(pointer);
}

void Flash_write(unsigned short index, char* str, char* spare) {
    FILE* pointer = fopen("FlashMemory.txt", "r+");
    fseek(pointer, (index * 528), SEEK_SET);
    fputs(str, pointer);
    fseek(pointer, (index * 528) + 512, SEEK_SET);
    fputs(spare, pointer);
    fclose(pointer);

}


void Flash_erase(unsigned short block_index) {
    FILE* pointer = fopen("FlashMemory.txt", "r+");
    fseek(pointer, (block_index * 528 * 32), SEEK_SET);
    for (int i = 0; i < 32 * 528; i++) {
        fputc(' ', pointer);
    }

    fclose(pointer);
}