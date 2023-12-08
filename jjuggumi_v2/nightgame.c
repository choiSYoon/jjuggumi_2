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

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX], p_num_item[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기, 소지한 아이템 번호
int ix[ITEM_MAX], iy[ITEM_MAX], item_type[ITEM_MAX]; // 각 아이템 위치, 아이템 종류
int num_item = 0;

void nightgame_init(void) {
	for (int i = 0; i < n_player; i++) {
		player_clear[i] = false;
		stm_heal(i);
	}
	map_init(8, 23);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		// 같은 자리가 나오면 다시 생성
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
		// 같은 자리가 나오면 다시 생성
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

	nightgame_move_tail(0, nx, ny);
}

// 0 <= dir < 4가 아니면 랜덤
void nightgame_move_random(int player, int dir) {
	int p = player;  // 이름이 길어서...
	int nx, ny;  // 움직여서 다음에 놓일 자리

	// 움직일 공간이 없는 경우는 없다고 가정(무한 루프에 빠짐)	

	do {
		// 아이템과 (아이템을 가진 플레이어)들 중, 가장 가까운 곳을 향해서 이동
		nx = px[p] + randint(-1, 1);
		ny = py[p] + randint(-1, 1);
	} while (!placable(nx, ny));

	nightgame_move_tail(p, nx, ny);
}

// back_buf[][]에 기록
void nightgame_move_tail(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void item_mount(int player1, int num_item) {
	int p = player1;  // 이름이 길어서...
	player[p].hasitem = true;
	p_num_item[p] = num_item;
	player[p].item.intel_buf = item[num_item].intel_buf;
	player[p].item.stamina_buf = item[num_item].stamina_buf;
	player[p].item.str_buf = item[num_item].str_buf;
}

void item_unmount(int player1) {
	int p = player1;  // 이름이 길어서...
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
				printf("아이템 종류: %d / x: %d / y: %d", item_type[j], ix[j], iy[j]);
				
				if (player[i].hasitem == false) {
					item_mount(i, j);
				}
				else {
					if (i == 0) {
						display();
						gotoxy(N_ROW + 1, 0);
						printf("아이템을 교환하시겠습니까? (Y/N)");
						int key = _getch();
						if (key == K_YKEY) {
							item_mount(i, j);
							gotoxy(N_ROW + 1, 0);
							printf("                                                                                                         ");
							display();
							gotoxy(N_ROW + 1, 0);
							printf("아이템을 교환했습니다.");
						}
						else if (key != K_YKEY) {
							gotoxy(N_ROW + 1, 0);
							printf("                                                                                                         ");
							display();
							gotoxy(N_ROW + 1, 0);
							printf("아이템을 교환하지 않았습니다.");
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
							printf("player %d 가 아이템을 교환했습니다.", i);
						}
						else {
							gotoxy(N_ROW + 1, 0);
							printf("                                                                                                         ");
							display();
							gotoxy(N_ROW + 1, 0);
							printf("player %d 가 아이템을 교환하지 않았습니다.", i);
						}
					}
				}
				// 아이템 먹으면 그 위치에서 제거
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
					printf("player %d 을/를 마주쳤습니다 (1: 강탈 / 2: 회유 / 3: 무시)", j);
					key = _getch();
				}
				else {
					int tmp = randint(1, 3);
					key = '0' + tmp;
				}
				if (key == '1') {
					//강탈
					if (player[i].stamina != 0) { // 스태미나가 0만 아니면
						if (player[i].stamina >= player[j].stamina) { // 스태미나 많음
							if (player[i].hasitem == true && player[j].hasitem == true) { //둘 다 아이템 소지
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
							else if (player[i].hasitem == true && player[j].hasitem == false){ // i만 아이템 소지
								if (player[i].stamina - 40 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 40;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == true) { // j만 아이템 소지
								item_mount(i, p_num_item[j]);
								item_unmount(j);
								if (player[i].stamina - 40 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 40;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == false) { // 둘 다 아이템 없음
								if (player[i].stamina - 40 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 40;
								}
							}
						}
						else { // 스태미나 이슈 실패
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
					//회유
					if (player[i].stamina != 0) { // 스태미나가 0만 아니면
						if (player[i].intel >= player[j].intel) { // 스태미나 많음
							if (player[i].hasitem == true && player[j].hasitem == true) { //둘 다 아이템 소지
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
							else if (player[i].hasitem == true && player[j].hasitem == false) { // i만 아이템 소지
								if (player[i].stamina - 20 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 20;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == true) { // j만 아이템 소지
								item_mount(i, p_num_item[j]);
								item_unmount(j);
								if (player[i].stamina - 20 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 20;
								}
							}
							else if (player[i].hasitem == false && player[j].hasitem == false) { // 둘 다 아이템 없음
								if (player[i].stamina - 20 < 0) {
									player[i].stamina = 0;
								}
								else {
									player[i].stamina -= 20;
								}
							}
						}
						else { // 스태미나 이슈 실패
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
					//무시
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
	dialog("곧 게임이 시작됩니다");
	while (1) {
		// player 0만 손으로 움직임(4방향)
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

		// player 1 부터는 랜덤으로 움직임(8방향)
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
