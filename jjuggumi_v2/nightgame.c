#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
#include <time.h>


#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

#define TRUE  1
#define FALSE 0

void gamemap_night_init(void);
void move_night_manual(key_t key);
void move_night_random(int i, int dir);
void move_night_tail(int i, int nx, int ny);
void finish_night_line(void);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX], prex[PLAYER_MAX], prey[PLAYER_MAX], dead_player[PLAYER_MAX], movable[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
int str_intro2 = 0, tagger_front2 = 1, playing_member;
int ix[ITEM_MAX], iy[ITEM_MAX];



//void move_toward_item(int player_index) {
//	int player_row = px[player_index];
//	int player_col = py[player_index];
//
//	int item_row = ix[0];  // 첫 번째 아이템을 대상으로 함
//	int item_col = iy[0];
//
//	int row_diff = item_row - player_row;
//	int col_diff = item_col - player_col;
//
//	if (abs(row_diff) >= abs(col_diff)) {
//		// 세로로 더 멀리 떨어져 있을 때
//		if (row_diff > 0) {
//			move_night_random(player_index, 1); // 아래로 이동
//		}
//		else {
//			move_night_random(player_index, 0); // 위로 이동
//		}
//	}
//	else {
//		// 가로로 더 멀리 떨어져 있을 때
//		if (col_diff > 0) {
//			move_night_random(player_index, 3); // 오른쪽으로 이동
//		}
//		else {
//			move_night_random(player_index, 2); // 왼쪽으로 이동
//		}
//	}
//}



void gamemap_night_init(void) {
	map_init(9, 35);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		// 같은 자리가 나오면 다시 생성
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
	}
	srand(time(NULL));
	int rdnum = rand() % 10 + 1;
	for (int i = 1; i < rdnum; i++) {
		// 같은 자리가 나오면 다시 생성
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		ix[i] = x;
		iy[i] = y;
		period[i] = randint(100, 500);

		back_buf[ix[i]][iy[i]] = 'I';  // (0 .. n_player-1)
	}

	tick = 0;
}
void move_night_manual(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_night_tail(0, nx, ny);

}


// 0 <= dir <= 4가 아니면 랜덤
void move_night_random(int player, int dir) {
	int p = player;  // 이름이 길어서...
	int nx, ny;  // 움직여서 다음에 놓일 자리

	// 움직일 공간이 없는 경우는 없다고 가정(무한 루프에 빠짐)	

	do {
		if (dir == 0) {
			nx = px[p] + 1;
			ny = py[p];
		}
		else if (dir == 1) {
			nx = px[p] - 1;
			ny = py[p];
		}
		else if (dir == 2) {
			nx = px[p];
			ny = py[p] - 1;
		}
		else if (dir == 3) {
			nx = px[p];
			ny = py[p] + 1;
		}
		else if (dir == 4) {
			nx = px[p];
			ny = py[p];
		}
		else {
			nx = px[p] + randint(-1, 1);
			ny = py[p] + randint(-1, 0);

		}

		if (!placable(nx, ny)) { // 가야할 공간이 없으면 랜덤으로 다시 지정 단 뒤로는 없음
			nx = px[p] + randint(-1, 1);
			ny = py[p] + randint(-1, 0);
		}
	} while (!placable(nx, ny));
	move_night_tail(p, nx, ny);
}

// back_buf[][]에 기록
void move_night_tail(int player2, int nx, int ny) {
	int p = player2;  // 이름이 길어서...
	if (player_clear[p] == false && player[p].is_alive == true && player_pause == false) {
		back_buf[nx][ny] = back_buf[px[p]][py[p]];
		back_buf[px[p]][py[p]] = ' ';
		px[p] = nx;
		py[p] = ny;
	}
}


int random_night_move(void) {
	int rand_move;
	rand_move = rand() % 99; //0~99 랜덤한 숫자
	if (rand_move <= 69) { // 70% 확률로 왼쪽으로 이동
		return 2;
	}
	else if (70 <= rand_move && rand_move <= 79) { // 10% 확률로 위쪽으로 이동
		return 0;
	}
	else if (80 <= rand_move && rand_move <= 89) { // 10% 확률로 아래쪽으로 이동
		return 1;
	}
	else { // 10% 확률로 제자리
		return 4;
	}
}





