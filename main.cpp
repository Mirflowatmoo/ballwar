//branch master

#include <iostream>
#include <Windows.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

using std::cout;
using std::endl;

constexpr int FOODSHOW = 0;
constexpr int FOODHIDE = 1;
constexpr int MOVESPEED = 3;
constexpr int FOODNUM = 10;
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 760;
constexpr int MAP_WIDTH = 5000;
constexpr int MAP_HEIGHT = 5000;
constexpr int PLAYER_POSX = 640;
constexpr int PLAYER_POSY = 380;
int g_posxdif = 0, g_posydif = 0, g_playerRadius = 20;
int g_playerPosxOnMap = 0, g_playerPosyOnMap = 0;

typedef struct tagFoodBall {
	int posx;
	int posy;
	int radius;
	COLORREF color;
	int flag;
} FoodBall, * PtrFoodBall;

void ShowFood(FoodBall* foodBall, int n);

void MoveFood(FoodBall* foodBall, int n);

void EatFood(FoodBall* foodBall, int n);

FoodBall* GameInitFoodBall();

int main() {

	srand(unsigned(time(NULL)));
	initgraph(1280, 760, 1);
	setbkcolor(WHITE);

	FoodBall* pFoodBall = GameInitFoodBall();

	BeginBatchDraw();
	while (1) {
		DWORD beginTime = GetTickCount();
		//input return area
		g_posxdif = g_posydif = 0;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			g_posxdif = MOVESPEED;
		}
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			g_posydif = MOVESPEED;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			g_posxdif = -MOVESPEED;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			g_posydif = -MOVESPEED;
		}

		//state changing area
		MoveFood(pFoodBall, FOODNUM);

		//collision detection area
		EatFood(pFoodBall, FOODNUM);



		ShowFood(pFoodBall, FOODNUM);
		setfillcolor(RED);
		solidcircle(PLAYER_POSX, PLAYER_POSY, g_playerRadius);

		FlushBatchDraw();

		while (GetTickCount() - beginTime < 15) {
			Sleep(1);
		};
		cout << GetTickCount() - beginTime << endl;
		cleardevice();

	}
	EndBatchDraw();

	delete[] pFoodBall;
	closegraph();
	return 0;
}

void ShowFood(FoodBall* foodBall, int n) {
	for (int index = 0; index < n; ++index) {
		setfillcolor(foodBall[index].color);
		solidcircle(foodBall[index].posx, foodBall[index].posy, foodBall[index].radius);
	}
}

void MoveFood(FoodBall* foodBall, int n) {
	for (int index = 0; index < n; ++index) {
		foodBall[index].posx += (rand() % 5 - 2 + g_posxdif);
		foodBall[index].posy += (rand() % 5 - 2 + g_posydif);
	}
}

void EatFood(FoodBall* foodBall, int n) {
	int judgeValue = g_playerRadius >> 1;

	for (int index = 0; index < n; ++index) {
		int fposx = foodBall[index].posx;
		int fposy = foodBall[index].posy;
		int fradius = foodBall[index].radius;


		if (abs(PLAYER_POSX - fposx) <= judgeValue && abs(PLAYER_POSY - fposy) <= judgeValue) {
			if (g_playerRadius < fradius) {
				continue;
			}
			g_playerRadius += (fradius >> 2);
			foodBall[index].posx = rand() % 2560;
			foodBall[index].posy = rand() % 1520;
			foodBall[index].radius = rand() % 15 + 5;
		}
	}
}

FoodBall* GameInitFoodBall() {

	FoodBall* pFoodBall = nullptr;
	pFoodBall = new FoodBall[FOODNUM];

	for (int index = 0; index < FOODNUM; ++index) {
		pFoodBall[index] = { rand() % 1280, rand() % 760, rand() % 15 + 5, RGB(rand() % 255, rand() % 255, rand() % 255), FOODSHOW };
	}

	return pFoodBall;
}
