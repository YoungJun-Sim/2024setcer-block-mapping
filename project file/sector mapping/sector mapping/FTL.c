//202120204 심영준
#include"FTL.h"





void Make_table(unsigned short* table) {
	for (int i = 0; i < SECTOR; i++) {
		table[i] = i;
	}
}
void Print_table(unsigned short* table) {
	for (int i = 0; i < SECTOR; i++) {
		printf("LSN %5hd  ||  PSN %5hd\n", i, table[i]);
	}
}

void FTL_search_sapre_index(unsigned short* table, unsigned short* sparesector_index, char* str, unsigned short* spare_index, counting* count) {
	char input = 0;
	char spare[17] = { 0, };
	unsigned short index = 0;
	Flash_read(SPARESECTORONE, str); //스페어 1 탐색
	count->read_count++;
	while (1) {
		if (str[512] != ' ') { //스페어부분이 비어있지 않으면
			printf("스페어 섹터 1번에 저장되지 않은 값이 있습니다. 복구 작업을 실시합니까? Y/N\n입력 : ");
			scanf("%c", &input);
			if (input == 'Y') {
				for (unsigned short i = SPARESECTORTWO - 1; i >= SPARESECTORONE; i--) { //뒤에서 앞으로 이동
					Flash_read(i, str);
					count->read_count++;
					strncpy(spare, str + 512, 16);
					if (spare[0] != ' ') {
						index = FTL_index_to_short(spare);
						if (table[index] < i) {
							table[index] = i; //테이블의 인덱스값을 스페어공간으로 이동
							if (*spare_index == 6144) {
								(*spare_index) = i + 1;
							}
						}
					}
				}
				break;
			}
			else if (input == 'N') {
				printf("스페어 섹터를 초기화합니다.\n");
				for (unsigned short i = SPAREBLOCKONE; i < SPAREBLOCKONE + 10; i++) {
					Flash_erase(i);
					count->erase_count++;
				}
				break;
			}
			else {
				printf("잘못된 값을 입력하였습니다.\n");
			}
		}
		else {
			break;
		}
	}
	Flash_read(SPARESECTORTWO, str); //스페어 2 탐색
	count->read_count++;
	while (1)
	{
		if (str[512] != ' ') {
			printf("스페어 섹터 2번에 저장되지 않은 값이 있습니다. 복구 작업을 실시합니까? Y/N\n입력 : ");
			scanf("%c", &input);
			if (input == 'Y') {
				for (unsigned short i = ALLSECTOR - 1; i >= SPARESECTORTWO; i--) { //뒤에서 앞으로 이동
					Flash_read(i, str);
					count->read_count++;
					strncpy(spare, str + 512, 16);
					if (str[0] != ' ') {

						index = FTL_index_to_short(spare);
						if (table[index] < i) {
							table[index] = i; //테이블의 인덱스값을 스페어공간으로 이동
							if (*spare_index == 6144) {
								(*spare_index) = i + 1;
							}
						}
					}
				}
				break;
			}
			else if (input == 'N') {
				printf("스페어 섹터를 초기화합니다.\n");
				for (unsigned short i = SPAREBLOCKTWO; i < SPAREBLOCKTWO + 10; i++) {
					Flash_erase(i);
					count->erase_count++;
				}
				break;
			}
			else {
				printf("잘못된 값을 입력하였습니다.\n");
			}
		}
		else {
			break;
		}
	}


	printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
}

void FTL_read(unsigned short index, char* str, char* sector, char* spare, counting* count) {
	Flash_read(index, str);
	count->read_count++;

	strncpy(spare, str + 512, 16);
	if (spare[0] == ' ') {
		printf("해당 섹터의 값이 존재하지 않습니다.\n");
	}
	else {
		strncpy(sector, str, 512);
		printf("sector : %s\n", sector);
	}
}



