#include"FTL.h"


void make_table(unsigned char* table) {
	for (unsigned char i = 0; i < SPAREBLOCKONE; i++) {
		table[i] = i;
	}
}


void Print_table(unsigned char* table) {
	for (unsigned char i = 0; i < SPAREBLOCKONE; i++) {
		printf("LBN %5hd  ||  PBN %5hd\n", i, table[i]);
	}
}



void FTL_is_empty_spare_block(unsigned char* table, char* str, char* spare, counting* count) {
	unsigned short sector_index = 0;
	char i = 0;
	while (i < PLUSB/2) {
		Flash_read(((SPAREBLOCKONE)+i * 2) * 32, str);
		count->read_count++;
		if (str[512] != ' ') {
			printf("저장되지 않은 값이 존재합니다. 복구 작업을 실시합니까? Y/N\n");
			while (1) {
				printf("입력 : ");
				scanf("%s", str);
				if (strcmp(str, "Y") == 0) {
					printf("복구작업을 실시합니다\n");
					for (char j = 0; j < PLUSB/2; j++) {
						Flash_read((SPAREBLOCKONE + j * 2) * 32, str);
						count->read_count++;
						if (str[512] >= '0' && str[512] <= '9') {
							strncpy(spare, str + 512, 16);
							sector_index = FTL_index_to_short(spare);
							table[sector_index / 32] = (SPAREBLOCKONE + j*2);
						}
					}
					break;
				}
				else if (strcmp(str, "N") == 0) {
					printf("스페어 블럭을 초기화합니다.\n");
					for (char j = 0; j < 20; j++) {
						Flash_erase(SPAREBLOCKONE + j);
						count->erase_count++;
					}
					break;
				}
				else {
					printf("다시입력해주세요\n");
				}
			}

			break;
		}
		i++;
	}
	printf("읽기 : %d, 쓰기 : %d, 지우기 :%d\n", count->read_count, count->write_count, count->erase_count);
}

