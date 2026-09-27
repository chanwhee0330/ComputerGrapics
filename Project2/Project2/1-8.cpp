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

typedef struct Triangle {
	GLfloat x, y,r,g,b,scale;
	GLint scaleCount;
	bool isAlive,isa;
}Triangle;

Triangle tri[4];

bool isa=true;

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
	std::string vertexSource = filetobuf("vertex1.glsl");
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

	glPointSize(10.0f);
	glLineWidth(3.0f);

	//--- 이 아래에 필요한 그리기 기능을 추가

	glUniform2f(offsetLocation, 0.0f, 0.0f);
	glUniform1f(scaleLocation, 1.0f);

	glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);

	glDrawArrays(GL_LINES, 3, 4);

	for (int i = 0; i < 4; i++)
	{
		glUniform2f(offsetLocation, tri[i].x, tri[i].y);
		glUniform1f(scaleLocation, tri[i].scale);
		glUniform4f(colorLocation,tri[i].r, tri[i].g, tri[i].b, 1.0f);
		if (tri[i].isAlive)
		{
			if (tri[i].isa)
				glDrawArrays(GL_TRIANGLES, 0, 3);
			else
				glDrawArrays(GL_LINE_LOOP, 0, 3);
		}
	}
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		isa = true;
	}
	else if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		isa = false;
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		for (int i = 0; i < 4; i++)
		{
			tri[i].isAlive = false;
		}
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

		if (x < 0 && y >= 0)
		{
			tri[0].x = x;
			tri[0].y = y;

			tri[0].r = (float)rand() / RAND_MAX;
			tri[0].g = (float)rand() / RAND_MAX;
			tri[0].b = (float)rand() / RAND_MAX;

			tri[0].scale = 0.5f + (float)rand() / RAND_MAX * 0.5f;
			tri[0].scaleCount = 0;

			tri[0].isAlive = true;
			if (isa == true)
				tri[0].isa = true;
			else
				tri[0].isa = false;
		}
		else if (x >= 0 && y >= 0)
		{
			tri[1].x = x;
			tri[1].y = y;

			tri[1].r = (float)rand() / RAND_MAX;
			tri[1].g = (float)rand() / RAND_MAX;
			tri[1].b = (float)rand() / RAND_MAX;

			tri[1].scale = 0.5f + (float)rand() / RAND_MAX * 0.5f;
			tri[1].scaleCount = 0;

			tri[1].isAlive = true;
			if (isa == true)
				tri[1].isa = true;
			else
				tri[1].isa = false;
		}
		else if (x >= 0 && y < 0)
		{
			tri[2].x = x;
			tri[2].y = y;

			tri[2].r = (float)rand() / RAND_MAX;
			tri[2].g = (float)rand() / RAND_MAX;
			tri[2].b = (float)rand() / RAND_MAX;

			tri[2].scale = 0.5f + (float)rand() / RAND_MAX * 0.5f;
			tri[2].scaleCount = 0;

			tri[2].isAlive = true;
			if (isa == true)
				tri[2].isa = true;
			else
				tri[2].isa = false;
		}
		else
		{
			tri[3].x = x;
			tri[3].y = y;

			tri[3].r = (float)rand() / RAND_MAX;
			tri[3].g = (float)rand() / RAND_MAX;
			tri[3].b = (float)rand() / RAND_MAX;

			tri[3].scale = 0.5f + (float)rand() / RAND_MAX * 0.5f;
			tri[3].scaleCount = 0;

			tri[3].isAlive = true;
			if (isa == true)
				tri[3].isa = true;
			else
				tri[3].isa = false;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double mousex, mousey;
		int windowx, windowy;
		glfwGetCursorPos(window, &mousex, &mousey);
		glfwGetWindowSize(window, &windowx, &windowy);
		float x = -1.0f + 2.0f * (float)(mousex / windowx);
		float y = 1.0f - 2.0f * (float)(mousey / windowy);

		if (x < 0 && y >= 0)
		{
			if (tri[0].scaleCount < 4)
			{
				tri[0].scale += 0.1f;
				tri[0].scaleCount++;
			}
			else if (tri[0].scaleCount < 8)
			{
				tri[0].scale -= 0.1f;
				tri[0].scaleCount++;
			}
			else
				tri[0].scaleCount = 0;
		}
		else if (x >= 0 && y >= 0)
		{
			if (tri[1].scaleCount < 4)
			{
				tri[1].scale += 0.1f;
				tri[1].scaleCount++;
			}
			else if (tri[1].scaleCount < 8)
			{
				tri[1].scale -= 0.1f;
				tri[1].scaleCount++;
			}
			else
				tri[1].scaleCount = 0;
		}
		else if (x >= 0 && y < 0)
		{
			if (tri[2].scaleCount < 4)
			{
				tri[2].scale += 0.1f;
				tri[2].scaleCount++;
			}
			else if (tri[2].scaleCount < 8)
			{
				tri[2].scale -= 0.1f;
				tri[2].scaleCount++;
			}
			else
				tri[2].scaleCount = 0;
		}
		else
		{
			if (tri[3].scaleCount < 4)
			{
				tri[3].scale += 0.1f;
				tri[3].scaleCount++;
			}
			else if (tri[3].scaleCount < 8)
			{
				tri[3].scale -= 0.1f;
				tri[3].scaleCount++;
			}
			else
				tri[3].scaleCount = 0;
		}
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
