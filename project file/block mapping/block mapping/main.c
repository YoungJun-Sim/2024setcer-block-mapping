#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#pragma comment(lib, "winmm.lib")
#include<time.h>
#include"FlashMemory.h"
#include"FTL.h"


int main() {
	char buffer[MAX] = { 0, };
	char sector[513] = { 0, };
	char spare[17] = { 0, };
	unsigned short index = 0;
	unsigned char table[SPAREBLOCKONE] = { 0, };

	counting* count = malloc(sizeof(counting));
	count->read_count = 0;
	count->write_count = 0;
	count->erase_count = 0;

	LARGE_INTEGER micro_start, micro_end, micro_timer;
	double timer;

	make_table(table);
	init();
	FTL_is_empty_spare_block(table, buffer, spare, count);
	printf("==============================================\n");
	while (1) {
		printf("�� ���� ���α׷��Դϴ�.\n");
		printf("TABLE           ...  ���� ���̺� ���(����)\n");
		printf("read LSN        ...  �ش� LSN�� ������ �б�\n");
		printf("write LSN DATA  ...  �ش� LSN�� ������ ����\n");
		printf("save            ...  ���� ���� �ʱ�ȭ\n");
		printf("exit            ...  ���α׷� ����\n\n");
		printf("test1           ...  ��� ��Ͽ� �ѹ��� ���� �ι��ϱ�\n");
		printf("test2           ...  ��� ���Ϳ� �ѹ��� ���� �ι��ϱ�\n");
		printf("test3           ...  �ϳ��� ��� ��ü�� ���� �ι��ϱ�\n");
		printf("test4           ...  �����ּҿ� 5000�� �����ϱ�\n");
		printf("test5           ...  �� ���Ϳ� 1000�� ���� �ϱ�\n");
		printf("��ɾ� �Է� : ");
		scanf("%s", sector);
		system("cls");
		if (strcmp(sector, "TABLE") == 0) {
			Print_table(table);
			printf("==============================================\n");
		}
		else if (strcmp(sector, "read") == 0) {
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			scanf("%hd", &index);
			if (index >= SPARESECTORONE) {
				printf("�ʰ��� �ּ��Դϴ�.\n");
			}
			else {
				QueryPerformanceFrequency(&micro_timer);
				QueryPerformanceCounter(&micro_start);
				FTL_read(index, table, buffer, sector, spare, count);

				QueryPerformanceCounter(&micro_end);
				timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
				printf("FTL_read �ɸ��ð� : %lf \n", timer);
			}
			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
			printf("==============================================\n");
		}
		else if (strcmp(sector, "write") == 0) {
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			scanf("%hd", &index);
			scanf("%s", sector);
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			FTL_write(index, table, buffer, sector, spare, count);

			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("FTL_write �ɸ��ð� : %lf \n", timer);
			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
			printf("==============================================\n");
		}
		else if (strcmp(sector, "save") == 0) {
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			for (int i = 0; i < PLUSB/2; i++) {
				FTL_save(SPAREBLOCKONE + i * 2, table, buffer, sector, spare, count);
			}

			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("save �ɸ��ð� : %lf \n", timer);
			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
			printf("==============================================\n");
		}

		else if (strcmp(sector, "exit") == 0) {
			printf("������ �����Ͽ����ϴ�.\n");
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			for (int i = 0; i < PLUSB/2; i++) {
				FTL_save(SPAREBLOCKONE + i * 2, table, buffer, sector, spare, count);
			}

			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("exit(save) �ɸ��ð� : %lf \n", timer);
			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
			break;
		}
		else if (strcmp(sector, "test1") == 0) {
			strcpy(sector, "teststring1");
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			for (int i = 0; i < SECTOR; i++) {
				FTL_write(i, table, buffer, sector, spare, count);
			}
			for (int i = 0; i < SECTOR; i++) {
				FTL_write(i, table, buffer, sector, spare, count);
			}
			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("6144ȸ �ۼ�*2(12288) �ɸ��ð� : %lf \n", timer);

			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
		}
		else if (strcmp(sector, "test2") == 0) {
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			strcpy(sector, "teststring2");
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			for (int i = 0; i < SECTOR; i += 32) {
				FTL_write(i, table, buffer, sector, spare, count);

			}
			for (int i = 0; i < SECTOR; i += 32) {
				FTL_write(i, table, buffer, sector, spare, count);
			}
			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("384ȸ �ۼ�(192 +192) �ɸ��ð� : %lf \n", timer);

			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
		}
		else if (strcmp(sector, "test3") == 0) {
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			strcpy(sector, "teststring3");
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			for (int i = 0; i < 32; i++) {
				FTL_write(i, table, buffer, sector, spare, count);
			}
			for (int i = 0; i < 32; i++) {
				FTL_write(i, table, buffer, sector, spare, count);
			}


			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("�� ��� 32ȸ*2 �ɸ��ð� : %lf \n", timer);



			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
		}
		else if (strcmp(sector, "test4") == 0) {
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			strcpy(sector, "teststring4");
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			srand((unsigned int)time(NULL));
			for (int i = 0; i < 5000; i++) {
				FTL_write(rand()%6144, table, buffer, sector, spare, count);;
			}

			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("�����ּҰ� (5000ȸ) �ɸ��ð� : %lf \n", timer);


			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
		}
		else if (strcmp(sector, "test5") == 0) {
			count->read_count = 0;
			count->write_count = 0;
			count->erase_count = 0;
			strcpy(sector, "teststring5");
			QueryPerformanceFrequency(&micro_timer);
			QueryPerformanceCounter(&micro_start);
			for (int i = 0; i < 1000; i++) {

				FTL_write(0, table, buffer, sector, spare, count);
			}

			QueryPerformanceCounter(&micro_end);
			timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
			printf("�� ���� ���� 1000ȸ �ɸ��ð� : %lf \n", timer);

			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
		}
		else {
			printf("�߸��� �Է��Դϴ�.\n");
		}


	}


	free(count);
	return 0;
}