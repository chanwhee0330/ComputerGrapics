#define _CRT_SECURE_NO_WARNINGS
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

void printboard(int x,int y)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	system("cls");
	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
		{
			if (row[i].col[j].board == 0)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15*16 + 15);
				printf(". ");
			}
			else if (row[i].col[j].board == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4*16+4);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15 * 16 + 15);
			}
			else if (row[i].col[j].board == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1 * 16 + 1);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15 * 16 + 15);
			}
			else if(row[i].col[j].board==3)
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
	int p1posx=0, p1posy=0, p2posx=0, p2posy=0;
	while (1)
	{
		printboard(x, y);
		char command;
		printf("command : ");
		scanf(" %c", &command);
		if (command == 'w')
		{
			for (int i = 0; i < p1y; i++)   // 원래 있던거 원래대로
			{
				for (int j = 0; j < p1x; j++)
				{
					row[(p1posy + i) % y].col[(p1posx + j) % x].board -= 1;
				}
			}
			p1posy = (p1posy - 1+y) % y;
			for (int i = 0; i < p1y; i++) // 이동시킨후 다시 그리기 
			{
				for (int j = 0; j < p1x; j++)
				{
					row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
				}
			}
		}
		else if (command == 'a')
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
		else if (command == 's')
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
		else if (command == 'd')
		{
			for (int i = 0; i < p1y; i++)   // 원래 있던거 원래대로
			{
				for (int j = 0; j < p1x; j++)
				{
					row[(p1posy+i)%y].col[(p1posx+j)%x].board -= 1;
				}
			}
			p1posx=(p1posx+1)%x;
			for (int i = 0; i < p1y; i++) // 이동시킨후 다시 그리기 
			{
				for (int j = 0; j < p1x; j++)
				{
					row[(p1posy + i) % y].col[(p1posx + j) % x].board += 1;
				}
			}
		}
		else if (command == 'j') //모두 한칸 축소
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
		else if (command == 'J')
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
		else if (command == 'k') //x축 한칸 축소
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
		else if (command == 'K') //x축 한칸 확대
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
		else if (command == 'l') //y축 한칸 축소
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
		else if (command == 'L') //y축 한칸 확대
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
		else if (command == 'p') //x한칸 축소 y  한칸 확대
		{
			if (p1y < y&&p1x>1)
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
		else if (command == 'P') //x한칸 확대 y 한칸 축소
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
		else if (command == 'b')
		{
			printf("도형 1의 면적 : %d / 도형 2의 면적 : %d\n", p1x* p1y, p2x * p2y);
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
			if (x > 20)
			{
				if (p1x < x && p1y < y && p2x < x && p2y < y)
				{
					x--;
					y--;
					p1posx = 0, p1posy = 0, p2posx = 0, p2posy = 0;
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
		}
	}
}