void FTL_wirte(unsigned short* table, unsigned short index, char* str, char* sector, char* spare, unsigned short* spare_index, counting* count) {
	if (table[index] == index) {//LSN과 PSN이 같을때
		Flash_read(index, str);
		count->read_count++;
		strncpy(spare, str + 512, 16);
		if (spare[0] == 'o') { //값이 존재하는지
			printf("이미 존재하는 값 FTL_overwrite 실행\n");
			FTL_index_to_char(index, spare);
			Flash_write(*spare_index, sector, spare);
			count->write_count++;
			table[index] = (*spare_index)++;
			if (*spare_index == SPARESECTORTWO) {

				*spare_index = FTL_save_original_info(table, SPARESECTORTWO, str, sector, spare, count);
			}
			else if (*spare_index == ALLSECTOR) {

				*spare_index = FTL_save_original_info(table, SPARESECTORONE, str, sector, spare, count);

			}

		}
		else {
			Flash_write(index, sector, "o");
			count->write_count++;
		}
	}
	else {
		printf("이미 존재하는 값 FTL_overwrite 실행\n");
		FTL_index_to_char(index, spare);
		Flash_write(*spare_index, sector, spare);
		count->write_count++;
		table[index] = (*spare_index)++;
		if (*spare_index == SPARESECTORTWO) {

			*spare_index = FTL_save_original_info(table, SPARESECTORTWO, str, sector, spare, count);

		}
		else if (*spare_index == ALLSECTOR) {

			*spare_index = FTL_save_original_info(table, SPARESECTORONE, str, sector, spare, count);

		}
	}
}

unsigned short FTL_save_original_info(unsigned short* table, unsigned short start_index, char* str, char* sector, char* spare, counting* count) {
	unsigned short another_sector = start_index;
	for (unsigned short i = 0; i < SECTOR; i++) {

		if (table[i] != i) { //다르다면
			for (unsigned short j = i / 32 * 32; j < i / 32 * 32 + 32; j++) {//다른 섹터가 존재하면 해당섹터의 수정되지 않은 원본 값을 여분 섹터에 저장
				if (table[j] == j) {
					Flash_read(table[j], str);
					count->read_count++;
					strncpy(sector, str, 512);
					strncpy(spare, str + 512, 16);
					if (spare[0] == 'o') { //값이 존재하면
						Flash_write(another_sector, sector, "o");
						count->write_count++;
						table[j] = another_sector++; // 이 섹터가 끝까지 가면 오류남
					}
				}
			}

			Flash_erase(i / 32);
			count->erase_count++;


			if (another_sector / 32 == (start_index) / 32 + 9) {
				for (unsigned short j = 0; j < SECTOR; j++) {
					if (table[j] != j) {
						Flash_read(table[j], str);
						count->read_count++;
						strncpy(sector, str, 512);
						strncpy(spare, str + 512, 16);
						Flash_write(j, sector, "o");
						count->write_count++;
						table[j] = j;
					}
				}
				for (unsigned short j = start_index / 32; j < start_index  / 32 +10; j++) {
					Flash_erase(j);
					count->erase_count++;
				}
				another_sector = start_index;
			}
			i = i / 32 * 32 + 32;
		}

	}
	for (unsigned short i = 0; i < SECTOR; i++) {
		if (table[i] != i) {
			Flash_read(table[i], str);
			count->read_count++;

			strncpy(sector, str, 512);

			Flash_write(i, sector, "o");
			count->write_count++;
			table[i] = i;
		}
	}
	if (start_index == SPARESECTORONE) {
		for (unsigned short j = SPAREBLOCKTWO; j < ALLSECTOR / 32; j++) {
			Flash_erase(j);
			count->erase_count++;
		}
	}
	else if (start_index == SPARESECTORTWO) {
		for (unsigned short j = SPAREBLOCKONE; j < SPAREBLOCKTWO; j++) {
			Flash_erase(j);
			count->erase_count++;
		}
	}

	if (another_sector == SPARESECTORTWO - 1) {
		for (unsigned short j = SPAREBLOCKONE; j < ALLSECTOR / 32; j++) {
			Flash_erase(j);
			count->erase_count++;
		}
		return SPARESECTORTWO;
	}
	else if (another_sector == ALLSECTOR) {
		for (unsigned short j = SPAREBLOCKTWO; j < ALLSECTOR / 32; j++) {
			Flash_erase(j);
			count->erase_count++;
		}
		return SPARESECTORONE;
	}

	return another_sector;
}



unsigned short FTL_index_to_short(char* str) {
	unsigned short i = 0;
	unsigned short index = 0;
	if (str[0] >= '0' && str[0] <= '9') {
		while (str[i] != ' ') {

			index *= 10;
			index += str[i] - '0';
			i++;
		}
		return index;
	}
	else {
		return 9999;
	}

}

void FTL_index_to_char(unsigned short index, char* str) {
	if (index == 0) {
		str[0] = '0';
		str[1] = '\0';
	}
	else {
		unsigned short i = 0;
		unsigned short count = 1;
		unsigned short temp = index;
		for (unsigned short i = 0; i < 4; i++) {
			if (temp / 10 != 0) {
				count++;
				temp /= 10;
			}
		}
		for (short i = count - 1; i >= 0; i--) {
			str[i] = '0' + (index % 10);
			index /= 10;
		}
		str[count] = '\0';
	}

}
