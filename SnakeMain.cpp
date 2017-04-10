#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <process.h>
#include <ctime>
using namespace std;

/*
 * 全局数据 Globel
 */
// 地图尺寸
const int Height = 19;
const int Width = 39;
const int ROW = Height + 2;
const int COL = Width + 2;
// 速度
const int Inteval = 500;	// 毫秒/步，越大越慢
volatile bool pause = false;			// 是否暂停
// 按键控制
const char KEY_UP = 'w';
const char KEY_DOWN = 's';
const char KEY_LEFT = 'a';
const char KEY_RIGHT = 'd';
const char KEY_PAUSE = 'p';

// 地图
int map[COL][ROW] = { 0 };
// 点
typedef struct { int x, y; } Point;		
// 方向
Point direction;
int MaxLength = 10;			// 蛇最大长度
vector<Point> snake(MaxLength);	// 蛇身
int HeadIndex;	// 蛇头
int TailIndex;	// 蛇尾
int Length;		// 蛇当前长度
// 食物
Point food;
int score;

/* 移动到指定的点 */
void gotoxy(int x, int y) {
	// or pos.X=x;pos.Y=y;
	COORD pos = { x,y };									//设置光标位置
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);			// 获取标准输出设备句柄
	SetConsoleCursorPosition(hOut, pos);					//两个参数分别是指定哪个窗体，什么位置
}
void gotoxy(const Point &p) {
	COORD pos = { p.x, p.y };									//设置光标位置
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);			// 获取标准输出设备句柄
	SetConsoleCursorPosition(hOut, pos);					//两个参数分别是指定哪个窗体，什么位置
}
inline void drawPoint(const Point &position, char content) {	// 画某一点
	gotoxy(position);
	cout << content;
}
/* 声明地图数组
0 -- you can walk 能走的地方
1 -- wall
2 -- 
*/
/* 初始化地图 */
void initWall()
{
	for (int i = 0; i < COL; i++)	// 底部
	{
		map[i][ROW - 1] = 1;
	}
	for (int i = 0; i < COL; i++)	// 顶部
	{
		map[i][0] = 1;
	}
	for (int j = 0; j < ROW; j++)	// 左边
	{
		map[0][j] = 1;
	}
	for (int j = 0; j < ROW; j++)	// 右边
	{
		map[COL - 1][j] = 1;
	}
}
/* 初始化蛇 */
void initSnake() {
	int y = 2;
	for (int i = 0; i < 2; i++) {	// 每个点的位置
		Point p;
		p.x = i + 1;
		p.y = y;
		snake[i] = p;
	}
	HeadIndex = 1;						// 赋予蛇头 索引
	TailIndex = 0;						// 蛇尾索引
	Length = 2;							// 蛇身长度
	direction.x = 1; direction.y = 0;	// 方向 ->
}

