#define _CRT_SECURE_NO_WARNINGS
/*
명령어 안내 (대소문자 구분, 입력 후 Enter)

j / J : 가로와 세로를 각각 한 칸 축소 / 확대
k / K : 가로를 한 칸 축소 / 확대
l / L : 세로를 한 칸 축소 / 확대
p /P    : 가로 한 칸 축소, 세로 한 칸 확대

[보드 및 기타]
b : 두 도형의 면적 출력
c : 보드의 가로와 세로를 각각 한 칸 확대 (최대 40칸)
v : 보드의 가로와 세로를 각각 한 칸 축소 (최소 20칸)
r : 두 도형의 크기 다시 입력
q : 프로그램 종료
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>

typedef struct Col {
	int board;
}Col;

typedef struct Row {
	Col col[40];
}Row;

Row row[40];

void printboard(int x, int y)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	system("cls");
	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
		{
			if (row[i].col[j].board == 0)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15 * 16 + 15);
				printf(". ");
			}
			else if (row[i].col[j].board == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4 * 16 + 4);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15 * 16 + 15);
			}
			else if (row[i].col[j].board == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1 * 16 + 1);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15 * 16 + 15);
			}
			else if (row[i].col[j].board == 3)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5 * 16 + 5);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15 * 16 + 15);
			}
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		printf("\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

int main()
{
	int x = 30, y = 30;
	printboard(x, y);
	printf("도형 1의 가로 세로를 입력하세요 : ");
	int p1x, p1y, p2x, p2y;
	scanf("%d %d", &p1x, &p1y);
	printf("도형 2의 가로 세로를 입력하세요 : ");
	scanf("%d %d", &p2x, &p2y);
	int player = 1;
	for (int i = 0; i < p1y; i++)
	{
		for (int j = 0; j < p1x; j++)
		{
			row[i].col[j].board += 1;
		}
	}
	for (int i = 0; i < p2y; i++)
	{
		for (int j = 0; j < p2x; j++)
		{
			row[i].col[j].board += 2;
		}
	}
	int p1posx = 0, p1posy = 0, p2posx = 0, p2posy = 0;
	while (1)
	{
		printboard(x, y);
		char command;
		printf("command : ");
		scanf(" %c", &command);
		if (command == 'w')
		{
			if (player == 1)
			{
				for (int i = 0; i < p1y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
					}
				}
				p1posy = (p1posy - 1 + y) % y;
				for (int i = 0; i < p1y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
					}
				}
			}
			else if (player == 2)
			{
				for (int i = 0; i < p2y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
					}
				}
				p2posy = (p2posy - 1 + y) % y;
				for (int i = 0; i < p2y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
					}
				}
			}
		}
		else if (command == 'a')
		{
			if (player == 1)
			{
				for (int i = 0; i < p1y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
					}
				}
				p1posx = (p1posx - 1 + x) % x;
				for (int i = 0; i < p1y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
					}
				}
			}
			else if (player == 2)
			{
				for (int i = 0; i < p2y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
					}
				}
				p2posx = (p2posx - 1 + x) % x;
				for (int i = 0; i < p2y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
					}
				}
			}
		}
		else if (command == 's')
		{
			if (player == 1)
			{
				for (int i = 0; i < p1y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
					}
				}
				p1posy = (p1posy + 1) % y;
				for (int i = 0; i < p1y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
					}
				}
			}
			else if (player == 2)
			{
				for (int i = 0; i < p2y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
					}
				}
				p2posy = (p2posy + 1) % y;
				for (int i = 0; i < p2y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
					}
				}
			}
		}
		else if (command == 'd')
		{
			if (player == 1)
			{
				for (int i = 0; i < p1y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
					}
				}
				p1posx = (p1posx + 1) % x;
				for (int i = 0; i < p1y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p1x; j++)
					{
						row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
					}
				}
			}
			else if (player == 2)
			{
				for (int i = 0; i < p2y; i++)   // 원래 있던거 원래대로
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
					}
				}
				p2posx = (p2posx + 1) % x;
				for (int i = 0; i < p2y; i++) // 이동시킨후 다시 그리기 
				{
					for (int j = 0; j < p2x; j++)
					{
						row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
					}
				}
			}
		}
		else if (command == 'j') //모두 한칸 축소
		{
			if (player == 1)
			{
				if (p1x > 1 && p1y > 1)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1x--;
					p1y--;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2x > 1 && p2y > 1)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2x--;
					p2y--;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'J')
		{
			if (player == 1)
			{
				if (p1x < x && p1y < y)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1x++;
					p1y++;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2x < x && p2y < y)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2x++;
					p2y++;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'k') //x축 한칸 축소
		{
			if (player == 1)
			{
				if (p1x > 1)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1x--;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2x > 1)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2x--;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'K') //x축 한칸 확대
		{
			if (player == 1)
			{
				if (p1x < x)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1x++;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2x < x)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2x++;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'l') //y축 한칸 축소
		{
			if (player == 1)
			{
				if (p1y > 1)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1y--;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2y > 1)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2y--;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'L') //y축 한칸 확대
		{
			if (player == 1)
			{
				if (p1y < y)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1y++;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2y < y)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2y++;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'p') //x한칸 축소 y  한칸 확대
		{
			if (player == 1)
			{
				if (p1y < y && p1x>1)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1x--;
					p1y++;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2y < y && p2x>1)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2x--;
					p2y++;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'P') //x한칸 확대 y 한칸 축소
		{
			if (player == 1)
			{
				if (p1x < x && p1y>1)
				{
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
						}
					}
					p1y--;
					p1x++;
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
				}
			}
			else if (player == 2)
			{
				if (p2x < x && p2y>1)
				{
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board -= 2;
						}
					}
					p2y--;
					p2x++;
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'b')
		{
			printf("도형 1의 면적 : %d / 도형 2의 면적 : %d\n", p1x * p1y, p2x * p2y);
			Sleep(2500);
		}
		else if (command == 'c') //보드칸 x,y 1칸씩 늘리기
		{
			if (x < 40)
			{
				x++;
				y++;
				for (int i = 0; i < 40; i++)
				{
					for (int j = 0; j < 40; j++)
					{
						row[i].col[j].board = 0;
					}
				}
				for (int i = 0; i < p1y; i++)
				{
					for (int j = 0; j < p1x; j++)
					{
						row[i].col[j].board += 1;
					}
				}
				for (int i = 0; i < p2y; i++)
				{
					for (int j = 0; j < p2x; j++)
					{
						row[i].col[j].board += 2;
					}
				}
			}
		}
		else if (command == 'v') //보드칸 x,y 1칸씩 줄이기
		{
			if (x > 20 && y > 20)
			{
				if (p1x < x && p1y < y && p2x < x && p2y < y)
				{
					if (p1posx + p1x >= x && p1posx > 0) p1posx--;
					if (p1posy + p1y >= y && p1posy > 0) p1posy--;
					if (p2posx + p2x >= x && p2posx > 0) p2posx--;
					if (p2posy + p2y >= y && p2posy > 0) p2posy--;
					x--;
					y--;
					for (int i = 0; i < 40; i++)
					{
						for (int j = 0; j < 40; j++)
						{
							row[i].col[j].board = 0;
						}
					}
					for (int i = 0; i < p1y; i++)
					{
						for (int j = 0; j < p1x; j++)
						{
							row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
						}
					}
					for (int i = 0; i < p2y; i++)
					{
						for (int j = 0; j < p2x; j++)
						{
							row[(p2posy + i) % y].col[(p2posx + j) % x].board += 2;
						}
					}
				}
			}
		}
		else if (command == 'r')
		{
			int x = 30, y = 30;
			printboard(x, y);
			printf("도형 1의 가로 세로를 입력하세요 : ");
			int p1x, p1y, p2x, p2y;
			scanf("%d %d", &p1x, &p1y);
			printf("도형 2의 가로 세로를 입력하세요 : ");
			scanf("%d %d", &p2x, &p2y);
			for (int i = 0; i < p1y; i++)
			{
				for (int j = 0; j < p1x; j++)
				{
					row[i].col[j].board += 1;
				}
			}
			for (int i = 0; i < p2y; i++)
			{
				for (int j = 0; j < p2x; j++)
				{
					row[i].col[j].board += 2;
				}
			}
			p1posx = 0, p1posy = 0, p2posx = 0, p2posy = 0;
		}
		else if (command == 'q')
		{
			break;
		}
		else if (command == '1')
		{
			player = 1;
		}
		else if (command == '2')
		{
			player = 2;
		}
	}
}
