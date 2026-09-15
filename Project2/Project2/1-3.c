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
	float x, y, width, height, r, g, b;
	bool isselect;
}Rect;

Rect rect[20];
int count, rectcount;

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
	window = glfwCreateWindow(1500, 900, "OpenGL Basic Window (C)", NULL, NULL);
	if (window == NULL) {
		printf("Window creation failed.\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

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
	glViewport(0, 0, 1500, 900);

	//--- 메인 루프
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		//--- 배경색 설정 후 화면 지우기
		glClear(GL_COLOR_BUFFER_BIT);


		//--- ESC 키를 누르면 종료
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		for (int i = 0; i < rectcount; i++)
		{
			glColor3f(rect[i].r, rect[i].g, rect[i].b);
			glRectf(rect[i].x-rect[i].width, rect[i].y-rect[i].height, rect[i].x + rect[i].width, rect[i].y + rect[i].height);
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
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		if (count < 10 && rectcount < 20)
		{
			rect[rectcount].width = 0.05f + 0.1f * ((float)rand() / RAND_MAX);
			rect[rectcount].height = 0.05f + 0.1f * ((float)rand() / RAND_MAX);
			rect[rectcount].x = (-1.0+rect[rectcount].width) + (2.0f - 2.0f * rect[rectcount].width) * ((float)rand() / RAND_MAX);
			rect[rectcount].y = (-1.0+rect[rectcount].height) + (2.0f -2.0f * rect[rectcount].height) * ((float)rand() / RAND_MAX);

			rect[rectcount].r = (float)rand() / RAND_MAX;
			rect[rectcount].g = (float)rand() / RAND_MAX;
			rect[rectcount].b = (float)rand() / RAND_MAX;
			count++;
			rectcount++;
		}
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double mousex, mousey;
		int windowWidth, windowHeight;
		glfwGetCursorPos(window, &mousex, &mousey);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		float x = -1.0f + 2.0f * (float)(mousex / windowWidth);
		float y = 1.0f - 2.0f * (float)(mousey / windowHeight);
		for (int i = 0; i < rectcount; i++)
		{
			if (x >= rect[i].x-rect[i].width && x <= rect[i].x + rect[i].width && y >= rect[i].y-rect[i].height && y <= rect[i].y + rect[i].height)
			{
				rect[i].isselect = true;
				break;
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double mousex, mousey;
		int windowWidth, windowHeight;
		glfwGetCursorPos(window, &mousex, &mousey);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		float x = -1.0f + 2.0f * (float)(mousex / windowWidth);
		float y = 1.0f - 2.0f * (float)(mousey / windowHeight);
		for (int i = 0; i < rectcount; i++)
		{
			if (x >= rect[i].x - rect[i].width && x <= rect[i].x + rect[i].width && y >= rect[i].y - rect[i].height && y <= rect[i].y + rect[i].height)
			{
				if (rectcount < 20)
				{
					for (int j = i; j < rectcount - 1; j++)
						rect[j] = rect[j + 1];

					rectcount--;
					count--;

					for (int j = 0; j < 2; j++)
					{
						int index = rectcount;

						rect[index].width = 0.05f + 0.1f * ((float)rand() / RAND_MAX);
						rect[index].height = 0.05f + 0.1f * ((float)rand() / RAND_MAX);
						rect[index].x = (-1.0f + rect[index].width) + (2.0f - 2.0f * rect[index].width) * ((float)rand() / RAND_MAX);
						rect[index].y = (-1.0f + rect[index].height) + (2.0f - 2.0f * rect[index].height) * ((float)rand() / RAND_MAX);

						rect[index].r = (float)rand() / RAND_MAX;
						rect[index].g = (float)rand() / RAND_MAX;
						rect[index].b = (float)rand() / RAND_MAX;

						rect[index].isselect = false;

						rectcount++;
						count++;
					}
				}
				break;
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		for (int i = 0; i < rectcount; i++)
		{
			if (rect[i].isselect == true)
			{
				for (int j = 0; j < rectcount; j++)
				{
					if (rect[j].isselect == false)
					{
						if (rect[i].x - rect[i].width <= rect[j].x + rect[j].width &&
							rect[i].x + rect[i].width >= rect[j].x - rect[j].width &&
							rect[i].y - rect[i].height <= rect[j].y + rect[j].height &&
							rect[i].y + rect[i].height >= rect[j].y - rect[j].height)
						{
							float minX = rect[i].x - rect[i].width;
							float maxX = rect[i].x + rect[i].width;
							float minY = rect[i].y - rect[i].height;
							float maxY = rect[i].y + rect[i].height;

							if (rect[j].x - rect[j].width < minX)
								minX = rect[j].x - rect[j].width;
							if (rect[j].x + rect[j].width > maxX)
								maxX = rect[j].x + rect[j].width;
							if (rect[j].y - rect[j].height < minY)
								minY = rect[j].y - rect[j].height;
							if (rect[j].y + rect[j].height > maxY)
								maxY = rect[j].y + rect[j].height;

							rect[j].x = (minX + maxX) / 2.0f;
							rect[j].y = (minY + maxY) / 2.0f;
							rect[j].width = (maxX - minX) / 2.0f;
							rect[j].height = (maxY - minY) / 2.0f;

							rect[j].r = (float)rand() / RAND_MAX;
							rect[j].g = (float)rand() / RAND_MAX;
							rect[j].b = (float)rand() / RAND_MAX;
							for (int k = i; k < rectcount - 1; k++)
								rect[k] = rect[k + 1];

							rectcount--;
							count--;
							return;
						}
					}
				}
			}
			rect[i].isselect = false;

		}
	}
}

void GetCursorPose(GLFWwindow* window, double xpos, double ypos)
{
	int windowWidth, windowHeight;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		float x = -1.0f + 2.0f * (float)(xpos / windowWidth);
		float y = 1.0f - 2.0f * (float)(ypos / windowHeight);
		for (int i = 0; i < rectcount; i++)
		{
			if (rect[i].isselect == true)
			{
				if (x <= -1.0f + rect[i].width)
					x = -1.0f + rect[i].width;
				if (x >= 1.0f - rect[i].width)
					x = 1.0f - rect[i].width;
				if (y <= -1.0f + rect[i].height)
					y = -1.0f + rect[i].height;
				if (y >= 1.0f - rect[i].height)
					y = 1.0f - rect[i].height;

				rect[i].x = x;
				rect[i].y = y;
				break;
			}
		}
	}
}
