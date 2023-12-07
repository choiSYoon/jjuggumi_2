// 2023-2 ������α׷��� ����: �޲ٹ� ����
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "jjuggumi.h"

#define	DATA_FILE	"jjuggumi.dat"

int jjuggumi_init(void);

// low �̻� high ���� ������ �߻���Ű�� �Լ�
int randint(int low, int high) {
	int rnum = rand() % (high - low + 1) + low;
	return rnum;
}

int jjuggumi_init(void) {
	srand((unsigned int)time(NULL));
	FILE* fp;
	fopen_s(&fp, DATA_FILE, "r");

	if (fp == NULL) {
		return -1; // -1 �����ϸ� �����Լ����� ó���ϰ� ����
	}

	// �÷��̾� ������ load
	fscanf_s(fp, "%d", &n_player);
	n_alive = n_player;
	for (int i = 0; i < n_player; i++) {
		// ���� �� ��� ����(����ü ������, ��������������)
		PLAYER* p = &player[i];
		// ���Ͽ��� �� ���� �ִ� �б�
		fscanf_s(fp, "%s%d%d",
			p->name, (unsigned int)sizeof(p->name),
			&(p->intel), &(p->str));
		p->stamina = 100; // 100%
		// ���� ����
		p->is_alive = true;
		p->hasitem = false;
	}

	// ������ ������ load
	fscanf_s(fp, "%d", &n_item);
	for (int i = 0; i < n_item; i++) {
		fscanf_s(fp, "%s%d%d%d",
			item[i].name, (unsigned int)sizeof(item[i].name),
			&(item[i].intel_buf),
			&(item[i].str_buf),
			&(item[i].stamina_buf));
	}
	fclose(fp);
	//player[0].hasitem = true;
	//player[0].item = item[0]; // �ӽ÷� ������ ������ �ִ��� �׽�Ʈ
	return 0;
}

int intro(void) { /// ��Ʈ�� �ۼ�, 5�� ���� ������
	printf("================�޲ٹ� ����================\n");
	Sleep(300);
	printf("         *   *****        *\n");
	printf("        **  *******        **\n");
	printf("        ** *********        **\n");
	printf("       *** *  **  **        ***\n");
	printf("       *** *********        ***\n");
	printf("        **  *******         ***\n");
	printf("        **   *****          **\n");
	printf("         *    ***           **\n");
	printf("         *******************\n");
	printf("         *****      *******\n");
	Sleep(300);
	printf("===========================================\n");
	Sleep(1000);

	system("cls");
	printf("================GAME START================\n");
	for (int i = 0; i < 4; i++) {
		printf("                     *                      \n");
		Sleep(300);
	}
	return 0;
}

int ending(void) {//����� ���
	int winner = 0;
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) { winner++; }
	}
	if (winner >= 2 || winner == 0) {
		printf("����ڸ� ������ ���߽��ϴ�.\n");
	}
	else {
		for (int i = 0; i < n_player; i++) {
			if (player[i].is_alive == true) { printf("����ڴ� %d �� �÷��̾� �Դϴ�.\n", i); }
		}
	}

	return 0;
}

int main(void) {
	jjuggumi_init();
	//intro();
	//mugunghwa();
	//nightgame();
	juldarigi();
	jebi();

	ending();
	return 0;
}
