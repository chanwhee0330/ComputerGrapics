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

typedef struct Box {
	float x, y, r, g, b;
	bool isOn,isAlive;
	int move;
}Box;

Box boxa[20];
Box boxb[20];
Box boxc[20];
int conA, conB, conC;
float boxaSpeed=0.005f, boxbSpeed=0.007f;
bool correct,onA,onB;

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
	conA = conB = conC = 0;

	for (int i = 0;i < 20;i++)
	{
		boxa[i].x = -0.7f;
		boxa[i].y = 0.75f;
		boxb[i].x = -0.2f;
		boxb[i].y = -0.75f;
		boxc[i].x = 0.7f;
		boxc[i].y = -0.75;

		boxa[i].r = (float)rand() / RAND_MAX;
		boxa[i].g = (float)rand() / RAND_MAX;
		boxa[i].b = (float)rand() / RAND_MAX;
		
		boxb[i].r = (float)rand() / RAND_MAX;
		boxb[i].g = (float)rand() / RAND_MAX;
		boxb[i].b = (float)rand() / RAND_MAX;
		
		boxa[conA].isAlive = true;
		boxb[conB].isAlive = true;

		boxa[i].move = 1;
		boxb[i].move = 2;
	}

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
	std::string vertexSource = filetobuf("vertex5.glsl");
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
	GLint colorLocation = glGetUniformLocation(shaderProgramID, "vColor");

	//--- 이 아래에 필요한 그리기 기능을 추가

	glUniform2f(offsetLocation, -0.7f, 0.0f);
	glUniform4f(colorLocation, 0,0,0,1.0f);
	glDrawArrays(GL_LINE_LOOP, 4, 4);
	glUniform2f(offsetLocation, -0.2f, 0.0f);
	glDrawArrays(GL_LINE_LOOP, 4, 4);
	glUniform2f(offsetLocation, 0.7f, 0.0f);
	glDrawArrays(GL_LINE_LOOP, 4, 4);
	glUniform2f(offsetLocation, -0.45f, 0.0f);
	glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_LINE_LOOP, 8, 4);

	for (int i = 0;i < 20;i++)
	{
		if (boxa[i].isAlive)
		{
			glUniform2f(offsetLocation, boxa[i].x, boxa[i].y);
			glUniform4f(colorLocation, boxa[i].r, boxa[i].g, boxa[i].b, 1.0f);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (boxa[i].move == 1)
			{
				boxa[i].y += boxaSpeed;
				if (boxa[i].y >= 0.75f)
					boxa[i].move = 2;
			}
			else if (boxa[i].move == 2)
			{
				boxa[i].y -= boxaSpeed;
				if (boxa[i].y <= -0.75f)
					boxa[i].move = 1;
			}
			else if (boxa[i].move == 3)
			{
				boxa[i].x += boxaSpeed;
				if (boxa[i].x >= 0.7f)
				{
					boxa[i].isAlive = false;
					onA = true;
				}
				
			}
			
			if (boxa[i].y >= -0.1f && boxa[i].y <= 0.1f)
			{
				boxa[i].isOn = true;
			}
			else
				boxa[i].isOn = false;
		}

		if (boxb[i].isAlive)
		{
			glUniform2f(offsetLocation, boxb[i].x, boxb[i].y);
			glUniform4f(colorLocation, boxb[i].r, boxb[i].g, boxb[i].b, 1.0f);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			if (boxb[i].move==1)
			{
				boxb[i].y += boxbSpeed;
				if (boxb[i].y >= 0.75f)
					boxb[i].move = 2;
			}
			else if (boxb[i].move == 2)
			{
				boxb[i].y -= boxbSpeed;
				if (boxb[i].y <= -0.75f)
					boxb[i].move = 1;
			}
			else if (boxb[i].move == 3)
			{
				boxb[i].x += boxbSpeed;
				if (boxb[i].x >= 0.7f)
				{
					boxb[i].isAlive = false;
					onB = true;
				}
			}

			if (boxb[i].y >= -0.1f && boxb[i].y <= 0.1f)
			{
				boxb[i].isOn = true;
			}
			else
				boxb[i].isOn = false;
		}

		if (onA)
		{
			boxc[conC].r = boxa[i].r;
			boxc[conC].g = boxa[i].g;
			boxc[conC].b = boxa[i].b;
			boxc[conC].isAlive = true;
			conC++;
			conA++;
			boxa[conA].isAlive = true;
			onA = false;
		}

		if (onB)
		{
			boxc[conC].r = boxb[i].r;
			boxc[conC].g = boxb[i].g;
			boxc[conC].b = boxb[i].b;
			boxc[conC].isAlive = true;
			conC++;
			conB++;
			boxb[conB].isAlive = true;
			onB = false;
		}

		if (boxc[i].isAlive)
		{
			glUniform2f(offsetLocation, boxc[i].x, boxc[i].y);
			glUniform4f(colorLocation, boxc[i].r, boxc[i].g, boxc[i].b, 1.0f);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		
	}

	for (int i = 0;i < conC;i++)
	{
		
		boxc[i].y = -0.75 + (i * 0.1f);
	}

}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		for (int i = 0;i < 20;i++)
		{
			if (boxa[i].isOn == true && boxb[i].isOn == true)
			{
				boxa[i].move = 3;
				boxb[i].move = 3;	
			}
		}
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
