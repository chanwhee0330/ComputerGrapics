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
	system("cls");
	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
		{
			if (row[i].col[j].board == 0)
			{
				printf(". ");
			}
			else if (row[i].col[j].board == 1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
			else if (row[i].col[j].board == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
			else if(row[i].col[j].board==3)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
				printf("* ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
		}
		printf("\n");
	}
}

int main()
{
	int x = 30, y = 30;
	printboard(30, 30);
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

		}
		else if (command == 'a')
		{

		}
		else if (command == 's')
		{

		}
		else if (command == 'd')
		{
			p1posx++;
			for (int i = 0; i < p1y; i++)   // 원래 있던거 원래대로
			{
				for (int j = 0; j < p1x; j++)
				{
					row[i].col[j].board -= 1;
				}
			}
			for (int i = 0; i < p1y; i++) // 이동시킨후 다시 그리기 
			{
				for (int j = p1posx; j < p1x%x; j++)
				{
					row[i].col[j].board += 1;
				}
			}
		}
	}
}