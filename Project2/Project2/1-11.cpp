#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>
#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

//--- 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void GetCursorPose(GLFWwindow* window, double xpos, double ypos);

//--- 셰이더 관련 함수
std::string filetobuf(const char* filePath);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();

//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO;

int winrow, wincol,px,py;
float speed=1.0f;
double lastMove=0;

typedef struct Col {
	float x, y, r, g, b,scalex,scaley,obr,obg,obb,obsize,pr,pg,pb,psize;
	bool isPassed,isPlayer,iscollision;
	int isObstacle,playerType;
}Col;

typedef struct Row {
	Col col[30];
}Row;

Row row[30];
int main(void)
{
	srand((unsigned int)time(NULL));

	GLFWwindow* window;
	GLenum glewResult;

	width = 1200;
	height = 1200;

	//--- GLFW 초기화
	if (glfwInit() != GLFW_TRUE)
	{
		printf("GLFW initialization failed.\n");
		return EXIT_FAILURE;
	}

	//--- OpenGL 버전 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	printf("가로 길이를 입력하세요 ( 10 ~ 30 ) : ");
	scanf("%d", &wincol);
	printf("세로 길이를 입력하세요 ( 10 ~ 30 ) : ");
	scanf("%d", &winrow);
	width = wincol * 40;
	height = winrow * 40;

	for (int i = 0;i < winrow;i++)
	{
		for (int j = 0;j < wincol;j++)
		{
			int random = rand() % 10;
			row[i].col[j].x = -1.0f + (j + 0.5f) * (2.0f / wincol);
			row[i].col[j].y = 1.0f - (i + 0.5f) * (2.0f / winrow);

			row[i].col[j].scalex = 2.0f/wincol;
			row[i].col[j].scaley = 2.0f/winrow;

			row[i].col[j].r = 0.0f;
			row[i].col[j].g = 0.0f;
			row[i].col[j].b = 0.0f;

			row[i].col[j].obr = (float)rand() / RAND_MAX;
			row[i].col[j].obg = (float)rand() / RAND_MAX;
			row[i].col[j].obb = (float)rand() / RAND_MAX;

			row[i].col[j].pr = (float)rand() / RAND_MAX;
			row[i].col[j].pg = (float)rand() / RAND_MAX;
			row[i].col[j].pb = (float)rand() / RAND_MAX;

			row[i].col[j].obsize = 0.5f + (float)rand() / RAND_MAX * 0.3f;

			if (random == 0)
				row[i].col[j].isObstacle = 1;
			else if (random == 1)
				row[i].col[j].isObstacle = 2;
			else if (random == 2)
				row[i].col[j].isObstacle = 3;
			else
				row[i].col[j].isObstacle = 0;
		}
	}

	row[0].col[0].isPlayer = true;
	row[0].col[0].playerType = 1;
	row[0].col[0].psize = 0.8f;
	row[0].col[0].isObstacle = 0;
	//--- 윈도우 생성
	window = glfwCreateWindow(width, height, "OpenGL Basic Window (C++)", NULL, NULL);
	if (window == NULL)
	{
		printf("Window creation failed.\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetWindowPos(window, 20, 50);

	//--- OpenGL 컨텍스트 설정
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//--- GLEW 초기화
	glewExperimental = GL_TRUE;
	glewResult = glewInit();
	if (glewResult != GLEW_OK)
	{
		printf("GLEW initialization failed: %s\n",
			(const char*)glewGetErrorString(glewResult));
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	//--- 콜백 함수 등록
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, GetCursorPose);

	//--- 뷰포트 설정
	glViewport(0, 0, width, height);

	//--- VAO 생성 및 바인딩
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//--- 세이더 읽어와서 세이더 프로그램 만들기
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	//--- 메인 루프
	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		//--- 화면 출력
		drawScene();

		//--- Q 키를 누르면 종료
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		//--- 버퍼 교체 및 이벤트 처리
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//--- 종료 처리
	glDeleteProgram(shaderProgramID);
	glDeleteVertexArrays(1, &VAO);
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

//--- 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	//--- 세이더 코드 읽어오기
	std::string vertexSource = filetobuf("vertex4.glsl");
	const char* source = vertexSource.c_str();

	//--- 세이더 생성하기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//--- 세이더에 코드 연결하고 컴파일 하기
	glShaderSource(vertexShader, 1, &source, NULL);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];

	//--- 에러 체크하기
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//--- 프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	//--- 세이더 코드 읽어오기
	std::string fragmentSource = filetobuf("fragment.glsl");
	const char* source = fragmentSource.c_str();

	//--- 세이더 생성하기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//--- 세이더에 코드 연결하고 컴파일 하기
	glShaderSource(fragmentShader, 1, &source, NULL);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];

	//--- 에러 체크하기
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//--- 세이더 프로그램 만들고 세이더 객체 링크하기
GLuint make_shaderProgram()
{
	GLint result;
	GLchar errorLog[512];

	//--- 세이더 프로그램 생성
	GLuint shaderID = glCreateProgram();

	//--- 버텍스 세이더와 프래그먼트 세이더 연결
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);

	//--- 세이더 프로그램 링크
	glLinkProgram(shaderID);

	//--- 링크가 끝났으므로 세이더 객체 삭제
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//--- 링크 성공 여부 확인
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return 0;
	}

	//--- 세이더 프로그램 사용
	glUseProgram(shaderID);
	return shaderID;
}

