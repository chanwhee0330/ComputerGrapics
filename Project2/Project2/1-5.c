#include <GL/glew.h>
#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int acction, int mods);
void GetCursorPose(GLFWwindow* window, double xpos, double ypos);

typedef struct Rect {
	float x, y, size, r, g, b;
	bool isalive;
}Rect;

Rect rect[50];
Rect eraser;
int rectCount,createCount;
float size;

int main(void)
{
	srand((unsigned int)time(NULL));
	GLFWwindow* window;
	GLenum glewResult;

	//--- GLFW 초기화
	if (glfwInit() != GLFW_TRUE) {
		printf("GLFW initialization failed.\n");
		return EXIT_FAILURE;
	}

	//--- OpenGL 버전 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	//--- 윈도우 생성
	window = glfwCreateWindow(1200, 1200, "OpenGL Basic Window (C)", NULL, NULL);
	if (window == NULL) {
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
	if (glewResult != GLEW_OK) {
		printf("GLEW initialization failed: %s\n",
			(const char*)glewGetErrorString(glewResult));
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, GetCursorPose);
	//--- 뷰포트 설정
	glViewport(0, 0, 1200, 1200);

	rectCount = rand() % 20 + 20;
	for (int i = 0; i < rectCount; i++)
	{
		rect[i].size = 0.01f;
		rect[i].x = (-1.0f + rect[i].size) + (2.0f - 2.0f * rect[i].size) * ((float)rand() / RAND_MAX);
		rect[i].y = (-1.0f + rect[i].size) + (2.0f - 2.0f * rect[i].size) * ((float)rand() / RAND_MAX);

		rect[i].isalive = true;
		rect[i].r = (float)rand() / RAND_MAX;
		rect[i].g = (float)rand() / RAND_MAX;
		rect[i].b = (float)rand() / RAND_MAX;
	}
	size = 0.05f;
	eraser.size = size;
	eraser.r = 0.0f;
	eraser.g = 0.0f;
	eraser.b = 0.0f;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//--- 메인 루프
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		//--- 배경색 설정 후 화면 지우기
		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < rectCount; i++)
		{
			if (rect[i].isalive == true)
			{
				glColor3f(rect[i].r, rect[i].g, rect[i].b);
				glRectf(rect[i].x - rect[i].size, rect[i].y - rect[i].size, rect[i].x + rect[i].size, rect[i].y + rect[i].size);
			}

			if (eraser.isalive == true&&rect[i].isalive==true)
			{
				if (rect[i].x >= eraser.x - eraser.size && rect[i].x <= eraser.x + eraser.size && rect[i].y >= eraser.y - eraser.size && rect[i].y <= eraser.y + eraser.size)
				{
					rect[i].isalive = false;
					eraser.r = rect[i].r;
					eraser.g = rect[i].g;
					eraser.b = rect[i].b;
					eraser.size += 0.01f;
					break;
				}
			}
		}

		if (eraser.isalive == true)
		{
			glColor3f(eraser.r, eraser.g, eraser.b);
			glRectf(eraser.x - eraser.size, eraser.y - eraser.size, eraser.x + eraser.size, eraser.y + eraser.size);
		}

		//--- ESC 키를 누르면 종료
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		//--- 버퍼 교체 및 이벤트 처리
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//--- 종료 처리
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		rectCount = rand() % 20 + 20;
		for (int i = 0; i < rectCount; i++)
		{
			rect[i].size = 0.01f;
			rect[i].x = (-1.0f + rect[i].size) + (2.0f - 2.0f * rect[i].size) * ((float)rand() / RAND_MAX);
			rect[i].y = (-1.0f + rect[i].size) + (2.0f - 2.0f * rect[i].size) * ((float)rand() / RAND_MAX);

			rect[i].isalive = true;
			rect[i].r = (float)rand() / RAND_MAX;
			rect[i].g = (float)rand() / RAND_MAX;
			rect[i].b = (float)rand() / RAND_MAX;
		}
		eraser.size = size;
		eraser.r = 0.0f;
		eraser.g = 0.0f;
		eraser.b = 0.0f;
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		eraser.isalive = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double openx, openy;
		float x, y;
		int winx, winy;
		glfwGetCursorPos(window, &openx, &openy);
		glfwGetWindowSize(window, &winx, &winy);
		x = -1.0f + 2.0f * (float)(openx / winx);
		y = 1.0f - 2.0f * (float)(openy / winy);
		if (createCount < 10)
		{
			rect[rectCount].x = x;
			rect[rectCount].y = y;
			rect[rectCount].size = 0.01f;
			rect[rectCount].isalive = true;

			rect[rectCount].r = (float)rand() / RAND_MAX;
			rect[rectCount].g = (float)rand() / RAND_MAX;
			rect[rectCount].b = (float)rand() / RAND_MAX;
			rectCount++;
			size -= 0.004f;
		}
		createCount++;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		eraser.isalive = false;
		for (int i = 0; i < rectCount; i++)
		{
			rect[i].isalive = true;
		}
			eraser.size = size;
			eraser.r = 0.0f;
			eraser.g = 0.0f;
			eraser.b = 0.0f;
	}
}

void GetCursorPose(GLFWwindow* window, double xpos, double ypos)
{
		float x, y;
		int winx, winy;
		glfwGetWindowSize(window, &winx, &winy);
		x = -1.0f + 2.0f * (float)(xpos / winx);
		y = 1.0f - 2.0f * (float)(ypos / winy);
		eraser.x = x;
		eraser.y = y;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

	}
}
