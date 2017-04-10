#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <process.h>
#include <ctime>
using namespace std;

/*
 * ȫ������ Globel
 */
// ��ͼ�ߴ�
const int Height = 19;
const int Width = 39;
const int ROW = Height + 2;
const int COL = Width + 2;
// �ٶ�
const int Inteval = 500;	// ����/����Խ��Խ��
volatile bool pause = false;			// �Ƿ���ͣ
// ��������
const char KEY_UP = 'w';
const char KEY_DOWN = 's';
const char KEY_LEFT = 'a';
const char KEY_RIGHT = 'd';
const char KEY_PAUSE = 'p';

// ��ͼ
int map[COL][ROW] = { 0 };
// ��
typedef struct { int x, y; } Point;		
// ����
Point direction;
int MaxLength = 10;			// ����󳤶�
vector<Point> snake(MaxLength);	// ����
int HeadIndex;	// ��ͷ
int TailIndex;	// ��β
int Length;		// �ߵ�ǰ����
// ʳ��
Point food;
int score;

/* �ƶ���ָ���ĵ� */
void gotoxy(int x, int y) {
	// or pos.X=x;pos.Y=y;
	COORD pos = { x,y };									//���ù��λ��
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);			// ��ȡ��׼����豸���
	SetConsoleCursorPosition(hOut, pos);					//���������ֱ���ָ���ĸ����壬ʲôλ��
}
void gotoxy(const Point &p) {
	COORD pos = { p.x, p.y };									//���ù��λ��
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);			// ��ȡ��׼����豸���
	SetConsoleCursorPosition(hOut, pos);					//���������ֱ���ָ���ĸ����壬ʲôλ��
}
inline void drawPoint(const Point &position, char content) {	// ��ĳһ��
	gotoxy(position);
	cout << content;
}
/* ������ͼ����
0 -- you can walk ���ߵĵط�
1 -- wall
2 -- 
*/
/* ��ʼ����ͼ */
void initWall()
{
	for (int i = 0; i < COL; i++)	// �ײ�
	{
		map[i][ROW - 1] = 1;
	}
	for (int i = 0; i < COL; i++)	// ����
	{
		map[i][0] = 1;
	}
	for (int j = 0; j < ROW; j++)	// ���
	{
		map[0][j] = 1;
	}
	for (int j = 0; j < ROW; j++)	// �ұ�
	{
		map[COL - 1][j] = 1;
	}
}
/* ��ʼ���� */
void initSnake() {
	int y = 2;
	for (int i = 0; i < 2; i++) {	// ÿ�����λ��
		Point p;
		p.x = i + 1;
		p.y = y;
		snake[i] = p;
	}
	HeadIndex = 1;						// ������ͷ ����
	TailIndex = 0;						// ��β����
	Length = 2;							// ������
	direction.x = 1; direction.y = 0;	// ���� ->
}

inline bool validX(int value) {
	return (snake[HeadIndex].x + direction.x) == value;
}
inline bool validY(int value) {
	return (snake[HeadIndex].y + direction.y) == value;
}
/* ʧ�ܣ�Խ�磩�ж� */
bool judge() {
	// �߽�
	if (validX(0) || validX(Width) || validY(0) || validY(Height)) {
		// ��
		gotoxy(5, ROW);
		cout << "���ˣ�" << endl;
		return false;
	}
	// ����
	int index = TailIndex; // ��ǰҪ���Ƶ���������
	for (int i = 0; i < Length - 1; i++) {
		if (index >= MaxLength) index = 0; // ��vector���Ƶ�ͷ���ٴ�[0]��ʼ��
		if (validX(snake[index].x) && validY(snake[index].y))	{ // �˴������ú���ָ��/lambda/bind������Ϊ������
			gotoxy(5, ROW);
			cout << "����" << endl;
			return false;
		}
		index++;
	}
	return true;
}
/* Ͷ��ʳ�� */
void feed() {
	// �����������
	// �ж��Ƿ���������
	food.x = 1 + rand() % Width;
	food.y = 1 + rand() % Height;

	int index = TailIndex; // ��ǰ��������
	for (int i = 0; i < Length; i++) {
		if (index >= MaxLength) index = 0; // ��vector������ͷ���ٴ�[0]��ʼ��
		while (food.x == snake[index].x && food.y == snake[index].y) {			// �˴������ú���ָ��/lambda/bind������Ϊ������TODO
			food.x = 1 + rand() % Width;
			food.y = 1 + rand() % Height;
		}
		index++;
	}
	drawPoint(food, 3);
}
void eat() {	// ��
	// ÿ��Ҫ��һ��ʱ��Ҫ���жϣ�����ͷ����ǰ�н�������һ������ʳ��ʱ����ʳ����ӵ�ͷ������Length++,��ͷHeadIndex�ı�,snake vector�ı�
	if (validX(food.x) && validY(food.y)) {
		if (Length >= MaxLength) {
			snake.resize(MaxLength += 10);	// �������ȳ���vector����,���·���vector��С
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

/* ��ʾ��Ϣ */
void printMessage() {
	gotoxy(5, ROW);
	cout << "��ǰ������" << score << endl;
}
void init()	/* ��ʼ�� */
{
	initWall();
	initSnake();
	feed();
	score = 0;	// ������ʼΪ0��
}
/* ��ǽ */
void drawWall() {
	for (int i = 0; i < COL; i++)
	{
		for (int j = 0; j < ROW; j++)
		{
			gotoxy(i, j);

			if (1 == map[i][j])
			{
				cout << '#';	// # ��ǽ
			}
		}
	}
}
/* ���� */
void drawSnake() {
	int index = TailIndex; // ��ǰҪ���Ƶ���������
	for (int i = 0; i < Length; i++) {
		if (index >= MaxLength) index = 0; // ��vector���Ƶ�ͷ���ٴ�[0]��ʼ��
		drawPoint(snake[index], 4);	// �˴������ú���ָ��/lambda/bind������Ϊ������
		index ++;
	}
}

/* �����ɣ�̰���� */
void crawl() {
	eat();
	int temp = HeadIndex;
	// ��ͷ��β������ǰ�ƶ�
	if (++TailIndex >= MaxLength) TailIndex = 0;
	if (++HeadIndex >= MaxLength) HeadIndex = 0;
	// ���һ���ƶ�����ͷǰ��
	snake[HeadIndex].x = snake[temp].x + direction.x;
	snake[HeadIndex].y = snake[temp].y + direction.y;

	printMessage();
}

inline void clearTail() {
	// ��β��ͼ�����
	drawPoint(snake[TailIndex], ' ');
}

/* ���� */
void control(PVOID param) {
	char c;
	while (c = getch()) {	// ������cin��������Ļ��ӡ���������ҷ����ʧЧ
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
		case KEY_PAUSE:		/* ��ͣ */
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
		while (judge() && !pause) {// ��һ��ȫ��bool����������ͣ
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
