#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void nightgame_init(void);
void nightgame_move_manual(key_t key);
void nightgame_move_random(int i, int dir);
void nightgame_move_tail(int i, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX], p_num_item[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�, ������ ������ ��ȣ
int ix[ITEM_MAX], iy[ITEM_MAX], item_type[ITEM_MAX]; // �� ������ ��ġ, ������ ����
int num_item = 0;

void nightgame_init(void) {
	for (int i = 0; i < n_player; i++) {
		player_clear[i] = false;
		stm_heal(i);
	}
	map_init(8, 23);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		// ���� �ڸ��� ������ �ٽ� ����
		if (player[i].is_alive == true) {
			do {
				x = randint(1, N_ROW - 2);
				y = randint(1, N_COL - 2);
			} while (!placable(x, y));
			px[i] = x;
			py[i] = y;
			period[i] = randint(100, 500);

			back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
		}
	}

	for (int i = 0; i < n_alive - 1; i++) {
		// ���� �ڸ��� ������ �ٽ� ����
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		ix[i] = x;
		iy[i] = y;
		item_type[i] = randint(0, n_item);
		num_item++;
		back_buf[ix[i]][iy[i]] = 'I';  // (0 .. n_player-1)
	}
	tick = 0;
}

void nightgame_move_manual(key_t key) {
	// �� �������� ������ �� x, y�� delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // ������ ����(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// �������� ���� �ڸ�
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	nightgame_move_tail(0, nx, ny);
}

// 0 <= dir < 4�� �ƴϸ� ����
void nightgame_move_random(int player, int dir) {
	int p = player;  // �̸��� ��...
	int nx, ny;  // �������� ������ ���� �ڸ�

	// ������ ������ ���� ���� ���ٰ� ����(���� ������ ����)	

	do {
		// �����۰� (�������� ���� �÷��̾�)�� ��, ���� ����� ���� ���ؼ� �̵�
		nx = px[p] + randint(-1, 1);
		ny = py[p] + randint(-1, 1);
	} while (!placable(nx, ny));

	nightgame_move_tail(p, nx, ny);
}

// back_buf[][]�� ���
void nightgame_move_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void item_mount(int player1, int num_item) {
	int p = player1;  // �̸��� ��...
	player[p].hasitem = true;
	p_num_item[p] = num_item;
	player[p].item.intel_buf = item[num_item].intel_buf;
	player[p].item.stamina_buf = item[num_item].stamina_buf;
	player[p].item.str_buf = item[num_item].str_buf;
}

void item_unmount(int player1) {
	int p = player1;  // �̸��� ��...
	player[p].hasitem = false;
	p_num_item[p] = -1;
	player[p].item.intel_buf = 0;
	player[p].item.stamina_buf = 0;
	player[p].item.str_buf = 0;
}

void nightgame_item_loot(void) {
	for (int i = 0; i < n_player; i++) {
		for (int j = 0; j < n_player - 1; j++) {
			if (px[i] == ix[j] && py[i] == iy[j]) {
				gotoxy(N_ROW + 2, 0);
				printf("������ ����: %d / x: %d / y: %d", item_type[j], ix[j], iy[j]);
				
				if (player[i].hasitem == false) {
					item_mount(i, j);
				}
				else {
					if (i == 0) {
						display();
						gotoxy(N_ROW + 1, 0);
						printf("�������� ��ȯ�Ͻðڽ��ϱ�? (Y/N)");
						int key = _getch();
						if (key == K_YKEY) {
							item_mount(i, j);
							gotoxy(N_ROW + 1, 0);
							printf("                                                                                                         ");
							display();
							gotoxy(N_ROW + 1, 0);
							printf("�������� ��ȯ�߽��ϴ�.");
						}
						else if (key != K_YKEY) {
							gotoxy(N_ROW + 1, 0);
							printf("                                                                                                         ");
							display();
							gotoxy(N_ROW + 1, 0);
							printf("�������� ��ȯ���� �ʾҽ��ϴ�.");
						}
					}
					else {
						int rand_sel = randint(0, 1);
						if (rand_sel == 0) {
							item_mount(i, j);
							gotoxy(N_ROW + 1, 0);
							printf("                                                                                                         ");
							display();
							gotoxy(N_ROW + 1, 0);
							printf("player %d �� �������� ��ȯ�߽��ϴ�.", i);
						}
						else {
							gotoxy(N_ROW + 1, 0);
							printf("                                                                                                         ");
							display();
							gotoxy(N_ROW + 1, 0);
							printf("player %d �� �������� ��ȯ���� �ʾҽ��ϴ�.", i);
						}
					}
				}
				// ������ ������ �� ��ġ���� ����
				num_item--;
				ix[j] = 0; 
				iy[j] = 0;
			}
		}
	}
}