inline bool validX(int value) {
	return (snake[HeadIndex].x + direction.x) == value;
}
inline bool validY(int value) {
	return (snake[HeadIndex].y + direction.y) == value;
}
/* 失败（越界）判断 */
bool judge() {
	// 边界
	if (validX(0) || validX(Width) || validY(0) || validY(Height)) {
		// 输
		gotoxy(5, ROW);
		cout << "输了！" << endl;
		return false;
	}
	// 自身
	int index = TailIndex; // 当前要绘制的蛇身索引
	for (int i = 0; i < Length - 1; i++) {
		if (index >= MaxLength) index = 0; // 若vector绘制到头，再从[0]开始画
		if (validX(snake[index].x) && validY(snake[index].y))	{ // 此处可以用函数指针/lambda/bind定义行为。。。
			gotoxy(5, ROW);
			cout << "输了" << endl;
			return false;
		}
		index++;
	}
	return true;
}
/* 投放食物 */
void feed() {
	// 随机生成坐标
	// 判断是否在蛇身上
	food.x = 1 + rand() % Width;
	food.y = 1 + rand() % Height;

	int index = TailIndex; // 当前蛇身索引
	for (int i = 0; i < Length; i++) {
		if (index >= MaxLength) index = 0; // 若vector遍历到头，再从[0]开始画
		while (food.x == snake[index].x && food.y == snake[index].y) {			// 此处可以用函数指针/lambda/bind定义行为。。。TODO
			food.x = 1 + rand() % Width;
			food.y = 1 + rand() % Height;
		}
		index++;
	}
	drawPoint(food, 3);
}
void eat() {	// 吃
	// 每将要走一步时都要做判断，当蛇头按当前行进方向下一个就是食物时，将食物添加到头，长度Length++,蛇头HeadIndex改变,snake vector改变
	if (validX(food.x) && validY(food.y)) {
		if (Length >= MaxLength) {
			snake.resize(MaxLength += 10);	// 若蛇身长度超过vector长度,重新分配vector大小
		}
		if (HeadIndex < MaxLength - 1) {
			++HeadIndex;
		}
		else {
			HeadIndex = 0;
		}
		snake[HeadIndex].x = food.x;
		snake[HeadIndex].y = food.y;
		Length++;
		score++;
		feed();
	}
}

/* 提示信息 */
void printMessage() {
	gotoxy(5, ROW);
	cout << "当前分数：" << score << endl;
}
void init()	/* 初始化 */
{
	initWall();
	initSnake();
	feed();
	score = 0;	// 分数初始为0；
}
/* 画墙 */
void drawWall() {
	for (int i = 0; i < COL; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			gotoxy(i, j);

			if (1 == map[i][j])
			{
				cout << '#';	// # 是墙
			}
		}
	}
}
/* 画蛇 */
void drawSnake() {
	int index = TailIndex; // 当前要绘制的蛇身索引
	for (int i = 0; i < Length; i++) {
		if (index >= MaxLength) index = 0; // 若vector绘制到头，再从[0]开始画
		drawPoint(snake[index], 4);	// 此处可以用函数指针/lambda/bind定义行为。。。
		index ++;
	}
}

/* 翻滚吧，贪吃蛇 */
void crawl() {
	eat();
	int temp = HeadIndex;
	// 蛇头蛇尾索引向前移动
	if (++TailIndex >= MaxLength) TailIndex = 0;
	if (++HeadIndex >= MaxLength) HeadIndex = 0;
	// 最后一节移动到蛇头前面
	snake[HeadIndex].x = snake[temp].x + direction.x;
	snake[HeadIndex].y = snake[temp].y + direction.y;

	printMessage();
}

inline void clearTail() {
	// 将尾部图案清除
	drawPoint(snake[TailIndex], ' ');
}

/* 操作 */
void control(PVOID param) {
	char c;
	while (c = getch()) {	// 不能用cin，会在屏幕打印出来，并且方向会失效
		switch (c) {
		case KEY_UP:
			if (direction.y != 1 && direction.y != -1) {
				direction.x = 0;
				direction.y = -1;
			}
			break;
		case KEY_DOWN:
			if (direction.y != 1 && direction.y != -1) {
				direction.x = 0;
				direction.y = 1;
			}
			break;
		case KEY_LEFT:
			if (direction.x != 1 && direction.x != -1) {
				direction.x = -1;
				direction.y = 0;
			}
			break;
		case KEY_RIGHT:
			if (direction.x != 1 && direction.x != -1) {
				direction.x = 1;
				direction.y = 0;
			}
			break;
		case KEY_PAUSE:		/* 暂停 */
			pause = !pause;
			break;
		}
	}
}
int main() {
	srand(time(0));
	init();
	drawWall();
	_beginthread(control, 0, 0);

	while (1) {
		while (judge() && !pause) {// 设一个全局bool变量控制暂停
			drawSnake();
			Sleep(Inteval);
			clearTail();
			crawl();
		}
	}
}
//for (char c = 1; c < 127; c++) {// 4: body 3: heart
//	cout << c << " " << endl;
//}
