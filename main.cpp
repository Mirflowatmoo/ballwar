//branch master

#include <iostream>
#include <Windows.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

#define CLOSE_CONSOLE 0 //关闭控制台 
#define OPEN_CONSOLE 1 //打开控制台
#define FOODSHOW 0 //食物球显示标记
#define FOODHIDE 1 //食物球隐藏标记
#define ENEMY_ALIVE 0 //敌球生存标记
#define ENEMY_DEAD 1 //敌球死亡标记
#define SCREEN_WIDTH 1280 //窗口宽度
#define SCREEN_HEIGHT 760 //窗口高度
#define MAP_WIDTH 5000 //地图宽度
#define MAP_HEIGHT 5000 //地图高度
#define PLAYER_POSX 640 //玩家在窗口客户区的横坐标（默认为中间）
#define PLAYER_POSY 380 //玩家在窗口客户区的纵坐标（默认为中间）
#define ENEMY_BALL_NUM 10 //敌球的数量
#define FRAMES_60 15 //1000 / 60 = 16, 16 - 1 = 15

using std::cout;
using std::endl;

int g_foodBallNum = 200; //食物球的数量
float g_zoomCoefficient = 1.0f; //缩放系数
float g_zoomCoefficientLock = 1.0f; //缩放阈值锁
float g_moveSpeed = 3.0f; //移动速度
float g_playerRadius = 0.0f; //玩家球体半径
float g_playerWeight = 20.0f; //玩家球体重量
float g_posxdif = 0.0f, g_posydif = 0.0f; //全局对象坐标偏移量
float g_playerPosxOnMap = MAP_WIDTH / 2, g_playerPosyOnMap = MAP_HEIGHT / 2; //玩家球体位于地图的坐标

typedef struct tagFoodBall {
	float posx, posy;
	float radius;
	float weight;
	COLORREF color;
	int flag;
} FoodBall, * PtrFoodBall;

typedef struct tagEnemyBall {
	float posx, posy;
	float radius;
	float weight;
	float speed;
	COLORREF color;
	tagEnemyBall* pNextSonBall;
} EnemyBall, * PtrEnemyBall;

//游戏初始化
FoodBall* GameInitFoodBall();

void GameInitPlayerBall();

//输入反馈
void InputResponse();

//缩放状态更新
void UpdateZoom();

//玩家状态更新
void UpdatePlayerState();

//绘制地图
void ShowMap();

//绘制食物球
void ShowFood(FoodBall* foodBall, int n);

//移动食物球
void MoveFood(FoodBall* foodBall, int n);

//碰撞检测，吞噬判定
void EatFood(FoodBall* foodBall, int n);

//体重转换速度
float WeightToSpeed(float weight);

//体重转换半径
float WeightToRadius(float weight);

//半径转换体重
float RadiusToWeight(float radius);

int main() {

	srand(unsigned(time(NULL)));
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT, CLOSE_CONSOLE);
	setbkcolor(RGB(50, 50, 50));
	setbkmode(TRANSPARENT);

	FoodBall* pFoodBall = GameInitFoodBall();
	GameInitPlayerBall();

	BeginBatchDraw();
	while (1) {

		//状态获取
		DWORD beginTime = GetTickCount();
		
		//输入反馈
		InputResponse();

		//状态更新
		UpdatePlayerState();
		MoveFood(pFoodBall, g_foodBallNum);
		UpdateZoom();

		//碰撞检测与交互
		EatFood(pFoodBall, g_foodBallNum);

		//绘制
		ShowFood(pFoodBall, g_foodBallNum);
		setfillcolor(RGB(150, 150, rand() % 255));
		solidcircle(PLAYER_POSX, PLAYER_POSY, g_playerRadius / g_zoomCoefficient);
		char str[80];
		sprintf_s(str, "体重: %f g  半径: %f pixel 速度: %f pixel/frame", g_playerWeight, g_playerRadius, g_moveSpeed);
		settextcolor(WHITE);
		outtextxy(30, 30, str);
		ShowMap();

		FlushBatchDraw();

		while (GetTickCount() - beginTime < FRAMES_60) {
			Sleep(1);
		};
		//cout << GetTickCount() - beginTime << endl;
		cleardevice();
	}

	EndBatchDraw();

	delete[] pFoodBall;
	closegraph();
	return 0;
}


FoodBall* GameInitFoodBall() {

	FoodBall* pFoodBall = nullptr;
	pFoodBall = new FoodBall[g_foodBallNum];

	for (int index = 0; index < g_foodBallNum; ++index) {
		int tempRadius = rand() % 15 + 5;
		pFoodBall[index] = { 
			float(rand() % 2560 - 1280), 
			float(rand() % 1520 - 760), 
			(float)tempRadius,
			RadiusToWeight((float)tempRadius),
			RGB(rand() % 255, rand() % 255, rand() % 255), 
			FOODSHOW 
		};
	}

	return pFoodBall;
}

