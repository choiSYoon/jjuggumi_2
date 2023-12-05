#pragma once
#ifndef _JJUGGUMI_H_
#define _JJUGGUMI_H_

#include <Windows.h>
#include <stdbool.h>

#define PLAYER_MAX		7
#define ITEM_MAX       10

typedef struct {
	char name[100];
	int intel_buf, str_buf, stamina_buf;
} ITEM;

ITEM item[ITEM_MAX];

typedef struct {
	int id;
	char name[100];

	// �ɷ�ġ: ����, ��, ���¹̳�
	int intel, str, stamina;

	// ���� ����
	bool is_alive; // �⺻�� true, Ż���ϸ� false
	bool hasitem; // �������� ������ true
	ITEM item; // ������ 1�� ���� ����
} PLAYER;

PLAYER player[PLAYER_MAX];

bool player_clear[PLAYER_MAX]; // �⺻�� true, Ż���ϸ� false
bool player_pause; // �⺻�� true, �̴ϰ����� �������� ������ false

int n_player, n_alive, n_item;

int tick;  // �ð�

// �̴ϰ���
//void sample(void);
//void mugunghwa(void);
//void mugunghwa(void);
//void nightgame(void);
//void juldarigi(void);
//void jebi(void);

int randint(int low, int high);

#endif
