//202120204 �ɿ���
#include "FlashMemory.h"


void init() {
    FILE* pointer = fopen("FlashMemory.txt", "a+");
    fseek(pointer, 0, SEEK_SET);
    if (pointer == NULL) {
        printf("���� �߻�\n");
    }
    else if (fgetc(pointer) == EOF) {
        for (int i = 0; i < ALLSECTOR * 528; i++) {
            fputc(' ', pointer);
        }
        printf("������ �����߽��ϴ�.\n");
        printf("�� ����Ʈ ��:%d\n", ftell(pointer));
        printf("�� ���� ��  :%d\n", ftell(pointer) / 528);
        printf("�� �� ��  :%d\n", ftell(pointer) / 528 / 32);
    }
    else {
        fseek(pointer, 0, SEEK_END);
        if (ftell(pointer) != ALLSECTOR * 528) {
            printf("��� �뷮 ũ�Ⱑ �ٸ��ϴ�.\n");
            printf("���� �뷮 = %dB, ���� �뷮 = %dB\n", ALLSECTOR * 528, ftell(pointer));
        }
        else {
            printf("�� ����Ʈ ��:%d\n", ftell(pointer));
            printf("�� ���� ��  :%d\n", ftell(pointer) / 528);
            printf("�� �� ��  :%d\n", ftell(pointer) / 528 / 32);
        }
    }


    fclose(pointer); //���� ������ �ݱ�
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