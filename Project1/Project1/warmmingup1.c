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
Col temp1[4];
Col temp2[4];
Col cur1[3];
Col cur2[3];
Col cur3[3];
Col cur4[3];
Col cur5[2];
Col cur6[2];
Col cur7[2];

int min = 0, max = 0, check1 = 0, check2 = 0;

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

int det3(Col cur[3])
{
	int det2[3];

	for (int i = 0; i < 3; i++)
	{
		int r = 0;
		for (int j = 1; j < 3; j++)
		{
			int c = 0;
			for (int k = 0; k < 3; k++)
			{
				if (k != i)
				{
					if (i == 0)
						cur5[r].row[c].x = cur[j].row[k].x;
					else if (i == 1)
						cur6[r].row[c].x = cur[j].row[k].x;
					else
						cur7[r].row[c].x = cur[j].row[k].x;
					c++;
				}
			}
			r++;
		}
	}

	det2[0] =
		cur5[0].row[0].x * cur5[1].row[1].x
		- cur5[0].row[1].x * cur5[1].row[0].x;
	det2[1] =
		cur6[0].row[0].x * cur6[1].row[1].x
		- cur6[0].row[1].x * cur6[1].row[0].x;
	det2[2] =
		cur7[0].row[0].x * cur7[1].row[1].x
		- cur7[0].row[1].x * cur7[1].row[0].x;

	return cur[0].row[0].x * det2[0]
		- cur[0].row[1].x * det2[1]
		+ cur[0].row[2].x * det2[2];
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
		scanf(" %c", &command);
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
					if (col1[i].row[j].x > 9)
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
					col3[i].row[j].x = 0;
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
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					temp1[i].row[j].x = col1[j].row[i].x;
					temp2[i].row[j].x = col2[j].row[i].x;
				}
			}
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					col1[i].row[j].x = temp1[i].row[j].x;
					col2[i].row[j].x = temp2[i].row[j].x;
				}
			}
			printmatrix();
		}
		else if (command == 'q')
		{
			break;
		}
		else if (command == 'r')
		{
			for (int i = 0; i < 4; i++)
			{
				int r = 0;
				for (int j = 1; j < 4; j++)
				{
					int c = 0;

					for (int k = 0; k < 4; k++)
					{
						if (k != i)
						{
							if (i == 0)
								cur1[r].row[c].x = col1[j].row[k].x;
							else if (i == 1)
								cur2[r].row[c].x = col1[j].row[k].x;
							else if (i == 2)
								cur3[r].row[c].x = col1[j].row[k].x;
							else
								cur4[r].row[c].x = col1[j].row[k].x;
							c++;
						}
					}
					r++;
				}
			}

			int det1 = det3(cur1);
			int det2 = det3(cur2);
			int det3_value = det3(cur3);
			int det4 = det3(cur4);

			int result =
				col1[0].row[0].x * det1
				- col1[0].row[1].x * det2
				+ col1[0].row[2].x * det3_value
				- col1[0].row[3].x * det4;

			printf("행렬 1의 행렬식 : %d\n", result);

			for (int i = 0; i < 4; i++)
			{
				int r = 0;
				for (int j = 1; j < 4; j++)
				{
					int c = 0;

					for (int k = 0; k < 4; k++)
					{
						if (k != i)
						{
							if (i == 0)
								cur1[r].row[c].x = col2[j].row[k].x;
							else if (i == 1)
								cur2[r].row[c].x = col2[j].row[k].x;
							else if (i == 2)
								cur3[r].row[c].x = col2[j].row[k].x;
							else
								cur4[r].row[c].x = col2[j].row[k].x;
							c++;
						}
					}
					r++;
				}
			}

			det1 = det3(cur1);
			det2 = det3(cur2);
			det3_value = det3(cur3);
			det4 = det3(cur4);

			result =
				col2[0].row[0].x * det1
				- col2[0].row[1].x * det2
				+ col2[0].row[2].x * det3_value
				- col2[0].row[3].x * det4;

			printf("행렬 2의 행렬식 : %d\n", result);
		}
	}
}

