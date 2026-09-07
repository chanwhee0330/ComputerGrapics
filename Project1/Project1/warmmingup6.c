/*
실습 6: 파일을 읽어 삼각형의 정점/텍스처 좌표 저장
입력 예: testdata1.txt
출력: testdata1.txt.result.txt
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#define MAX_DATA 1000

typedef struct Vertex {
	float x, y, z;
}Vertex;
typedef struct Texture {
	float s, t;
}Texture;
typedef struct Face {
	int vertex[3], texture[3]; // 텍스처가 없으면 0
	int line;
}Face;

Vertex vertex[MAX_DATA];
Texture texture[MAX_DATA];
Face face[MAX_DATA];
int vertexLines[MAX_DATA];
int vcount = 0, tcount = 0, fcount = 0, error = 0;

void printError(FILE* out, int line, const char* message)
{
	printf("오류 (%d번째 줄) : %s\n", line, message);
	fprintf(out, "오류 (%d번째 줄) : %s\n", line, message);
	error++;
}

// 숫자 뒤에 문자가 붙었는지도 검사하는 함수
int checkFloat(const char* text, float* value)
{
	char* end;
	errno = 0;
	*value = strtof(text, &end);
	return end != text && *end == '\0' && errno != ERANGE && isfinite(*value);
}

int checkIndex(const char* text, int* value)
{
	char* end;
	long number;
	errno = 0;
	number = strtol(text, &end, 10);
	if (end == text || *end != '\0' || errno == ERANGE || number < 1 || number > INT_MAX)
	{
		return 0;
	}
	*value = (int)number;
	return 1;
}

// 1/2를 정점 1번, 텍스처 2번으로 나누는 함수
int checkFace(char* text, int* vindex, int* tindex)
{
	char* slash = strchr(text, '/');
	*tindex = 0;
	if (slash == NULL)
	{
		return checkIndex(text, vindex);
	}
	*slash = '\0';
	return checkIndex(text, vindex) && checkIndex(slash + 1, tindex);
}

int checkSame(Vertex a, Vertex b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

// 한 줄씩 읽어 종류별 배열에 저장한다.
void openFile(char* fileName, FILE* out)
{
	FILE* input = fopen(fileName, "r");
	if (input == NULL)
	{
		printError(out, 0, "파일을 찾을 수 없습니다.");
		return;
	}
	char line[1024];
	int lineNumber = 0;
	while (fgets(line, sizeof(line), input) != NULL)
	{
		char* tokens[8];
		char* token;
		char* comment;
		int count = 0;
		lineNumber++;
		if (!strchr(line, '\n') && !feof(input))
		{
			int ch = fgetc(input);
			if (ch != EOF)
			{
				while (ch != EOF && ch != '\n')
				{
					ch = fgetc(input);
				}
				printError(out, lineNumber, "한 줄의 길이가 너무 깁니다.");
				continue;
			}
		}
		comment = strchr(line, '#');
		if (comment != NULL)
		{
			*comment = '\0';
		}
		token = strtok(line, " \t\r\n");
		while (token != NULL && count < 8)
		{
			tokens[count++] = token;
			token = strtok(NULL, " \t\r\n");
		}
		if (count == 0)
		{
			continue;
		}
		if (strcmp(tokens[0], "v") == 0)
		{
			Vertex v;
			if (count != 4 || !checkFloat(tokens[1], &v.x) || !checkFloat(tokens[2], &v.y) ||
			    !checkFloat(tokens[3], &v.z))
			{
				printError(out, lineNumber, "v 뒤에는 x, y, z 숫자 3개가 필요합니다.");
				continue;
			}
			if (v.x < -1 || v.x > 1 || v.y < -1 || v.y > 1 || v.z < -1 || v.z > 1)
			{
				printError(out, lineNumber, "정점 좌표는 -1부터 1까지 입력해야 합니다.");
				continue;
			}
			if (vcount == MAX_DATA)
			{
				printError(out, lineNumber, "정점 배열이 꽉 찼습니다.");
				continue;
			}
			vertexLines[vcount] = lineNumber;
			vertex[vcount++] = v;
		}
		else if (strcmp(tokens[0], "vt") == 0)
		{
			Texture t;
			if (count != 3 || !checkFloat(tokens[1], &t.s) || !checkFloat(tokens[2], &t.t))
			{
				printError(out, lineNumber, "vt 뒤에는 s, t 숫자 2개가 필요합니다.");
				continue;
			}
			if (t.s < 0 || t.s > 1 || t.t < 0 || t.t > 1)
			{
				printError(out, lineNumber, "텍스처 좌표는 0부터 1까지 입력해야 합니다.");
				continue;
			}
			if (tcount == MAX_DATA)
			{
				printError(out, lineNumber, "텍스처 배열이 꽉 찼습니다.");
				continue;
			}
			texture[tcount++] = t;
		}
		else if (strcmp(tokens[0], "f") == 0)
		{
			Face f = {{0}, {0}, 0};
			bool check = true;
			if (count != 4)
			{
				printError(out, lineNumber, "삼각형 꼭짓점은 3개여야 합니다.");
				continue;
			}
			for (int i = 0; i < 3; i++)
			{
				if (!checkFace(tokens[i + 1], &f.vertex[i], &f.texture[i]))
				{
					check = false;
				}
			}
			if (check == false || ((f.texture[0] != 0) != (f.texture[1] != 0)) ||
			    ((f.texture[0] != 0) != (f.texture[2] != 0)))
			{
				printError(out, lineNumber,
				           "면의 번호 형식이 잘못됐습니다. f 정점 정점 정점 또는 f 정점/텍스처 정점/텍스처 정점/텍스처 "
				           "형식을 사용하세요.");
				continue;
			}
			if (fcount == MAX_DATA)
			{
				printError(out, lineNumber, "삼각형 배열이 꽉 찼습니다.");
				continue;
			}
			f.line = lineNumber;
			face[fcount++] = f;
		}
		else
		{
			printError(out, lineNumber, "v, vt, f 이외의 문자가 있습니다.");
		}
	}
	if (ferror(input))
	{
		printError(out, lineNumber, "파일을 읽을 수 없습니다.");
	}
	fclose(input);
}

// 번호 범위를 먼저 검사한 뒤 실제 좌표를 찾아 출력한다.
void print(FILE* out)
{
	int duplicates = 0;
	for (int i = 0; i < vcount; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (checkSame(vertex[i], vertex[j]))
			{
				char message[128];
				sprintf(message, "정점 %d번과 %d번의 좌표가 같습니다.", j + 1, i + 1);
				printError(out, vertexLines[i], message);
				duplicates++;
				break;
			}
		}
	}
	for (int i = 0; i < fcount; i++)
	{
		Face f = face[i];
		bool check = true;
		for (int j = 0; j < 3; j++)
		{
			if (f.vertex[j] > vcount || f.texture[j] > tcount)
			{
				check = false;
			}
		}
		if (check == false)
		{
			printError(out, f.line, "정점 또는 텍스처 번호가 범위를 벗어났습니다.");
			continue;
		}
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < j; k++)
			{
				if (checkSame(vertex[f.vertex[j] - 1], vertex[f.vertex[k] - 1]))
				{
					check = false;
				}
			}
		}
		if (check == false)
		{
			printError(out, f.line, "삼각형 안에 같은 정점 번호 또는 좌표가 있습니다.");
			continue;
		}
		fprintf(out, "Face %d (%d, %d, %d):\nvertex ", i + 1, f.vertex[0], f.vertex[1], f.vertex[2]);
		for (int j = 0; j < 3; j++)
		{
			Vertex v = vertex[f.vertex[j] - 1];
			fprintf(out, "(%.6f, %.6f, %.6f) ", v.x, v.y, v.z);
		}
		fprintf(out, "\ntexture ");
		if (f.texture[0] == 0)
		{
			fprintf(out, "(not specified)");
		}
		else
		{
			for (int j = 0; j < 3; j++)
			{
				Texture t = texture[f.texture[j] - 1];
				fprintf(out, "(%.6f, %.6f) ", t.s, t.t);
			}
		}
		fprintf(out, "\n\n");
	}
	if (duplicates == 0)
	{
		fprintf(out, "No duplicate vertex value\n");
	}
}

int main()
{
	char fileName[512], resultName[540];
	printf("파일 이름을 입력하세요 (예: testdata1.txt) : ");
	if (fgets(fileName, sizeof(fileName), stdin) == NULL)
	{
		return 1;
	}
	fileName[strcspn(fileName, "\r\n")] = '\0';
	sprintf(resultName, "%s.result.txt", fileName);

	FILE* out = fopen(resultName, "w");
	if (out == NULL)
	{
		printf("결과 파일을 만들 수 없습니다.\n");
		return 1;
	}

	openFile(fileName, out);
	if (error == 0)
	{
		print(out);
	}
	else
	{
		fprintf(out, "입력 오류가 있어 삼각형 좌표 출력을 중단했습니다.\n");
	}
	fprintf(out, "\n오류 개수 : %d\n", error);

	int check = ferror(out);
	if (fclose(out) != 0)
	{
		check = 1;
	}
	if (check != 0)
	{
		printf("결과 파일 저장에 실패했습니다.\n");
		return 1;
	}
	printf("%s 파일에 저장했습니다.\n", resultName);
	if (error != 0)
	{
		return 1;
	}
	return 0;
}