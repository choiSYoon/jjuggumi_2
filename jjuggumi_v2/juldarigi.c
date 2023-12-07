#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void juldarigi_init(void);
void juldarigi_move_tail(int i, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
int even_str = 0, odd_str = 0, even_player = 0, odd_player = 0, r_stop = 0, l_stop = 0, even_lay = 0, odd_lay = 0;// 짝수팀 힘의 합, 홀수팀 힘의 합
float sum_str = 0, tmp_str = 0;
int n_dead[PLAYER_MAX];

void juldarigi_init(void) {
	juldarigi_playing = true;
	juldarigi_map_init(3, 31); // #으로 맵 그리기
	back_buf[0][15] = ' '; // 중간에 구멍 뚫기
	back_buf[2][15] = ' ';

	for (int i = 0; i < 3; i++) { // 가운데 줄 그리기
		back_buf[1][14 + i] = '-';
	}
	int even_y = 13, odd_y = 17; // 짝수번째 플레이어, 홀수번째 플레이어
	for (int i = 0; i < n_player; i++) {
		px[i] = 1;
		if (i % 2 == 0) {
			back_buf[1][even_y] = '0' + i;
			py[i] = even_y;
			even_player++;
			even_y--;
		}
		else {
			back_buf[1][odd_y] = '0' + i;
			py[i] = odd_y;
			odd_player++;
			odd_y++;
		}
	}
	
	tick = 0;
}

void juldarigi_str(void) {
	for (int i = 0; i < n_player; i++) {
		if (i % 2 == 0) {
			even_str += player[i].str;
			if (player[i].hasitem == true) {
				even_str += player[i].item.str_buf;
			}
		}
		else {
			odd_str += player[i].str;
			if (player[i].hasitem == true) {
				odd_str += player[i].item.str_buf;
			}
		}
	}
	sum_str = even_str - odd_str;
}

// back_buf[][]에 기록
void juldarigi_move_tail(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void juldarigi_line_move_tail(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = '-';
	px[p] = nx;
	py[p] = ny;
}

void juldarigi_right(int move) { // 줄 오른쪽으로 움직이기
	r_stop = 0; //왼쪽 움직임 멈춤
	for (int i = 0; i < n_player; i++) { // 짝수 플레이어 제어
		// 어떤 플레이어가 14에 도착하면 오른쪽 플레이어의 움직임 멈춤
		if (i % 2 == 0 && n_dead[i] == 0) {
			if (py[i] == 14) {
				back_buf[px[i]][py[i]] = '-';
				r_stop = 1;
			}
			juldarigi_move_tail(i, px[i], py[i] + move);
		}
	}
	for (int i = n_player - 1; i >= 0; i--) { // 홀수 플레이어 제어
		if (i % 2 != 0 && n_dead[i] == 0 && r_stop == 0) {
			juldarigi_line_move_tail(i, px[i], py[i] + move);
		}
	}
}

void juldarigi_left(int move) { // 줄 왼쪽으로 움직이기
	l_stop = 0; //오른쪽 움직임 멈춤
	for (int i = 0; i < n_player; i++) { // 짝수 플레이어 제어
		// 어떤 플레이어가 16에 도착하면 오른쪽 플레이어의 움직임 멈춤
		if (i % 2 != 0 && n_dead[i] == 0) {
			if (py[i] == 16) {
				back_buf[px[i]][py[i]] = '-';
				l_stop = 1;
			}
			juldarigi_move_tail(i, px[i], py[i] - move);
		}
	}
	for (int i = n_player - 1; i >= 0; i--) { // 홀수 플레이어 제어
		if (i % 2 == 0 && n_dead[i] == 0 && l_stop == 0) {
			juldarigi_line_move_tail(i, px[i], py[i] - move);
		}
	}
}

void juldarigi_check(void) {
	for (int i = 0; i < n_player; i++) {
		if (py[i] == 15 && n_dead[i] == 0) {
			n_dead[i] = 1;
			char msg_fail[] = "player d fail!";
			msg_fail[7] = i + '0';
			dialog(msg_fail);
			if (i % 2 == 0) {
				even_str -= player[i].str;
				even_player--;
				if (player[i].hasitem == true) {
					even_str -= player[i].item.str_buf;
				}
			}
			else {
				odd_str -= player[i].str;
				odd_player--;
				if (player[i].hasitem == true) {
					odd_str -= player[i].item.str_buf;
				}
			}
			if (player[i].is_alive == true && player[i].hasitem == true) {
				player[i].hasitem = false;
				player[i].item.intel_buf = 0;
				player[i].item.stamina_buf = 0;
				player[i].item.str_buf = 0;
				player[i].item.name == "";
			}
		}
	}
	sum_str = (even_str - odd_str) + tmp_str;
}

void juldarigi(void) {
	juldarigi_init();
	juldarigi_str();
	
	system("cls");
	display();
	gotoxy(N_ROW + 2, 0);
	printf("str: %5.1f", sum_str);
	Sleep(1000);
	while (1) {
		gotoxy(N_ROW + 2, 0);
		printf("str: %5.1f", sum_str);
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT || even_player == 0 || odd_player == 0) {
			break;
		}
		else if (key == K_ZKEY) {
			tmp_str -= 1;
			juldarigi_check();
		}
		else if (key == K_SLASH) {
			tmp_str += 1;
			juldarigi_check();
		}
		else if (key == K_XKEY) {
			even_str *= 2;
			for (int i = 0; i < n_player; i++) {
				if (i % 2 == 0) {
					player[i].stamina = (player[i].stamina / 100) * 30;
				}
			}
			even_lay = 1;
			juldarigi_check();
		}
		else if (key == K_COL) {
			odd_str *= 2;
			for (int i = 0; i < n_player; i++) {
				if (i % 2 != 0) {
					player[i].stamina = (player[i].stamina / 100) * 30;
				}
			}
			odd_lay = 1;
			juldarigi_check();
		}
		if (tick % 3000 == 0) { // 3초마다
			tmp_str = 0;
			sum_str = even_str - odd_str;
		}
		
		if (tick % 1000 == 0) { // 1초마다
			if (sum_str >= 0) {
				if (odd_lay == 1 || even_lay == 1) { 
					juldarigi_right(2); 
					odd_lay = 0;
					even_lay = 0;
				}
				else { juldarigi_right(1);}
				display();
				juldarigi_check();
			}
			else {
				if (odd_lay == 1 || even_lay == 1) {
					juldarigi_left(2);
					odd_lay = 0;
					even_lay = 0;
				}
				else { juldarigi_left(1); }
				display();
				juldarigi_check();
			}
		}
		display();
		Sleep(10);
		tick += 10;
	}
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == false && n_dead[i] == 0) {
			player[i].is_alive = true;
		}
		juldarigi_playing = 0;
	}
}