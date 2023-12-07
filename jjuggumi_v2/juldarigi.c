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

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�
int even_str = 0, odd_str = 0, even_player = 0, odd_player = 0, r_stop = 0, l_stop = 0, even_lay = 0, odd_lay = 0;// ¦���� ���� ��, Ȧ���� ���� ��
float sum_str = 0, tmp_str = 0;
int n_dead[PLAYER_MAX];

void juldarigi_init(void) {
	juldarigi_playing = true;
	juldarigi_map_init(3, 31); // #���� �� �׸���
	back_buf[0][15] = ' '; // �߰��� ���� �ձ�
	back_buf[2][15] = ' ';

	for (int i = 0; i < 3; i++) { // ��� �� �׸���
		back_buf[1][14 + i] = '-';
	}
	int even_y = 13, odd_y = 17; // ¦����° �÷��̾�, Ȧ����° �÷��̾�
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

// back_buf[][]�� ���
void juldarigi_move_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void juldarigi_line_move_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = '-';
	px[p] = nx;
	py[p] = ny;
}

void juldarigi_right(int move) { // �� ���������� �����̱�
	r_stop = 0; //���� ������ ����
	for (int i = 0; i < n_player; i++) { // ¦�� �÷��̾� ����
		// � �÷��̾ 14�� �����ϸ� ������ �÷��̾��� ������ ����
		if (i % 2 == 0 && n_dead[i] == 0) {
			if (py[i] == 14) {
				back_buf[px[i]][py[i]] = '-';
				r_stop = 1;
			}
			juldarigi_move_tail(i, px[i], py[i] + move);
		}
	}
	for (int i = n_player - 1; i >= 0; i--) { // Ȧ�� �÷��̾� ����
		if (i % 2 != 0 && n_dead[i] == 0 && r_stop == 0) {
			juldarigi_line_move_tail(i, px[i], py[i] + move);
		}
	}
}

void juldarigi_left(int move) { // �� �������� �����̱�
	l_stop = 0; //������ ������ ����
	for (int i = 0; i < n_player; i++) { // ¦�� �÷��̾� ����
		// � �÷��̾ 16�� �����ϸ� ������ �÷��̾��� ������ ����
		if (i % 2 != 0 && n_dead[i] == 0) {
			if (py[i] == 16) {
				back_buf[px[i]][py[i]] = '-';
				l_stop = 1;
			}
			juldarigi_move_tail(i, px[i], py[i] - move);
		}
	}
	for (int i = n_player - 1; i >= 0; i--) { // Ȧ�� �÷��̾� ����
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
		// player 0�� ������ ������(4����)
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
		if (tick % 3000 == 0) { // 3�ʸ���
			tmp_str = 0;
			sum_str = even_str - odd_str;
		}
		
		if (tick % 1000 == 0) { // 1�ʸ���
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