void finish_night_line(void) {
	for (int i = 0; i < n_player; i++) {
		if (1 < prex[i] && prex[i] <= 6 && prey[i] <= 2) {
			if (!(prex[i] == 2 && prey[i] == 2 || prex[i] == 6 && prey[i] == 2)) {
				if (player_clear[i] == false) {
					player_clear[i] = true;
					playing_member--;
					back_buf[ix[i]][iy[i]] = ' ';
				}

			}
		}
	}
}

void night_reload(void) { //맵 다시로드
	system("cls");
	map_init(9, 35);

	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true && player_clear[i] == false) {
			back_buf[px[i]][py[i]] = '0' + i;
		}
	}
}

int check_night_movable() {
	for (int i = 0; i < n_player; ++i) {
		movable[i] = FALSE;
	}
	for (int cur = 0; cur < n_player; cur++) {
		if (player[cur].is_alive == FALSE) {
			continue;
		}
		for (int other = 0; other < n_player; ++other) {
			if (player[other].is_alive == FALSE) {
				continue;
			}
			if (cur == other) {
				continue;
			}
			if (py[cur] <= py[other]) {
				continue;
			}
			if (px[cur] == px[other]) {
				movable[cur] = TRUE;
				break;
			}
		}
	}
	return 0;
}

void player_night_move_check(void) {
	if (tagger_front2 == 1) { //뒤 볼때가 1 앞 보면 0
		for (int i = 0; i < n_player; i++) {
			prex[i] = px[i];
			prey[i] = py[i];
		}
	}
	else {
		check_night_movable();
		for (int i = 0; i < n_player; i++) {
			if (movable[i] == FALSE) {
				if (px[i] == prex[i] && py[i] == prey[i]) {
					continue;
				}
				if (player[i].is_alive == true) {
					player[i].is_alive = false;
					back_buf[px[i]][py[i]] = ' ';
					display();
					
					n_alive--;
					playing_member--;
				}
			}
			else {
				prex[i] = px[i];
				prey[i] = py[i];
			}
		}
	}
}

void start_night_game(void) { //모든 플레이어를 출발선으로 배치, 술래 배치
	gamemap_night_init();
	system("cls");
	display();
	playing_member = n_alive;
	for (int i = 0; i < n_player; i++) {
		if (n_player >= 7) {
			move_night_tail(i, 1 + i, 33);
		}
		else if (n_player >= 5) {
			move_night_tail(i, 2 + i, 33);
		}
		else if (n_player >= 3) {
			move_night_tail(i, 3 + i, 33);
		}
		else {
			move_night_tail(i, 4 + i, 33);
		}
	}

	dialog("곧 게임이 시작됩니다");
}

void nightgame(void) {
	start_night_game();
	gamemap_night_init();
	dialog("야간운동");
	while (1) {
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_night_manual(key);
		}

		for (int i = 1; i < n_player; i++) {

			if (tick % period[i] == 0 && tagger_front2 == 1) { // 뒤에 볼 때
				move_night_random(i, random_night_move());

			}
			else if (tick % period[i] == 0 && tagger_front2 == 0) { // 앞에 볼 때
				int rand_move;
				rand_move = rand() % 99; //0~99 랜덤한 숫자
				if (rand_move < 10) { // 10% 확률로 움직임
					move_night_random(i, random_night_move());
				}
			}
		}
		player_night_move_check();
		finish_night_line();
		display();
		Sleep(10);
		tick += 10;
		if (tick % 3000 == 0 && tagger_front2 == 0) {
			tagger_front2 = 1;
			night_reload();
		}
		if (str_intro2 == 22) {
			str_intro2 = 0;
			tagger_front2 = 0;
			tick = 0;
		}
		if (n_alive <= 1 || playing_member <= 0) {
			break;
		}
	}
}
