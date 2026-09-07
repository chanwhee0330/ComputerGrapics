#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char lines[10][100];
bool isa = false, isc = false, isd = false, ise = false;
bool isf = false, isg = false, ish = false;
char change, replacement;

void openFile(void)
{
	FILE* file = fopen("test.txt", "r");
	if (file == NULL)
	{
		printf("파일을 찾을 수 없습니다.\n");
		return;
	}
	for (int i = 0; i < 10; i++)
	{
		if (fgets(lines[i], sizeof(lines[i]), file) == NULL)
			break;
		lines[i][strcspn(lines[i], "\r\n")] = '\0';
		// 긴 줄의 나머지가 다음 줄로 들어가지 않도록 건너뛴다.
		if (strlen(lines[i]) == sizeof(lines[i]) - 1)
		{
			int ch;
			while ((ch = fgetc(file)) != '\n' && ch != EOF) {}
		}
	}
	fclose(file);
}

bool isSeparator(char ch)
{
	return ch == ' ' || ch == '*';
}

void reverse(char* line, int start, int end)
{
	while (start < end)
	{
		char temp = line[start];
		line[start++] = line[end];
		line[end--] = temp;
	}
}

void makeLine(int index, char* result)
{
	strcpy(result, lines[index]);
	// 입력 순서와 관계없이 같은 순서로 켜진 기능을 적용한다.
	for (int j = 0; result[j] != '\0'; j++)
	{
		if (isa)
		{
			if (result[j] >= 'A' && result[j] <= 'Z') result[j] += 32;
			else if (result[j] >= 'a' && result[j] <= 'z') result[j] -= 32;
		}
		if (ise)
		{
			if (result[j] == ' ') result[j] = '*';
			else if (result[j] == '*') result[j] = ' ';
		}
		if (isg && result[j] == change) result[j] = replacement;
	}
	int length = (int)strlen(result);
	if (isf)
	{
		int start = 0;
		for (int j = 0; j <= length; j++)
		{
			if (isSeparator(result[j]) || result[j] == '\0')
			{
				reverse(result, start, j - 1);
				start = j + 1;
			}
		}
	}
	if (isd) reverse(result, 0, length - 1);
}

bool sameWord(const char* text, int length, const char* word)
{
	if ((int)strlen(word) != length) return false;
	for (int i = 0; i < length; i++)
		if (tolower((unsigned char)text[i]) != tolower((unsigned char)word[i]))
			return false;
	return true;
}

void print(bool countWords, const char* search)
{
	int matches = 0;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 10; i++)
	{
		char line[100];
		makeLine(i, line);
		int words = 0, capitals = 0;
		bool highlight = false;
		for (int j = 0; line[j] != '\0'; j++)
		{
			if (isSeparator(line[j])) highlight = false;
			else if (j == 0 || isSeparator(line[j - 1]))
			{
				words++;
				bool capital = line[j] >= 'A' && line[j] <= 'Z';
				if (capital) capitals++;
				highlight = isc && capital;
				if (search != NULL)
				{
					int end = j;
					while (line[end] != '\0' && !isSeparator(line[end])) end++;
					if (sameWord(line + j, end - j, search))
					{
						matches++;
						highlight = true;
					}
				}
			}
			SetConsoleTextAttribute(console, highlight ? 4 : 15);
			printf("%c", line[j]);
			if (ish && line[j] >= '0' && line[j] <= '9') printf("\n");
		}
		SetConsoleTextAttribute(console, 15);
		if (isc) printf(" %d개", capitals);
		if (countWords) printf(" 단어 %d개", words);
		printf("\n");
	}
	if (search != NULL) printf("%s는 %d개입니다.\n", search, matches);
}

int main(void)
{
	openFile();
	print(false, NULL);
	while (1)
	{
		char command, word[100];
		printf("명령어를 입력하세요 : ");
		if (scanf(" %c", &command) != 1) break;
		switch (command)
		{
		case 'a': isa = !isa; break;
		case 'c': isc = !isc; break;
		case 'd': isd = !isd; break;
		case 'e': ise = !ise; break;
		case 'f': isf = !isf; break;
		case 'g':
			if (!isg)
			{
				printf("바꿀 문자와 새 문자를 입력하세요 : ");
				if (scanf(" %c %c", &change, &replacement) != 2) return 0;
			}
			isg = !isg;
			break;
		case 'h': ish = !ish; break;
		case 'b': print(true, NULL); continue;
		case 'i':
			if (scanf(" %99s", word) != 1) return 0;
			print(false, word);
			continue;
		case 'j':
		{
			char temp[100];
			strcpy(temp, lines[9]);
			for (int i = 8; i >= 0; i--) strcpy(lines[i + 1], lines[i]);
			strcpy(lines[0], temp);
			break;
		}
		case 'q': return 0;
		default: continue;
		}
		print(false, NULL);
	}
	return 0;
}
