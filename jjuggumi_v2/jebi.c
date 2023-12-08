#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_LEFT	2
#define DIR_RIGHT	3

void jebi_init(void);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
int jebi_selected[PLAYER_MAX]; // 선택된 제비
int n_jebi_loc, n_jebi_fail_loc; // 각 제비 위치, 탈락 제비 위치
int round = 1, now_player = 0; // 라운드, 제비 뽑고 있는 플레이어

void jebi_init(void) {
	for (int i = 0; i < n_player; i++) {
		player_clear[i] = false;
		stm_heal(i);
	}
	map_init(9, 35);
	for (int i = 0; i < n_alive; i++) {
		px[i] = 4;
		py[i] = 2;
		period[i] = randint(100, 500);

		if (now_player == i) {
			back_buf[px[i]][py[i]] = '@';  // (0 .. n_alive-1)
		}
	}
	tick = 0;
}

void jebi_round(void) {
	int rand_fail_jebi = randint(0, n_alive - 1);
	n_jebi_fail_loc = 2 + (rand_fail_jebi * 2);
}

void mark_jebi(int jebi_loc) {
	jebi_selected[now_player] = jebi_loc;
}

int selected_jebi(int jebi_loc) {
	for (int i = 0; i < n_alive; i++) {
		if (jebi_selected[i] == jebi_loc) {
			return 1; // 이미 선택된 제비
		}
	}
	return 0;
}

void jebi_map(void) {
	for (int i = 0; i < n_alive; i++) {
		n_jebi_loc = 2 + i * 2;
		if (py[now_player] == n_jebi_loc) {
			back_buf[4][n_jebi_loc] = '@';
		}
		else if (selected_jebi(n_jebi_loc)) {
			back_buf[4][n_jebi_loc] = '!';
		}
		else {
			back_buf[4][n_jebi_loc] = '?';
		}
	}
	gotoxy(N_ROW, 0);
	printf("round %d, turn: player %d", round, now_player);
}

void jebi_reload(void) { //맵 다시로드
	system("cls");
	map_init(9, 35);
	gotoxy(N_ROW, 0);
	printf("round %d, turn: player %d", round, now_player);
	map_init(9, 35);
	for (int i = 0; i < n_alive; i++) {
		px[i] = 4;
		py[i] = 2;

		if (player[i].is_alive == true && now_player == i) {
			back_buf[px[i]][py[i]] = '@';
		}
	}

	for (int i = 0; i < n_alive; i++) {
		n_jebi_loc = 2 + i * 2;
		if (py[now_player] == n_jebi_loc) {
			back_buf[4][n_jebi_loc] = '@';
		}
		else if (selected_jebi(n_jebi_loc)) {
			back_buf[4][n_jebi_loc] = '!';
		}
		else {
			back_buf[4][n_jebi_loc] = '?';
		}
	}
}

void jebi_newround(void) {
	n_jebi_loc = 0;
	back_buf[4][n_alive * 2] = ' ';
	n_alive--;
	round++;
	if (player[0].is_alive == false) { now_player = 1; }
	else { now_player = 0; }

	for (int i = 0; i < n_alive; i++) {
		jebi_selected[i] = 0;
	}
	jebi_round();
	jebi_reload();
	display();
}

void move_jebi_tail(int player2, int nx, int ny) {
	int p = player2;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	if (selected_jebi(py[p])) {
		back_buf[px[p]][py[p]] = '!';
	}
	else {
		back_buf[px[p]][py[p]] = '?';
	}
	px[p] = nx;
	py[p] = ny;
}

void move_jebi_manual(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dy[4] = { 0, 0, -2, 2 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = 4;
	ny = py[0] + dy[dir];
	if (ny <= 0 || ny > n_jebi_loc) {
		return;
	}
	move_jebi_tail(0, nx, ny);
}

void jebi_random(int player) {
	int p = player;  // 이름이 길어서...
	int tmp, nx, ny;  // 움직여서 다음에 놓일 자리

	do {
		tmp = randint(0, n_alive - 1);
		nx = 4;
		ny = 2 + (tmp * 2);
	} while (selected_jebi(ny) != 0);
	move_jebi_tail(p, nx, ny);
}

void jebi_select(int key) {
	if (key == K_SPACE) {
		if (py[now_player] == n_jebi_fail_loc) {
			char msg_fail[] = "player d fail!";
			msg_fail[7] = now_player + '0';
			dialog(msg_fail);
			player[now_player].is_alive = false;
			jebi_newround();

		}
		else {
			if (selected_jebi(py[now_player])) {
				dialog("already select!");
				return;
			}
			char msg_pass[] = "player d pass!";
			msg_pass[7] = now_player + '0';
			dialog(msg_pass);
			mark_jebi(py[now_player]);
			now_player += 1;
			jebi_map();
		}
	}
}



void jebi(void) {
	jebi_init();
	system("cls");
	display();
	dialog("곧 게임이 시작됩니다");
	jebi_round();
	jebi_map();
	while (1) {
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			if (now_player == 0 && player[0].is_alive == true) {
				move_jebi_manual(key);
				jebi_select(key);
			}
		}

		// player 1 부터는 랜덤으로 움직임(8방향)
		for (int i = 1; i < n_player; i++) {
			if (now_player == i && player[i].is_alive == false) {
				now_player += 1;
				jebi_map();
			}
			if (tick % period[i] == 0) {
				if (now_player == i && player[i].is_alive == true) {
					jebi_random(i);
					display();
					jebi_select(32);
				}
			}
		}

		display();
		Sleep(10);
		tick += 10;

		if (n_alive <= 1) {
			break;
		}
	}
}
