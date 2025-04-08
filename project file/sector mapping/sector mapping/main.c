//202120204 �ɿ���
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
	char buffer[MAX] = { 0, };  //�� ���� ������ ����
	char sector[513] = { 0, };  //������ ���� ������ ����
	char spare[17] = { 0, };   //������ ���� ���� ������ ����
	unsigned short index = 0;  //�����ּ�
	unsigned short table[SECTOR-1] = { 0, }; //TABLE�� ���� ���� (2B * 6144)
	unsigned short sparesector_index = 6144; //���м����ּ�

	//����� ���� ������ Ƚ���� �������� ����
	counting* count = malloc(sizeof(count));
	//0���� �ʱ�ȭ
	count->read_count = 0;
	count->write_count = 0;
	count->erase_count = 0;
	//�ð��� �����ϱ� ���� ����
	LARGE_INTEGER micro_start, micro_end, micro_timer;
	double timer;
	init();
	Make_table(table);
	

	FTL_search_sapre_index(table, &sparesector_index, buffer, &sparesector_index, count);
	printf("==============================================\n");
	while (1) {
		printf("���� ���� ���α׷��Դϴ�.\n");
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
				FTL_read(table[index], buffer, sector, spare, count);

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


			FTL_wirte(table, index, buffer, sector, spare, &sparesector_index, count);
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
			if (sparesector_index < SPARESECTORTWO) {
				QueryPerformanceFrequency(&micro_timer);
				QueryPerformanceCounter(&micro_start);
				sparesector_index = FTL_save_original_info(table, SPARESECTORTWO, buffer, sector, spare, count);
				QueryPerformanceCounter(&micro_end);
				timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;


				printf("save(overwrite) �ɸ��ð� : %lf \n", timer);
			}
			else {
				QueryPerformanceFrequency(&micro_timer);
				QueryPerformanceCounter(&micro_start);
				sparesector_index = FTL_save_original_info(table, SPARESECTORONE, buffer, sector, spare, count);
				QueryPerformanceCounter(&micro_end);
				timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;


				printf("save(overwrite) �ɸ��ð� : %lf \n", timer);
			}
			printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
			printf("==============================================\n");
		}
		else if (strcmp(sector, "exit") == 0) {
			printf("������ �����Ͽ����ϴ�.\n");
			if (sparesector_index < SPARESECTORTWO) {
				QueryPerformanceFrequency(&micro_timer);
				QueryPerformanceCounter(&micro_start);
				sparesector_index = FTL_save_original_info(table, SPARESECTORTWO, buffer, sector, spare, count);
				QueryPerformanceCounter(&micro_end);
				timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;


				printf("save(overwrite) �ɸ��ð� : %lf \n", timer);
			}
			else {
				QueryPerformanceFrequency(&micro_timer);
				QueryPerformanceCounter(&micro_start);
				sparesector_index = FTL_save_original_info(table, SPARESECTORONE, buffer, sector, spare, count);
				QueryPerformanceCounter(&micro_end);
				timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;


				printf("save(overwrite) �ɸ��ð� : %lf \n", timer);
			}
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
				FTL_wirte(table, i, buffer, sector, spare, &sparesector_index, count);
			}
			for (int i = 0; i < SECTOR; i++) {
				FTL_wirte(table, i, buffer, sector, spare, &sparesector_index, count);
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
				printf("%dȸ ����\n ", i / 32);
				FTL_wirte(table, i, buffer, sector, spare, &sparesector_index, count);
				
			}
			for (int i = 0; i < SECTOR; i += 32) {
				printf("%dȸ ����\n ", i / 32);
				FTL_wirte(table, i, buffer, sector, spare, &sparesector_index, count);
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
				FTL_wirte(table, i, buffer, sector, spare, &sparesector_index, count);
			}
			for (int i = 0; i < 32; i++) {
				FTL_wirte(table, i, buffer, sector, spare, &sparesector_index, count);
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
				FTL_wirte(table, rand()%6144, buffer, sector, spare, &sparesector_index, count);
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

					FTL_wirte(table, 0, buffer, sector, spare, &sparesector_index, count);
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