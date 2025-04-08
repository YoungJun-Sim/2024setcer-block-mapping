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
		printf("블럭 맵핑 프로그램입니다.\n");
		printf("TABLE           ...  매핑 테이블 출력(섹터)\n");
		printf("read LSN        ...  해당 LSN의 데이터 읽기\n");
		printf("write LSN DATA  ...  해당 LSN에 데이터 쓰기\n");
		printf("save            ...  여분 섹터 초기화\n");
		printf("exit            ...  프로그램 종료\n\n");
		printf("test1           ...  모든 블록에 한번씩 저장 두번하기\n");
		printf("test2           ...  모든 섹터에 한번씩 저장 두번하기\n");
		printf("test3           ...  하나의 블록 전체에 저장 두번하기\n");
		printf("test4           ...  랜던주소에 5000번 저장하기\n");
		printf("test5           ...  한 섹터에 1000번 저장 하기\n");
		printf("명령어 입력 : ");
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
				printf("초과된 주소입니다.\n");
			}
			else {
				QueryPerformanceFrequency(&micro_timer);
				QueryPerformanceCounter(&micro_start);
				FTL_read(index, table, buffer, sector, spare, count);

				QueryPerformanceCounter(&micro_end);
				timer = (micro_end.QuadPart - micro_start.QuadPart) / (double)micro_timer.QuadPart;
				printf("FTL_read 걸린시간 : %lf \n", timer);
			}
			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
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
			printf("FTL_write 걸린시간 : %lf \n", timer);
			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
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
			printf("save 걸린시간 : %lf \n", timer);
			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
			printf("==============================================\n");
		}

		else if (strcmp(sector, "exit") == 0) {
			printf("파일을 저장하였습니다.\n");
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
			printf("exit(save) 걸린시간 : %lf \n", timer);
			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
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
			printf("6144회 작성*2(12288) 걸린시간 : %lf \n", timer);

			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
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
			printf("384회 작성(192 +192) 걸린시간 : %lf \n", timer);

			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
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
			printf("한 블록 32회*2 걸린시간 : %lf \n", timer);



			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
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
			printf("랜덤주소값 (5000회) 걸린시간 : %lf \n", timer);


			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
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
			printf("한 섹터 저장 1000회 걸린시간 : %lf \n", timer);

			printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
		}
		else {
			printf("잘못된 입력입니다.\n");
		}


	}


	free(count);
	return 0;
}