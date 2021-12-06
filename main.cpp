//branch master
//555
#include <iostream>
#include <Windows.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

using std::cout;
using std::endl;

constexpr int FOODSHOW = 0; //食物球显示标记
constexpr int FOODHIDE = 1; //食物球隐藏标记
constexpr int ENEMY_ALIVE = 0; //敌球生存标记
constexpr int ENEMY_DEAD = 1; //敌球死亡标记
constexpr int SCREEN_WIDTH = 1280; //窗口宽度
constexpr int SCREEN_HEIGHT = 760; //窗口高度
constexpr int MAP_WIDTH = 5000; //地图宽度
constexpr int MAP_HEIGHT = 5000; //地图高度
constexpr int PLAYER_POSX = 640; //玩家在窗口客户区的横坐标（默认为中间）
constexpr int PLAYER_POSY = 380; //玩家在窗口客户区的纵坐标（默认为中间）
constexpr int ENEMY_BALL_NUM = 10; //敌球的数量

int g_posxdif = 0, g_posydif = 0; //全局对象坐标偏移量
int g_playerRadius = 20; //玩家球体半径
int g_playerPosxOnMap = 2500, g_playerPosyOnMap = 2500; //玩家球体位于地图的坐标
int g_moveSpeed = 3; //移动速度
int g_foodBallNum = 10; //食物球的数量

typedef struct tagFoodBall {
	int posx, posy;
	int radius;
	COLORREF color;
	int flag;
} FoodBall, * PtrFoodBall;

typedef struct tagEnemyBall {
	int posx, posy;
	int radius;
	COLORREF color;
	int flag;
} EnemyBall, * PtrEnemyBall;

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
			g_posxdif = g_moveSpeed;
		}
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			g_posydif = g_moveSpeed;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			g_posxdif = -g_moveSpeed;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			g_posydif = -g_moveSpeed;
		}

		//state changing area
		MoveFood(pFoodBall, g_foodBallNum);

		//collision detection area
		EatFood(pFoodBall, g_foodBallNum);


		ShowFood(pFoodBall, g_foodBallNum);
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
	pFoodBall = new FoodBall[g_foodBallNum];

	for (int index = 0; index < g_foodBallNum; ++index) {
		pFoodBall[index] = { rand() % 1280, rand() % 760, rand() % 15 + 5, RGB(rand() % 255, rand() % 255, rand() % 255), FOODSHOW };
	}

	return pFoodBall;
}
