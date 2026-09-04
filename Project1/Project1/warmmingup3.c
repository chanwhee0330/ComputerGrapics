#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

typedef struct List {
	bool isalive;
	int x, y, z;
	double dis;
}List;

List list[10];
List saveList[10];
List min[2], max[2];

bool isf = false;
void print()
{
	for (int i = 9;i >= 0;i--)
	{
		if(isf==true&&list[i].isalive==true)
			printf("%d : %d %d %d distance : %f\n", i, list[i].x, list[i].y, list[i].z,list[i].dis);
		else if (list[i].isalive == true)
			printf("%d : %d %d %d\n", i, list[i].x, list[i].y, list[i].z);
		else
			printf("%d\n", i);
	}
}

int main()
{
	while (1)
	{
		char command;
		print();
		printf("명령어를 입력하세요 : ");
		scanf(" %c", &command);
		if (command == '+')
		{
			int x, y, z;
			scanf("%d %d %d", &x, &y, &z);
			bool isfull = true;
			
			for (int i = 0;i < 10;i++)
			{
				if (list[i].isalive == false)
				{
					list[i].isalive = true;
					list[i].x = x;
					list[i].y = y;
					list[i].z = z;

					isfull = false;
					break;
				}
			}
			if (isfull == true)
			{
				printf("리스트가 꽉찼습니다.\n");
			}
		}
		else if (command == '-')
		{
			bool isempty = true;
			for (int i = 9;i >= 0;i--)
			{
				if (list[i].isalive == true)
				{
					list[i].isalive = false;
					isempty = false;
					break;
				}
			}
			if (isempty == true)
				printf("리스트가 다 비워졌습니다.\n");
		}
		else if (command == 'e')
		{
			int x, y, z;
			scanf("%d %d %d", &x, &y, &z);

			int empty = -1;

			for (int i = 0; i < 10; i++)
			{
				if (list[i].isalive == false)
				{
					empty = i;
					break;
				}
			}

			if (empty == -1)
			{
				printf("리스트가 꽉찼습니다.\n");
			}
			else
			{
				for (int i = empty; i > 0; i--)
				{
					list[i] = list[i - 1];
				}

				list[0].isalive = true;
				list[0].x = x;
				list[0].y = y;
				list[0].z = z;
			}
		}
		else if (command == 'd')
		{
			for (int i = 0;i < 10;i++)
			{
				if (list[i].isalive == true)
				{
					list[i].isalive = false;
					break;
				}
			}
		}
		else if (command == 'a')
		{
			int count = 0;
			for(int i= 0;i<10;i++)
			{
				if (list[i].isalive == true)
					count++;
			}
			printf("리스트에 저장된 점의 개수 : %d\n", count);
		}
		else if (command == 'b')
		{
			List temp = list[9];

			for (int i = 9; i > 0; i--)
			{
				list[i] = list[i - 1];
			}

			list[0] = temp;
		}
		else if (command == 'c')
		{
			for (int i = 0;i < 10;i++)
			{
				list[i].isalive = false;
			}
		}
		else if (command == 'f')
		{
			if (isf == false)
			{
				isf = true;
				for (int i = 0;i < 10;i++)
				{
					if (list[i].isalive == true)
					{
						list[i].dis = sqrt(list[i].x * list[i].x + list[i].y * list[i].y + list[i].z * list[i].z);
					}
					saveList[i] = list[i];
				}
				for (int i = 0;i < 9;i++)
				{
					if (list[i].isalive == false)
						continue;

					for (int j = i + 1;j < 10;j++)
					{
						if (list[j].isalive == false)
							continue;
						double dis1 = list[i].dis;
						double dis2 = list[j].dis;
					
						if (dis1 > dis2)
						{
							List temp = list[i];
							list[i] = list[j];
							list[j] = temp;
						}
					}
				}
			}
			else
			{
				for (int i = 0;i < 10;i++)
				{
					list[i] = saveList[i];
				}
				isf = false;
			}
		}
		else if (command == 'g')
		{
			double mindis = 9999999, maxdis=0;
			for (int i = 0;i < 9;i++)
			{
				if (list[i].isalive == false)
					continue;
				for (int j = i + 1;j < 10;j++)
				{
					if (list[j].isalive == false)
						continue;
					double dis = sqrt((list[i].x - list[j].x) * (list[i].x - list[j].x) + (list[i].y - list[j].y) * (list[i].y - list[j].y) + (list[i].z - list[j].z) * (list[i].z - list[j].z));
					if (dis > maxdis)
					{
						max[0] = list[i];
						max[1] = list[j];
						maxdis = dis;
					}

					if (dis < mindis)
					{
						min[0] = list[i];
						min[1] = list[j];
						mindis = dis;
					}
				}
			}
			printf("가장 먼 거리의 점 (%d, %d, %d), (%d, %d, %d) 거리 : %f 가장 가까운 거리의 점 (%d, %d, %d), (%d, %d, %d) 거리 : %f\n", max[0].x, max[0].y, max[0].z, max[1].x, max[1].y, max[1].z, maxdis, min[0].x, min[0].y, min[0].z, min[1].x, min[1].y, min[1].z, mindis);
		}
		else if (command == 'q')
		{
			break;
		}
	}
}