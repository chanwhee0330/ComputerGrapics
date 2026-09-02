#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


char lines[10][100];

void openFile()
{
	FILE* file = fopen("test.txt", "r");
	if (file == NULL)
	{
		printf("파일을 찾을 수 없습니다.");
		return;
	}

	char line[200];
	for (int i = 0;i < 10;i++)
	{
		if ((fgets(line, sizeof(line), file)) == NULL)
			break;
		strcpy(lines[i], line);
		lines[i][strcspn(lines[i], "\n")] = '\0';
	}

	fclose(file);
}

void print()
{
	for (int i = 0;i < 10;i++)
		printf("%s\n", lines[i]);
}

int main()
{
	openFile();
	char command;
	bool isd = false;
	print();
	while (1)
	{
		printf("명령어를 입력하세요 : ");
		scanf(" %c", &command);
		if (command == 'a')
		{
			for (int i = 0;i < 10;i++)
			{
				for (int j = 0;lines[i][j] != '\0';j++)
				{
					if (lines[i][j] >= 'A' && lines[i][j] <= 'Z')
					{
						lines[i][j] += 32;
					}
					else if (lines[i][j] >= 'a' && lines[i][j] <= 'z')
					{
						lines[i][j] -= 32;
					}
				}
			}
			print();
		}
		else if (command == 'b')
		{
			for (int i = 0;i < 10;i++)
			{
				int count = 0;
				for (int j = 0;lines[i][j]!='\0';j++)
				{
					if (lines[i][j] != ' ' && (j == 0 || lines[i][j - 1] == ' '))
					{
						count++;
					}
					printf("%c", lines[i][j]);
				}
				printf(" %d개\n", count);
			}
		}
		else if (command == 'c')
		{
			for (int i = 0;i < 10;i++)
			{
				int count = 0;
				for (int j = 0;lines[i][j] != '\0';j++)
				{
					if (lines[i][j]>='A'&&lines[i][j]<='Z'&&(j==0||lines[i][j-1]==' '))
					{
						count++;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
						printf("%c", lines[i][j]);
					}
					else if (lines[i][j]==' ')
					{
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						printf("%c", lines[i][j]);
					}
					else
					{
						printf("%c", lines[i][j]);
					}
				}
				printf(" %d개\n", count);
			}
		}
		else if (command == 'd')
		{
			if (isd == false)
			{
				for (int i = 0;i < 10;i++)
				{
					int count = 0;
					for (int j = 0;lines[i][j] != '\0';j++)
					{
						count++;
					}
					for (int j = count;j > 0;j--)
					{
						printf("%c", lines[i][j]);
					}
					printf("\n");
				}
				isd = true;
			}
			else
			{
				print();
				isd = false;
			}
		}
		else if (command == 'e')
		{
			for (int i = 0;i < 10;i++)
			{
				for (int j = 0;lines[i][j] != '\0';j++)
				{
					if (lines[i][j] == ' ')
						lines[i][j]='*';
					else if (lines[i][j] == '*')
						lines[i][j] = ' ';
				}
			}
			print();
		}
	}
}