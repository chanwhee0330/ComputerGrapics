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

typedef struct Shape {
	GLfloat x, y, r, g, b;
	GLint type;
	bool isSelect,isAlive;
}Shape;

Shape shape[100];

GLint shapeCount;

int main(void)
{
	srand((unsigned int)time(NULL));

	GLFWwindow* window;
	GLenum glewResult;

	width = 1500;
	height = 900;

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
	std::string vertexSource = filetobuf("vertex.glsl");
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

	GLint vOffsetLocation = glGetUniformLocation(shaderProgramID, "offset");
	GLint vColorLocation = glGetUniformLocation(shaderProgramID, "vColor");

	glPointSize(10.0f);
	glLineWidth(3.0f);

	for (int i = 0; i < shapeCount; i++)
	{
		glUniform2f(vOffsetLocation, shape[i].x, shape[i].y);


		if (shape[i].isAlive)
		{
			if (shape[i].isSelect)
				glUniform4f(vColorLocation, 0, 0, 0, 1.0f);
			else
				glUniform4f(vColorLocation, shape[i].r, shape[i].g, shape[i].b, 1.0f);

			if (shape[i].type == 0)
				glDrawArrays(GL_POINTS, 0, 1);
			else if (shape[i].type == 1)
				glDrawArrays(GL_LINES, 1, 2);
			else if (shape[i].type == 2)
				glDrawArrays(GL_TRIANGLES, 3, 3);
			else if (shape[i].type == 3)
			{
				glDrawArrays(GL_TRIANGLES, 6, 3);
				glDrawArrays(GL_TRIANGLES, 7, 3);
			}
		}
	}

	//--- 이 아래에 필요한 그리기 기능을 추가
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (shapeCount < 100)
		{
			shape[shapeCount].x = -0.8f + 1.6f * ((float)rand() / RAND_MAX);
			shape[shapeCount].y = -0.8f + 1.6f * ((float)rand() / RAND_MAX);

			shape[shapeCount].r = (float)rand() / RAND_MAX;
			shape[shapeCount].g = (float)rand() / RAND_MAX;
			shape[shapeCount].b = (float)rand() / RAND_MAX;

			shape[shapeCount].type = 0;
			shape[shapeCount].isSelect = false;
			shape[shapeCount].isAlive = true;
			shapeCount++;
		}
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		if (shapeCount < 100)
		{
			shape[shapeCount].x = -0.8f + 1.6f * ((float)rand() / RAND_MAX);
			shape[shapeCount].y = -0.8f + 1.6f * ((float)rand() / RAND_MAX);

			shape[shapeCount].r = (float)rand() / RAND_MAX;
			shape[shapeCount].g = (float)rand() / RAND_MAX;
			shape[shapeCount].b = (float)rand() / RAND_MAX;

			shape[shapeCount].type = 1;
			shape[shapeCount].isSelect = false;
			shape[shapeCount].isAlive = true;
			shapeCount++;
		}
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		if (shapeCount < 100)
		{
			shape[shapeCount].x = -0.8f + 1.6f * ((float)rand() / RAND_MAX);
			shape[shapeCount].y = -0.8f + 1.6f * ((float)rand() / RAND_MAX);

			shape[shapeCount].r = (float)rand() / RAND_MAX;
			shape[shapeCount].g = (float)rand() / RAND_MAX;
			shape[shapeCount].b = (float)rand() / RAND_MAX;

			shape[shapeCount].type = 2;
			shape[shapeCount].isSelect = false;
			shape[shapeCount].isAlive = true;
			shapeCount++;
		}
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (shapeCount < 100)
		{
			shape[shapeCount].x = -0.8f + 1.6f * ((float)rand() / RAND_MAX);
			shape[shapeCount].y = -0.8f + 1.6f * ((float)rand() / RAND_MAX);

			shape[shapeCount].r = (float)rand() / RAND_MAX;
			shape[shapeCount].g = (float)rand() / RAND_MAX;
			shape[shapeCount].b = (float)rand() / RAND_MAX;

			shape[shapeCount].type = 3;
			shape[shapeCount].isSelect = false;
			shape[shapeCount].isAlive = true;
			shapeCount++;
		}
	}
	if (key == GLFW_KEY_W)
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				if (shape[i].y < 0.9f)
					shape[i].y += 0.02f;
			}
		}
	}
	else if (key == GLFW_KEY_A)
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				if (shape[i].x > -1.0f)
					shape[i].x -= 0.02f;
			}
		}
	}
	else if (key == GLFW_KEY_S)
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				if (shape[i].y > -1.0f)
					shape[i].y -= 0.02f;
			}
		}
	}
	else if (key == GLFW_KEY_D)
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				if (shape[i].x < 1.0f)
					shape[i].x += 0.02f;
			}
		}
	}

	if (key == GLFW_KEY_I) // 좌상
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				shape[i].x -= 0.02f;
				shape[i].y += 0.02f;

				if (shape[i].x <= -1.0f)
					shape[i].x = -1.0f;
				if (shape[i].y >= 1.0f)
					shape[i].y = 1.0f;
			}
		}
	}
	else if (key == GLFW_KEY_O) //우상
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				shape[i].x += 0.02f;
				shape[i].y += 0.02f;

				if (shape[i].x >= 1.0f)
					shape[i].x = 1.0f;
				if (shape[i].y >= 1.0f)
					shape[i].y = 1.0f;
			}
		}
	}
	else if (key == GLFW_KEY_J) // 좌하
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				shape[i].x -= 0.02f;
				shape[i].y -= 0.02f;

				if (shape[i].x <= -1.0f)
					shape[i].x = -1.0f;
				if (shape[i].y <= -1.0f)
					shape[i].y = -1.0f;
			}
		}
	}
	else if (key == GLFW_KEY_L) // 우하
	{
		for (int i = 0; i < shapeCount; i++)
		{
			if (shape[i].isSelect)
			{
				shape[i].x += 0.02f;
				shape[i].y -= 0.02f;

				if (shape[i].x >= 1.0f)
					shape[i].x = 1.0f;
				if (shape[i].y <= -1.0f)
					shape[i].y = -1.0f;
			}
		}
	}

	if (key == GLFW_KEY_1)
	{
		for (int i = 0; i < shapeCount; i++)
		{

			if (shape[i].y < 0.9f)
				shape[i].y += 0.02f;

		}
	}
	else if (key == GLFW_KEY_2)
	{
		for (int i = 0; i < shapeCount; i++)
		{

			if (shape[i].x > -1.0f)
				shape[i].x -= 0.02f;

		}
	}
	else if (key == GLFW_KEY_3)
	{
		for (int i = 0; i < shapeCount; i++)
		{

			if (shape[i].y > -1.0f)
				shape[i].y -= 0.02f;

		}
	}
	else if (key == GLFW_KEY_4)
	{
		for (int i = 0; i < shapeCount; i++)
		{

			if (shape[i].x < 1.0f)
				shape[i].x += 0.02f;

		}
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		for (int i = 0; i < shapeCount; i++)
		{
			shape[i].isAlive = false;
		}
		shapeCount = 0;
	}

}

