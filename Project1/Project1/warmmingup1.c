#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Row {
	int x;
}Row;

typedef struct Col {
	Row row[4];
	int min, max;
}Col;

Col col1[4];
Col col2[4];
Col col3[4];
int min=0, max=0,check1=0,check2=0;

void printmatrix()
{
	printf("[행렬 1]\n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%d ", col1[i].row[j].x);
		}
		printf("\n");
	}
	printf("[행렬 2]\n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%d ", col2[i].row[j]);
		}
		printf("\n");
	}
}

int main()
{
	srand(time(NULL));
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			col1[i].row[j].x = rand() % 10;
			col2[i].row[j].x = rand() % 10;
		}
	}

	printmatrix();
	while (1)
	{
		char command;
		printf("명령어를 입력하세요 : ");
		scanf("%c", &command);
		if (command == 'a')
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					col3[i].row[j].x = col1[i].row[j].x + col2[i].row[j].x;
					printf("%d ", col3[i].row[j]);
				}
				printf("\n");
			}
		}
		else if (command == 'd')
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					col3[i].row[j].x = col1[i].row[j].x - col2[i].row[j].x;
					printf("%d ", col3[i].row[j]);
				}
				printf("\n");
			}
		}
		else if (command == 's')
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					col1[i].row[j].x = rand() % 10;
					col2[i].row[j].x = rand() % 10;
				}
			}
			printmatrix();
		}
		else if (command == '-')
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					col1[i].row[j].x--;
					col2[i].row[j].x--;
					if (col1[i].row[j].x < 0)
						col1[i].row[j].x = 9;
					if (col2[i].row[j].x < 0)
						col2[i].row[j].x = 9;
				}
			}
			printmatrix();
		}
		else if (command == '+')
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					col1[i].row[j].x++;
					col2[i].row[j].x++;
					if (col1[i].row[j].x >9 )
						col1[i].row[j].x = 0;
					if (col2[i].row[j].x > 9)
						col2[i].row[j].x = 0;
				}
			}
			printmatrix();
		}
		else if (command == 'e')
		{
			if (check1 == 0)
			{
				for (int i = 0; i < 4; i++)
				{
					col1[i].min = col1[i].row[0].x;
					col2[i].min = col2[i].row[0].x;
					for (int j = 0; j < 4; j++)
					{
						if (col1[i].row[j].x < col1[i].min)
							col1[i].min = col1[i].row[j].x;
						if (col2[i].row[j].x < col2[i].min)
							col2[i].min = col2[i].row[j].x;
					}
					for (int j = 0; j < 4; j++)
					{
						col1[i].row[j].x -= col1[i].min;
						col2[i].row[j].x -= col2[i].min;
					}
				}
				printmatrix();
				check1 = 1;
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						col1[i].row[j].x += col1[i].min;
						col2[i].row[j].x += col2[i].min;
					}
				}
				printmatrix();
				check1 = 0;
			}
		}
		else if (command == 'f')
		{
			if (check1 == 0)
			{
				for (int i = 0; i < 4; i++)
				{
					col1[i].max = col1[i].row[0].x;
					col2[i].max = col2[i].row[0].x;
					for (int j = 0; j < 4; j++)
					{
						if (col1[i].row[j].x > col1[i].max)
							col1[i].max = col1[i].row[j].x;
						if (col2[i].row[j].x > col2[i].max)
							col2[i].max = col2[i].row[j].x;
					}
					for (int j = 0; j < 4; j++)
					{
						col1[i].row[j].x += col1[i].max;
						col2[i].row[j].x += col2[i].max;
					}
				}
				printmatrix();
				check1 = 1;
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						col1[i].row[j].x -= col1[i].max;
						col2[i].row[j].x -= col2[i].max;
					}
				}
				printmatrix();
				check1 = 0;
			}
		}
		else if (command == 'm')
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					for (int k = 0; k < 4; k++)
					{
						col3[i].row[j].x += col1[i].row[k].x * col2[k].row[j].x;
					}
				}
			}
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					printf("%d ", col3[i].row[j]);
				}
				printf("\n");
			}
		}
		else if (command == 't')
		{

		}
	}
}