void player_loc_check(void) {
	int key;
	for (int i = 0; i < n_player; i++) {
		for (int j = 0; j < n_player; j++) {
			if (px[i] == px[j] && py[i] == py[j] + 1 || px[i] == px[j] + 1 && py[i] == py[j] || px[i] == px[j] - 1 && py[i] == py[j] || px[i] == px[j] && py[i] == py[j] - 1) {
				if (i == 0) {
					gotoxy(N_ROW + 1, 0);
					printf("player %d ��/�� �����ƽ��ϴ� (1: ��Ż / 2: ȸ�� / 3: ����)", j);
					key = _getch();
				}
				else {
					int tmp = randint(1, 3);
					key = '0' + tmp;
				}
				if (key == '1') {
					//��Ż
					if (player[i].stamina != 0) { // ���¹̳��� 0�� �ƴϸ�
						if (player[i].stamina >= player[j].stamina) { // ���¹̳� ����
							if (player[i].hasitem == true && player[j].hasitem == true) { //�� �� ������ ����
								int tmp = p_num_item[i];
								item_mount(i, p_num_item[j]);
								item_unmount(j);
								item_mount(j, tmp);
								if (player[i].stamina - 40 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 40;
								}
							}
							else if (player[i].hasitem == true && player[j].hasitem == false){ // i�� ������ ����
								if (player[i].stamina - 40 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 40;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == true) { // j�� ������ ����
								item_mount(i, p_num_item[j]);
								item_unmount(j);
								if (player[i].stamina - 40 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 40;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == false) { // �� �� ������ ����
								if (player[i].stamina - 40 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 40;
								}
							}
						}
						else { // ���¹̳� �̽� ����
							if (player[i].stamina - 60 < 0) {
								player[i].stamina = 0;
							}
							else {
								player[i].stamina -= 60;
							}
						}
					}
					else {
						return;
					}
				}
				else if (key == '2') {
					//ȸ��
					if (player[i].stamina != 0) { // ���¹̳��� 0�� �ƴϸ�
						if (player[i].intel >= player[j].intel) { // ���¹̳� ����
							if (player[i].hasitem == true && player[j].hasitem == true) { //�� �� ������ ����
								int tmp = p_num_item[i];
								item_mount(i, p_num_item[j]);
								item_unmount(j);
								item_mount(j, tmp);
								if (player[i].stamina - 20 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 20;
								}
							}
							else if (player[i].hasitem == true && player[j].hasitem == false) { // i�� ������ ����
								if (player[i].stamina - 20 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 20;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == true) { // j�� ������ ����
								item_mount(i, p_num_item[j]);
								item_unmount(j);
								if (player[i].stamina - 20 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 20;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == false) { // �� �� ������ ����
								if (player[i].stamina - 20 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 20;
								}
							}
						}
						else { // ���¹̳� �̽� ����
							if (player[i].stamina - 40 < 0) {
								player[i].stamina = 0;
							}
							else {
								player[i].stamina -= 40;
							}
						}
					}
					else {
						return;
					}
				}
				else if (key == '3') {
					//����
					return;
				}
				else {
					player_loc_check();
				}
			}
		}
	}
	display();
}


void nightgame(void) {
	nightgame_init();
	system("cls");
	display();
	dialog("�� ������ ���۵˴ϴ�");
	while (1) {
		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED && player[0].is_alive == true) {
			nightgame_move_manual(key);
		}
		nightgame_item_loot();
		
		if (tick % 1000 == 0) {
			player_loc_check();
		}

		// player 1 ���ʹ� �������� ������(8����)
		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] == 0 && player[i].is_alive == true) {
				nightgame_move_random(i, -1);
			}
		}

		if (tick % 3000 == 0) {
			gotoxy(N_ROW + 1, 0);
			printf("                                                                                                         ");
		}

		display();
		Sleep(10);
		tick += 10;
		if (num_item <= 0) {
			break;
		}
	}
}
