//202120204 �ɿ���
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
	Flash_read(SPARESECTORONE, str); //����� 1 Ž��
	count->read_count++;
	while (1) {
		if (str[512] != ' ') { //�����κ��� ������� ������
			printf("����� ���� 1���� ������� ���� ���� �ֽ��ϴ�. ���� �۾��� �ǽ��մϱ�? Y/N\n�Է� : ");
			scanf("%c", &input);
			if (input == 'Y') {
				for (unsigned short i = SPARESECTORTWO - 1; i >= SPARESECTORONE; i--) { //�ڿ��� ������ �̵�
					Flash_read(i, str);
					count->read_count++;
					strncpy(spare, str + 512, 16);
					if (spare[0] != ' ') {
						index = FTL_index_to_short(spare);
						if (table[index] < i) {
							table[index] = i; //���̺��� �ε������� ������������ �̵�
							if (*spare_index == 6144) {
								(*spare_index) = i + 1;
							}
						}
					}
				}
				break;
			}
			else if (input == 'N') {
				printf("����� ���͸� �ʱ�ȭ�մϴ�.\n");
				for (unsigned short i = SPAREBLOCKONE; i < SPAREBLOCKONE + 10; i++) {
					Flash_erase(i);
					count->erase_count++;
				}
				break;
			}
			else {
				printf("�߸��� ���� �Է��Ͽ����ϴ�.\n");
			}
		}
		else {
			break;
		}
	}
	Flash_read(SPARESECTORTWO, str); //����� 2 Ž��
	count->read_count++;
	while (1)
	{
		if (str[512] != ' ') {
			printf("����� ���� 2���� ������� ���� ���� �ֽ��ϴ�. ���� �۾��� �ǽ��մϱ�? Y/N\n�Է� : ");
			scanf("%c", &input);
			if (input == 'Y') {
				for (unsigned short i = ALLSECTOR - 1; i >= SPARESECTORTWO; i--) { //�ڿ��� ������ �̵�
					Flash_read(i, str);
					count->read_count++;
					strncpy(spare, str + 512, 16);
					if (str[0] != ' ') {

						index = FTL_index_to_short(spare);
						if (table[index] < i) {
							table[index] = i; //���̺��� �ε������� ������������ �̵�
							if (*spare_index == 6144) {
								(*spare_index) = i + 1;
							}
						}
					}
				}
				break;
			}
			else if (input == 'N') {
				printf("����� ���͸� �ʱ�ȭ�մϴ�.\n");
				for (unsigned short i = SPAREBLOCKTWO; i < SPAREBLOCKTWO + 10; i++) {
					Flash_erase(i);
					count->erase_count++;
				}
				break;
			}
			else {
				printf("�߸��� ���� �Է��Ͽ����ϴ�.\n");
			}
		}
		else {
			break;
		}
	}


	printf("�б� : %d, ���� : %d, ����� :%d\n", count->read_count, count->write_count, count->erase_count);
}

void FTL_read(unsigned short index, char* str, char* sector, char* spare, counting* count) {
	Flash_read(index, str);
	count->read_count++;

	strncpy(spare, str + 512, 16);
	if (spare[0] == ' ') {
		printf("�ش� ������ ���� �������� �ʽ��ϴ�.\n");
	}
	else {
		strncpy(sector, str, 512);
		printf("sector : %s\n", sector);
	}
}



void FTL_wirte(unsigned short* table, unsigned short index, char* str, char* sector, char* spare, unsigned short* spare_index, counting* count) {
	if (table[index] == index) {//LSN�� PSN�� ������
		Flash_read(index, str);
		count->read_count++;
		strncpy(spare, str + 512, 16);
		if (spare[0] == 'o') { //���� �����ϴ���
			printf("�̹� �����ϴ� �� FTL_overwrite ����\n");
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
		printf("�̹� �����ϴ� �� FTL_overwrite ����\n");
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

		if (table[i] != i) { //�ٸ��ٸ�
			for (unsigned short j = i / 32 * 32; j < i / 32 * 32 + 32; j++) {//�ٸ� ���Ͱ� �����ϸ� �ش缽���� �������� ���� ���� ���� ���� ���Ϳ� ����
				if (table[j] == j) {
					Flash_read(table[j], str);
					count->read_count++;
					strncpy(sector, str, 512);
					strncpy(spare, str + 512, 16);
					if (spare[0] == 'o') { //���� �����ϸ�
						Flash_write(another_sector, sector, "o");
						count->write_count++;
						table[j] = another_sector++; // �� ���Ͱ� ������ ���� ������
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
