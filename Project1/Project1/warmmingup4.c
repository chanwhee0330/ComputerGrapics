#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

typedef struct Row {
	char card;
	bool isalive, isfound, check;
}Row;

typedef struct Col {
	Row row[7];
}Col;

Col col[6];

void printBoard(int x, int y)
{
	printf("   ");

	for (int j = 0; j < x; j++) //위에 abcd만들어 주는거
	{
		printf("%c  ", 'a' + j);
	}

	printf("\n");

	for (int i = 0; i < y; i++)
	{
		printf("%d  ", i + 1); // 옆에 1234

		for (int j = 0; j < x; j++)
		{
			if (col[i].row[j].isfound == true)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				if (col[i].row[j].card == '@')
					printf("★  ");
				else
					printf("%c  ", col[i].row[j].card - 32);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
			else if (col[i].row[j].check == true)
			{
				printf("%c  ", col[i].row[j].card);
			}
			else
				printf("*  ");
		}
		printf("\n");
	}


}

int main()
{
	srand(time(NULL));
	char cards[36];
	printf("가로 세로를 입력하세요 : ");
	int x, y,score=0,attemp;
	scanf("%d %d", &x, &y);
	attemp = (x * y) / 2 + 1;
	if (x > 2 && x < 7 && y>2 && y < 7)
	{
		int pair = (x * y) / 2;
		int index = 0;

		for (int i = 0;i < pair;i++)
		{
			cards[index++] = 'a' + i;
			cards[index++] = 'a' + i;
		}

		if ((x * y) % 2 != 0)
		{
			cards[index++] = '@';
		}

		for (int i = (x * y) - 1;i > 0;i--)
		{
			int r = rand() % (i + 1);
			char temp = cards[i];
			cards[i] = cards[r];
			cards[r] = temp;
		}

		index = 0;

		for (int i = 0;i < y;i++)
		{
			for (int j = 0;j < x;j++)
			{
				col[i].row[j].isalive = true;
				col[i].row[j].isfound = false;
				col[i].row[j].check = false;
				col[i].row[j].card = cards[index++];
			}
		}

		printBoard(x, y);

		while (1)
		{
			system("cls");
			printBoard(x, y);
			int found = 0;

			for (int i = 0; i < y; i++)
			{
				for (int j = 0; j < x; j++)
				{
					if (col[i].row[j].isfound == true)
						found++;
				}
			}

			if (found == x * y)
			{
				printf("GAME CLEAR! 점수 : %d", score);
				break;
			}
			char command[20];
			if (attemp <= 0)
			{
				printf("GAME OVER! 점수 : %d",score);
				break;
			}

			printf("명령어를 입력하세요 (남은 기회 : %d / 점수 : %d ) : ", attemp,score);
			scanf(" %[^\n]", command);


			if (command[0] == 'r' && command[1] == '\0')
			{
				char cards[36];
				printf("가로 세로를 입력하세요 : ");
				score = 0;
				attemp = (x * y) / 2 + 1;
				scanf("%d %d", &x, &y);

				if (x > 2 && x < 7 && y>2 && y < 7)
				{
					int pair = (x * y) / 2;
					int index = 0;

					for (int i = 0;i < pair;i++)
					{
						cards[index++] = 'a' + i;
						cards[index++] = 'a' + i;
					}

					if ((x * y) % 2 != 0)
					{
						cards[index++] = '@';
					}

					for (int i = (x * y) - 1;i > 0;i--)
					{
						int r = rand() % (i + 1);
						char temp = cards[i];
						cards[i] = cards[r];
						cards[r] = temp;
					}

					index = 0;

					for (int i = 0;i < y;i++)
					{
						for (int j = 0;j < x;j++)
						{
							col[i].row[j].isalive = true;
							col[i].row[j].isfound = false;
							col[i].row[j].check = false;
							col[i].row[j].card = cards[index++];
						}
					}

					printBoard(x, y);
				}
			}
			else if (command[0] == 'h' && command[1] == '\0')
			{
				score--;
				system("cls");
				for (int i = 0;i < y;i++)
				{
					for (int j = 0;j < x;j++)
					{
						col[i].row[j].check = true;
					}
				}
				printBoard(x, y);
				Sleep(1000);
				system("cls");
				for (int i = 0;i < y;i++)
				{
					for (int j = 0;j < x;j++)
					{
						col[i].row[j].check = false;
					}
				}
				printBoard(x, y);
			}
			else if (command[0] == 'q' && command[1] == '\0')
			{
				break;
			}
			else
			{
				char x1, x2;
				int y1, y2;

				if (sscanf(command, "%c%d %c%d", &x1, &y1, &x2, &y2) == 4)
				{
					system("cls");

					int cx1 = x1 - 'a';
					int cy1 = y1 - 1;

					int cx2 = x2 - 'a';
					int cy2 = y2 - 1;

					if (cx1 < 0 || cx1 >= x || cx2 < 0 || cx2 >= x || cy1 < 0 || cy1 >= y || cy2 < 0 || cy2 >= y)
					{
						printf("잘못된 좌표입력입니다.\n");
						Sleep(1000);
						continue;
					}

					if (cx1 == cx2 && cy1 == cy2)
					{
						printf("같은 좌표입력입니다.");
						Sleep(1000);
						continue;
					}

					if (col[cy1].row[cx1].isfound == true || col[cy2].row[cx2].isfound == true)
					{
						printf("이미 찾은 카드입니다.\n");
						Sleep(1000);
						continue;
					}

					col[cy1].row[cx1].check = true;
					col[cy2].row[cx2].check = true;
					system("cls");
					printBoard(x, y);

					if (col[cy1].row[cx1].card == col[cy2].row[cx2].card)
					{
						col[cy1].row[cx1].isfound = true;
						col[cy2].row[cx2].isfound = true;
						score += 2;
						Sleep(1000);
					}
					else if (col[cy1].row[cx1].card == '@')  
					{
						col[cy1].row[cx1].isfound = true;
						for (int i = 0;i < y;i++)
						{
							for (int j = 0;j < x;j++)
							{
								if (col[i].row[j].card == col[cy2].row[cx2].card)
									col[i].row[j].isfound = true;
							}
						}
						score += 2;
						Sleep(1000);
					}
					else if (col[cy2].row[cx2].card == '@')
					{
						col[cy2].row[cx2].isfound = true;
						for (int i = 0;i < y;i++)
						{
							for (int j = 0;j < x;j++)
							{
								if (col[i].row[j].card == col[cy1].row[cx1].card)
									col[i].row[j].isfound = true;
							}
						}
						score+=2;
						Sleep(1000);
					}
					else
					{
						attemp--;
						Sleep(1000);
					}
					col[cy1].row[cx1].check = false;
					col[cy2].row[cx2].check = false;
				
				}
				else
				{
					printf("잘못된 입력입니다.\n");
				}
			}
		}
	}
	else
	{
		printf("범위는 3~6입니다.\n");
		Sleep(1000);
	}
}