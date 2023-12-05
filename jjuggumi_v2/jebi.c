#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_LEFT	2
#define DIR_RIGHT	3

void jebi_init(void);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�
int n_jebi_loc, n_jebi_fail_loc; // �� ���� ��ġ, Ż�� ���� ��ġ
int round = 1, now_player = 0; // ����, ���� �̰� �ִ� �÷��̾�

void jebi_init(void) {
	map_init(9, 35);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		px[i] = 2;
		py[i] = 2;
		period[i] = randint(100, 500);

		back_buf[2][2] = '@';  // (0 .. n_player-1)
	}
	tick = 0;
}

//void jebi_reload(void) { //�� �ٽ÷ε�
//	system("cls");
//	map_init(5, 18);
//	for (int i = 0; i < n_player; i++) {
//		back_buf[px[i]][py[i]] = '@';
//	}
//}

//void start_jebi_game(void) {
//	jebi_init();
//	system("cls");
//	display();
//	dialog("�� ������ ���۵˴ϴ�");
//}

void jebi_round(void) {
	int rand_fail_jebi = randint(0, n_player);
	n_jebi_fail_loc = 2 + (rand_fail_jebi * 2);
}

void jebi_map(void) {
	for (int i = 0; i < n_player; i++) {
		n_jebi_loc = 2 + i * 2;
		if (py[now_player] == n_jebi_loc) {
			back_buf[2][n_jebi_loc] = '@';
		}
		else {
			back_buf[2][n_jebi_loc] = '#';
		}
	}
	gotoxy(N_ROW, 0);
	printf("round %d, turn: player %d", round, now_player);
}

void move_jebi_tail(int player2, int nx, int ny) {
	int p = player2;  // �̸��� ��...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = '#';
	px[p] = nx;
	py[p] = ny;
}

void move_jebi_manual(key_t key) {
	// �� �������� ������ �� x, y�� delta
	static int dy[4] = { 0, 0, -2, 2 };

	int dir;  // ������ ����(0~3)
	switch (key) {
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// �������� ���� �ڸ�
	int nx, ny;
	nx = 2;
	ny = py[0] + dy[dir];
	if (ny <= 0 || ny > n_jebi_loc) {
		return;
	}
	jebi_map();
	move_jebi_tail(0, nx, ny);
}

void jebi_select(int key) {
	if (key == K_SPACE) {
		if (py[now_player] == n_jebi_fail_loc) {
			gotoxy(N_ROW + 1, 0);
			printf("player %d fail!", now_player);
		}
		else {
			gotoxy(N_ROW + 1, 0);
			printf("player %d pass!", now_player);
		}
	}
}

void jebi(void) {
	//start_jebi_game();
	//jebi_reload();
	jebi_init();
	system("cls");
	display();
	jebi_map();
	jebi_round();
	while (1) {
		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			jebi_select(key);
			move_jebi_manual(key);

		}

		// player 1 ���ʹ� �������� ������(8����)
		/*for (int i = 1; i < n_player; i++) {
			if (tick % period[i] == 0) {
				move_random(i, -1);
			}
		}*/

		display();
		Sleep(10);
		tick += 10;
	}
}