void GameInitPlayerBall() {
	g_playerRadius = 20.0f;
	g_playerWeight = RadiusToWeight(20.0f);
	g_moveSpeed = WeightToSpeed(g_playerWeight);
}

void InputResponse() {
	g_posxdif = g_posydif = 0;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		g_playerWeight += 5000;
	}

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
}

void UpdateZoom() {

	if (g_zoomCoefficient < g_zoomCoefficientLock) {
		g_zoomCoefficient += 0.01f;
	}

	if (g_playerRadius > 60.0f && g_zoomCoefficientLock < 1.01f) {
		g_zoomCoefficientLock = 1.8f;
		g_moveSpeed /= g_zoomCoefficientLock;
	}
	else if (g_playerRadius > 120.0f && g_zoomCoefficientLock < 1.81f) {
		g_zoomCoefficientLock = 2.2f;
		g_moveSpeed = g_moveSpeed * 1.8 / g_zoomCoefficientLock;
	}
	else if (g_playerRadius > 250.0f && g_zoomCoefficientLock < 2.21f) {
		g_zoomCoefficientLock = 2.8f;
		g_moveSpeed = g_moveSpeed * 2.2 / g_zoomCoefficientLock;
	}
	else if (g_playerRadius > 500.0f && g_zoomCoefficientLock < 2.81f) {
		g_zoomCoefficientLock = 5.0f;
		g_moveSpeed = g_moveSpeed * 2.8f / g_zoomCoefficientLock;
	}
	else if (g_playerRadius > 800.0f && g_zoomCoefficientLock < 5.01f) {
		g_zoomCoefficientLock = 7.5f;
		g_moveSpeed = g_moveSpeed * 5.0f / g_zoomCoefficientLock;
	}
	
}

void UpdatePlayerState() {
	g_playerPosxOnMap += g_posxdif;
	g_playerPosyOnMap -= g_posydif;
	g_playerRadius = WeightToRadius(g_playerWeight);
	g_moveSpeed = WeightToSpeed(g_playerWeight) / g_zoomCoefficient;
}

void ShowMap()
{
	int b = 20;
	setfillcolor(RGB(50, 255, 0));
	solidrectangle(SCREEN_WIDTH - MAP_WIDTH / b, 0, MAP_WIDTH, MAP_HEIGHT / b);
	setfillcolor(RGB(50, 0, 225));
	solidrectangle(SCREEN_WIDTH - g_playerPosxOnMap / b - SCREEN_WIDTH * g_zoomCoefficient / b / 2, (g_playerPosyOnMap / b - SCREEN_HEIGHT * g_zoomCoefficient / b / 2),
		SCREEN_WIDTH - g_playerPosxOnMap / b + SCREEN_WIDTH * g_zoomCoefficient / b / 2, (g_playerPosyOnMap / b + SCREEN_HEIGHT * g_zoomCoefficient / b / 2));
	setfillcolor(RED);

	solidcircle(SCREEN_WIDTH - g_playerPosxOnMap / b, g_playerPosyOnMap / b, 5);

}


void ShowFood(FoodBall* foodBall, int n) {
	for (int index = 0; index < n; ++index) {
		setfillcolor(foodBall[index].color);
		solidcircle(PLAYER_POSX + int((foodBall[index].posx - PLAYER_POSX) / g_zoomCoefficient),
			PLAYER_POSY + int((foodBall[index].posy - PLAYER_POSY) / g_zoomCoefficient), 
			int(foodBall[index].radius / g_zoomCoefficient));
	}
}

void MoveFood(FoodBall* foodBall, int n) {
	for (int index = 0; index < n; ++index) {
		foodBall[index].posx += (rand() % 5 - 2 + g_posxdif);
		foodBall[index].posy += (rand() % 5 - 2 + g_posydif);
	}
}

void EatFood(FoodBall* foodBall, int n) {

	for (int index = 0; index < n; ++index) {

		if (g_playerRadius < foodBall[index].radius) {
			continue;
		}

		float fposx = foodBall[index].posx;
		float fposy = foodBall[index].posy;
		float fradius = foodBall[index].radius;
		float fweight = foodBall[index].weight;
		float radiusDif = abs(g_playerRadius - fradius);
		float posDif = sqrt(pow(abs(fposx - PLAYER_POSX), 2) + pow(abs(fposy - PLAYER_POSY), 2));

		if (posDif < radiusDif) {
			g_playerWeight += fweight;
			foodBall[index].posx = float(rand() % 2560);
			foodBall[index].posy = float(rand() % 1520);
			foodBall[index].radius = float(rand() % 15 + 5);
		}
	}
}

float WeightToSpeed(float weight) {
	weight = sqrt(weight);
	weight = sqrt(weight);
	weight = sqrt(weight);
	return 25 / weight;
}

float WeightToRadius(float weight) {
	return sqrt(weight / 3.14159f);
}

float RadiusToWeight(float radius) {
	return radius * radius * 3.14159f;
}