void FTL_read(unsigned short index, unsigned char* table, char* str, char* sector, char* spare, counting* count) {
	if (index > 6143) {
		printf("너무 큰 값입니다.\n");
	}
	else {
		if (table[index / 32] != index / 32) {
			for (char i = 63; i >= 0; i--) {
				Flash_read(table[index / 32]*32 + i, str);
				count->read_count++;
				strncpy(spare, str + 512, 16);
				printf("%s, %hd\n",spare, FTL_index_to_short(spare));
				if (index == FTL_index_to_short(spare)) {
					strncpy(sector, str, 512);
					printf("sector : %s\n", sector);
					i = -1;
				}
				else if (i == 0 && index != FTL_index_to_short(spare) / 32) {
					printf("해당 섹터에 값이 없습니다.\n");
				}
			}

		}
		else {
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
	}
	



}
void FTL_write(unsigned short index, unsigned char* table, char* str, char* sector, char* spare, counting* count) {
	if (index > 6143) {
		printf("너무 큰 값입니다.\n");
	}
	else {
		if (index / 32 == table[index / 32]) {
			Flash_read(index, str);
			count->read_count++;
			if (str[512] == ' ') {
				Flash_write(index, sector, "o");
				count->write_count++;
			}
			else {
				printf("이미존재하는 섹터\n");
				FTL_overwrite(index, table, str, sector, spare, count);
			}
		}
		else {
			printf("이미 추가블럭에 연산하는 블럭\n");
			FTL_overwrite(index, table, str, sector, spare, count);
		}
	}
	

}
void FTL_overwrite(unsigned short index, unsigned char* table, char* str, char* sector, char* spare, counting* count) {
	char sector_temp[513] = { 0, };
	if (index / 32 == table[index / 32]) { //신규 스페어블럭 연산
		unsigned char i = 0;
		unsigned char j = 0;
		unsigned char k = 0;

		while (i < PLUSB / 2) {
			Flash_read((SPAREBLOCKONE + (i * 2)) * 32, str);
			count->read_count++;
			if (str[512] == ' ') {
				table[index / 32] = (SPAREBLOCKONE + (i * 2));// 192 194 196 ....
				while (k < 32) { //원본 블럭 전체 저장(해당부분)
					Flash_read((index / 32) * 32 + k, str); // 원본 읽기
					count->read_count++;

					if (str[512] == 'o') {
						FTL_index_to_str((index / 32) * 32 + k, spare);
						strncpy(sector_temp, str, 512);
						Flash_write((table[index / 32] * 32) + j++, sector_temp, spare); //바뀐 블럭에 저장
						count->write_count++;
					}
					k++;
				}//스페어 블럭에 저장


				FTL_index_to_str(index, spare);
				Flash_write(((table[index / 32] * 32) + j), sector, spare);
				count->write_count++;
				break;
			}
			else {
				i++;
				if ((i * 2) + 192 == ALLSECTOR / 32) {
					for (j = 0; j < PLUSB / 2; j++) {
						FTL_save(SPAREBLOCKONE + j * 2, table, str, sector_temp, spare, count); // 스페어 초기화 저장
					}
					j = 0;
					make_table(table);
					table[index / 32] = (SPAREBLOCKONE);
					while (k < 32) { //원본 블럭 전체 저장(해당부분)
						Flash_read((index / 32) * 32 + k, str);
						count->read_count++;

						if (str[512] == 'o') {
							FTL_index_to_str(((index / 32) * 32) + k, spare);
							strncpy(sector_temp, str, 512);
							Flash_write((table[index / 32] * 32) + j++, sector_temp, spare);
							count->write_count++;
						}
						k++;
					}//스페어 블럭에 저장
					FTL_index_to_str(index, spare);
					Flash_write((table[index / 32]*32 + j), sector, spare);
					count->write_count++;
					i = 0;
					break;
				}
			}
		}
	}
	else { //여기서부터 기존 스페어에 저장
		unsigned char i = 0;

		Flash_read(table[index / 32] * 32 + 63, str);
		count->read_count++;
		if (str[512] != ' ') {
			unsigned char temp_index = table[index / 32];
			FTL_save(table[index / 32], table, str, sector_temp, spare, count);
			table[index / 32] = temp_index;
		}
		else {
			while (i < 64) {
				//64칸 채우고 다음 칸으로 이동하게 만듬
				//>> 64칸까지는 유지 만약 64칸에 값이 있으면 연산 ㄱㄱ
				Flash_read(table[index / 32] * 32 + i, str);
				count->read_count++;

				if (str[512] == ' ') {
					FTL_index_to_str(index, spare);
					Flash_write(table[index / 32] * 32 + i, sector, spare);
					count->write_count++;
					break;
				}
				else {

					i++;
				}

			}
		}




	}

}

void FTL_save(unsigned char index, unsigned char* table, char* str, char* sector, char* spare, counting* count) {
	unsigned short sector_index = 0;


	for (unsigned char i = 0; i < SPAREBLOCKONE; i++) {
		if (table[i] == index) {
			Flash_erase(i);
			count->erase_count++;

			for (char j = 63; j >= 0; j--) {
				Flash_read(table[i] * 32 + j, str);
				count->read_count++;
				if (str[512] != ' ') {
					strncpy(sector, str, 512);
					strncpy(spare, str + 512, 16);
					sector_index = FTL_index_to_short(spare);
					Flash_read(sector_index, str);
					count->read_count++;
					if (str[512] == ' ') {
						Flash_write(sector_index, sector, "o");
						count->write_count++;
					}
				}
			}
			table[i] = i;
		}
	}
	Flash_erase(index);
	count->erase_count++;
	Flash_erase(index + 1);
	count->erase_count++;

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


void FTL_index_to_str(unsigned short index, char* str) {
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