//--- 출력 함수
GLvoid drawScene()
{
	//--- 흰색 배경으로 화면 지우기
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//--- 셰이더 프로그램 사용
	glUseProgram(shaderProgramID);

	GLint offsetLocation = glGetUniformLocation(shaderProgramID, "offset");
	GLint scaleLocation = glGetUniformLocation(shaderProgramID, "scale");
	GLint colorLocation = glGetUniformLocation(shaderProgramID, "vColor");

	//--- 이 아래에 필요한 그리기 기능을 추가
	for (int i = 0;i < winrow;i++)
	{
		for (int j = 0;j < wincol;j++)
		{
			if (row[i].col[j].iscollision == false)
			{
				glUniform2f(offsetLocation, row[i].col[j].x, row[i].col[j].y);
				glUniform2f(scaleLocation, row[i].col[j].scalex, row[i].col[j].scaley);
				glUniform4f(colorLocation, row[i].col[j].r, row[i].col[j].g, row[i].col[j].b, 1.0f);
				glDrawArrays(GL_LINE_LOOP, 0, 4);
			}
			else
			{
				glUniform2f(offsetLocation, row[i].col[j].x, row[i].col[j].y);
				glUniform2f(scaleLocation, row[i].col[j].scalex, row[i].col[j].scaley);
				glUniform4f(colorLocation, 1.0f, 0, 0, 1.0f);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}


			if (row[i].col[j].isObstacle == 1)
			{
				glUniform2f(scaleLocation, row[i].col[j].scalex * row[i].col[j].obsize, row[i].col[j].scaley * row[i].col[j].obsize);
				glUniform4f(colorLocation, row[i].col[j].obr, row[i].col[j].obg, row[i].col[j].obb, 1.0f);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}
			else if (row[i].col[j].isObstacle == 2)
			{
				glUniform2f(scaleLocation, row[i].col[j].scalex * row[i].col[j].obsize, row[i].col[j].scaley * row[i].col[j].obsize);
				glUniform4f(colorLocation, row[i].col[j].obr, row[i].col[j].obg, row[i].col[j].obb, 1.0f);
				glDrawArrays(GL_TRIANGLES, 4, 3);
			}
			else if (row[i].col[j].isObstacle == 3)
			{
				glUniform2f(scaleLocation, row[i].col[j].scalex * row[i].col[j].obsize, row[i].col[j].scaley * row[i].col[j].obsize);
				glUniform4f(colorLocation, row[i].col[j].obr, row[i].col[j].obg, row[i].col[j].obb, 1.0f);
				glDrawArrays(GL_TRIANGLES, 7, 3);
			}

			if (row[i].col[j].isPlayer)
			{
				glUniform2f(scaleLocation, row[i].col[j].scalex * row[i].col[j].psize, row[i].col[j].scaley * row[i].col[j].psize);
				glUniform4f(colorLocation, row[i].col[j].pr, row[i].col[j].pg, row[i].col[j].pb, 1.0f);
				if (row[i].col[j].playerType == 1)
					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				else if (row[i].col[j].playerType == 2)
					glDrawArrays(GL_TRIANGLES, 4, 3);
				else if (row[i].col[j].playerType == 3)
					glDrawArrays(GL_TRIANGLES, 7, 3);
			}
		}
	}

	double now = glfwGetTime();
	if (now - lastMove > speed)
	{
		int nextX = px;
		int nextY = py;

		if (py % 2 == 0 && px < wincol - 1)
			nextX++;
		else if (py % 2 == 1 && px > 0)
			nextX--;
		else if (py < winrow - 1)
			nextY++;

		if (nextX != px || nextY != py)
		{
			for (int i = 0; i < winrow; i++)
			{
				for (int j = 0; j < wincol; j++)
				{
					if (i == py && j == px)
					{
						row[nextY].col[nextX].psize = row[i].col[j].psize;
						row[nextY].col[nextX].pr = row[i].col[j].pr;
						row[nextY].col[nextX].pg = row[i].col[j].pg;
						row[nextY].col[nextX].pb = row[i].col[j].pb;
						row[nextY].col[nextX].playerType = row[i].col[j].playerType;

						row[i].col[j].isPlayer = false;
						row[i].col[j].iscollision = false;
						row[nextY].col[nextX].isPlayer = true;

						if (row[nextY].col[nextX].isObstacle != 0)
						{
							float tsize = row[nextY].col[nextX].psize;
							float tr = row[nextY].col[nextX].pr;
							float tg = row[nextY].col[nextX].pg;
							float tb = row[nextY].col[nextX].pb;
							int ttype = row[nextY].col[nextX].playerType;

							row[nextY].col[nextX].psize = row[nextY].col[nextX].obsize;
							row[nextY].col[nextX].pr = row[nextY].col[nextX].obr;
							row[nextY].col[nextX].pg = row[nextY].col[nextX].obg;
							row[nextY].col[nextX].pb = row[nextY].col[nextX].obb;
							row[nextY].col[nextX].playerType = row[nextY].col[nextX].isObstacle;

							row[nextY].col[nextX].obsize = tsize;
							row[nextY].col[nextX].obr = tr;
							row[nextY].col[nextX].obg = tg;
							row[nextY].col[nextX].obb = tb;
							row[nextY].col[nextX].isObstacle = ttype;

							row[nextY].col[nextX].iscollision = true;
						}
					}
				}
			}
			px = nextX;
			py = nextY;
		}
		else
		{
			row[py].col[px].iscollision = false;
		}
		lastMove = now;
	}

}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT)) || key == GLFW_KEY_KP_ADD)
	{
		if (speed >= 0.2f)
			speed -= 0.1f;
	}
	else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
	{
		if (speed < 1.5f)
			speed += 0.1f;
	}
}

//--- 마우스 버튼 입력 콜백 함수
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{

	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{

	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{

	}
}

//--- 마우스 위치 입력 콜백 함수
void GetCursorPose(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

	}
}

//--- 텍스트 파일의 내용을 문자열로 읽어오는 함수
std::string filetobuf(const char* filePath)
{
	std::ifstream file(filePath);
	std::ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}
