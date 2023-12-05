// 2023-2 고급프로그래밍 과제: 쭈꾸미 게임
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "jjuggumi.h"

#define	DATA_FILE	"jjuggumi.dat"

int jjuggumi_init(void);

// low 이상 high 이하 난수를 발생시키는 함수
int randint(int low, int high) {
	int rnum = rand() % (high - low + 1) + low;
	return rnum;
}

int jjuggumi_init(void) {
	srand((unsigned int)time(NULL));
	FILE* fp;
	fopen_s(&fp, DATA_FILE, "r");

	if (fp == NULL) {
		return -1; // -1 리턴하면 메인함수에서 처리하고 종료
	}

	// 플레이어 데이터 load
	fscanf_s(fp, "%d", &n_player);
	n_alive = n_player;
	for (int i = 0; i < n_player; i++) {
		// 아직 안 배운 문법(구조체 포인터, 간접참조연산자)
		PLAYER* p = &player[i];
		// 파일에서 각 스탯 최댓값 읽기
		fscanf_s(fp, "%s%d%d",
			p->name, (unsigned int)sizeof(p->name),
			&(p->intel), &(p->str));
		p->stamina = 100; // 100%
		// 현재 상태
		p->is_alive = true;
		p->hasitem = false;
	}

	// 아이템 데이터 load
	fscanf_s(fp, "%d", &n_item);
	for (int i = 0; i < n_item; i++) {
		fscanf_s(fp, "%s%d%d%d",
			item[i].name, (unsigned int)sizeof(item[i].name),
			&(item[i].intel_buf),
			&(item[i].str_buf),
			&(item[i].stamina_buf));
	}
	fclose(fp);
	return 0;
}

int intro(void) { /// 인트로 작성, 5초 내로 끝내기
	printf("================쭈꾸미 게임================\n");
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

int ending(void) {//우승자 출력
	int winner = 0;
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) { winner++; }
	}
	if (winner >= 2 || winner == 0) {
		printf("우승자를 가리지 못했습니다.\n");
	}
	else {
		for (int i = 0; i < n_player; i++) {
			if (player[i].is_alive == true) { printf("우승자는 %d 번 플레이어 입니다.\n", i); }
		}
	}

	return 0;
}

int main(void) {
	jjuggumi_init();
	intro();
	mugunghwa();
	//nightgame();
	//juldarigi();
	jebi();

	ending();
	return 0;
}