//--- 마우스 버튼 입력 콜백 함수
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double mousex, mousey;
		int windowx, windowy;
		glfwGetCursorPos(window, &mousex, &mousey);
		glfwGetWindowSize(window, &windowx, &windowy);

		float x = -1.0f + 2.0f * (float)(mousex / windowx);
		float y = 1.0f - 2.0f * (float)(mousey / windowy);

		for (int i = 0; i < shapeCount; i++)
			shape[i].isSelect = false;

		for (int i = shapeCount - 1; i >= 0; i--)
		{
			bool isClicked = false;
			if (shape[i].type == 0)
			{
				float range = 0.03f;
				if (x >= shape[i].x - range && x <= shape[i].x + range &&
					y >= shape[i].y - range && y <= shape[i].y + range)
					isClicked = true;
			}
			else if (shape[i].type == 1)
			{
				float range = 0.03f;
				if (x >= shape[i].x - 0.1f && x <= shape[i].x + 0.1f &&
					y >= shape[i].y - range && y <= shape[i].y + range)
					isClicked = true;
			}
			else if (shape[i].type == 2)
			{
				if (x >= shape[i].x - 0.1f && x <= shape[i].x + 0.1f &&
					y >= shape[i].y - 0.1f && y <= shape[i].y + 0.1f)
					isClicked = true;
			}
			else if (shape[i].type == 3)
			{
				if (x >= shape[i].x - 0.1f && x <= shape[i].x + 0.1f &&
					y >= shape[i].y - 0.1f && y <= shape[i].y + 0.1f)
					isClicked = true;
			}

			if (isClicked)
			{
				shape[i].isSelect = true;
				break;
			}
		}
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
