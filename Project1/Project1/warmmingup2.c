#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


char lines[10][100];
char temp[100];

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
	bool isd = false,isf=false,ish=false;
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
						lines[i][j] = '*';
					else if (lines[i][j] == '*')
						lines[i][j] = ' ';
				}
			}
			print();
		}
		else if (command == 'f')
		{
			if (isf == false)
			{
				for (int i = 0;i < 10;i++)
				{
					for (int j = 0;lines[i][j] != '\0'; j++)
					{
						int count = j;
						while (lines[i][j] != ' ' && lines[i][j] != '*' && lines[i][j] != '\0')
							j++;

						for (int k = j - 1;k >= count;k--)
							printf("%c", lines[i][k]);

						if (lines[i][j] == ' ')
							printf(" ");
					}
					printf("\n");
				}
				isf = true;
			}
			else
			{
				print();
				isf = true;
			}
		}
		else if (command == 'g')
		{
			char change,word;
			scanf(" %c %c", &change,&word);
			for (int i = 0;i < 10;i++)
			{
				for (int j = 0;lines[i][j] != '\0';j++)
				{
					if (lines[i][j] == change)
						lines[i][j] = word;
				}
			}
			print();
		}
		else if (command == 'h')
		{
			if (ish == false)
			{
				for (int i = 0;i < 10;i++)
				{
					for (int j = 0;lines[i][j] != '\0';j++)
					{
						if (lines[i][j] >= '0' && lines[i][j] <= '9')
							printf("%c\n", lines[i][j]);
						else
							printf("%c", lines[i][j]);
					}
					printf("\n");
				}
				ish = true;
			}
			else
			{
				ish = false;
				print();
			}
			
		}
		else if (command == 'i')
		{
			char word[100];
			int count = 0;

			scanf(" %s", word);

			for (int i = 0; i < 10; i++)
			{
				int start = 0;

				for (int j = 0; ; j++)
				{
					if (lines[i][j] == ' ' || lines[i][j] == '\0')
					{
						int len = j - start;
						int same = 1;

						if (strlen(word) != len)
						{
							same = 0;
						}
						else
						{
							for (int k = 0; k < len; k++)
							{
								if (tolower(lines[i][start + k]) != tolower(word[k]))
								{
									same = 0;
									break;
								}
							}
						}

						if (same == 1)
						{
							count++;
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
						}
						else
						{
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						}

						for (int k = start; k < j; k++)
							printf("%c", lines[i][k]);

						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
						if (lines[i][j] == ' ')
							printf(" ");

						if (lines[i][j] == '\0')
							break;

						start = j + 1;
					}
				}

				printf("\n");
			}

			printf("%s는 %d개입니다.\n", word, count);
		}
		else if (command == 'j')
		{
			strcpy(temp, lines[9]);
			for (int i = 8;i >= 0;i--)
			{
				strcpy(lines[i + 1], lines[i]);
			}
			strcpy(lines[0], temp);
			print();
		}
		else if (command=='q')
			break;
	}
}