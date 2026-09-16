#include <GL/glew.h>
#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int acction, int mods);
void GetCursorPose(GLFWwindow* window, double xpos, double ypos);

typedef struct Rect{
	float x, y, sizex, sizey, oldx, oldy, r, g, b, speedx, speedy,targetY;
	int move;
	bool is4, is5;
	bool movex, movey, growing,movingdown;
}Rect;

Rect rect[5];
int rectCount;
bool is1, is2, is3, is4, is5;

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
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		//--- 배경색 설정 후 화면 지우기
		glClear(GL_COLOR_BUFFER_BIT);


		//--- ESC 키를 누르면 종료
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		for (int i = 0; i < rectCount; i++)
		{
			if (is1 == true)
			{
				
				if (rect[i].x-rect[i].sizex < -1.0f)
				{
					rect[i].movex = true;
				}
				else if (rect[i].x+ rect[i].sizex > 1.0f)
				{
					rect[i].movex = false;
				}

				if (rect[i].y - rect[i].sizey < -1.0f)
				{
					rect[i].movey = true;
				}
				else if (rect[i].y + rect[i].sizey > 1.0f)
				{
					rect[i].movey = false;
				}

				if (rect[i].movex == true)
					rect[i].x = rect[i].x + rect[i].speedx;
				else if (rect[i].movex == false)
					rect[i].x = rect[i].x - rect[i].speedx;

				if (rect[i].movey == true)
					rect[i].y = rect[i].y + rect[i].speedy;
				else if (rect[i].movey == false)
					rect[i].y = rect[i].y - rect[i].speedy;
			}
			else if (is2 == true)
			{
				if (rect[i].movingdown == true)
				{
					rect[i].y -= rect[i].speedy;
					if (rect[i].y <= rect[i].targetY)
					{
						rect[i].y = rect[i].targetY;
						rect[i].movingdown = false;
					}
				}
				else if (rect[i].movex == true)
				{
					rect[i].x += rect[i].speedx;
					if (rect[i].x + rect[i].sizex >= 1.0f)
					{
						rect[i].x = 1.0f - rect[i].sizex;
						rect[i].movex = false;

						rect[i].targetY = rect[i].y - 0.05f;
						rect[i].movingdown = true;
					}
				}
				else
				{
					rect[i].x -= rect[i].speedx;
					if (rect[i].x - rect[i].sizex <= -1.0f)
					{
						rect[i].x = -1.0f + rect[i].sizex;
						rect[i].movex = true;

						rect[i].targetY = rect[i].y - 0.05f;
						rect[i].movingdown = true;
					}
				}

				if (rect[i].y - rect[i].sizey <= -1.0f)
				{
					rect[i].y = 1.0f - rect[i].sizey;
					rect[i].movingdown = false;
				}
			}
			else if (is3 == true)
			{
				if (rect[i].move == 0) //위로
				{
					rect[i].y += rect[i].speedy;
					if (rect[i].y + rect[i].sizey >= 1.0f)
					{
						rect[i].y = 1.0f - rect[i].sizey;
						rect[i].move = 1;
					}
				}
				else if (rect[i].move == 1)
				{
					rect[i].x += rect[i].speedx;
					if (rect[i].x + rect[i].sizex >= 1.0f)
					{
						rect[i].x = 1.0f - rect[i].sizex;
						rect[i].move = 2;
					}
				}
				else if (rect[i].move == 2)
				{
					rect[i].y -= rect[i].speedy;
					if (rect[i].y - rect[i].sizey <= -1.0f)
					{
						rect[i].y = -1.0f + rect[i].sizey;
						rect[i].move = 3;
					}
				}
				else if (rect[i].move == 3)
				{
					rect[i].x -= rect[i].speedx;
					if (rect[i].x - rect[i].sizex <= -1.0f)
					{
						rect[i].x = -1.0f + rect[i].sizex;
						rect[i].move = 0;
					}
				}
			}

			if (is4 == true)
			{
				const float sizeSpeed = 0.0008f;

				if (rect[i].growing == true)
				{
					rect[i].sizex += sizeSpeed;
					rect[i].sizey += sizeSpeed;
					if (rect[i].sizex >= 0.1f)
					{
						rect[i].sizex = 0.1f;
						rect[i].sizey = 0.1f;
						rect[i].growing = false;
					}
				}
				else
				{
					rect[i].sizex -= sizeSpeed;
					rect[i].sizey -= sizeSpeed;
					if (rect[i].sizex <= 0.02f)
					{
						rect[i].sizex = 0.02f;
						rect[i].sizey = 0.02f;
						rect[i].growing = true;
					}
				}
			}
			if (is5 == true)
			{
				rect[i].r += 0.0010f;
				rect[i].g += 0.0015f;
				rect[i].b += 0.0020f;

				if (rect[i].r > 1.0f)
					rect[i].r -= 1.0f;
				if (rect[i].g > 1.0f)
					rect[i].g -= 1.0f;
				if (rect[i].b > 1.0f)
					rect[i].b -= 1.0f;
			}
			glColor3f(rect[i].r, rect[i].g, rect[i].b);
			
			glRectf(rect[i].x-rect[i].sizex,rect[i].y-rect[i].sizey,rect[i].x+rect[i].sizex,rect[i].y+rect[i].sizey);

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
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		if (is1 == false)
		{
			is1 = true;
			is2 = false;
			is3 = false;
		}
		else if (is1 == true)
			is1 = false;
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		if (is2 == false)
		{
			is1 = false;
			is2 = true;
			is3 = false;
		}
		else if (is2 == true)
			is2 = false;
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		if (is3 == false)
		{
			is1 = false;
			is2 = false;
			is3 = true;
		}
		else if (is3 == true)
			is3 = false;
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		if (is4 == false)
			is4 = true;
		else
			is4 = false;
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		if (is5 == false)
			is5 = true;
		else
			is5 = false;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		is4 = false;
		is5 = false;
	}
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		for (int i = 0; i < rectCount; i++)
		{
			rect[i].x = rect[i].oldx;
			rect[i].y = rect[i].oldy;
		}
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		rectCount = 0;
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double openx, openy;
		int windowx, windowy;
		glfwGetCursorPos(window, &openx, &openy);
		glfwGetWindowSize(window, &windowx, &windowy);
		float x = -1.0f + 2.0f * (float)(openx / windowx);
		float y = 1.0f - 2.0f * (float)(openy / windowy);
		if (rectCount < 5)
		{
			rect[rectCount].oldx = x;
			rect[rectCount].oldy = y;
			rect[rectCount].x = rect[rectCount].oldx;
			rect[rectCount].y = rect[rectCount].oldy;
			rect[rectCount].sizex = 0.05f;
			rect[rectCount].sizey = 0.05f;
			rect[rectCount].speedx = 0.002f + 0.004f * ((float)rand() / RAND_MAX);
			rect[rectCount].speedy = 0.002f + 0.004f * ((float)rand() / RAND_MAX);
			
			rect[rectCount].r = (float)rand() / RAND_MAX;
			rect[rectCount].g = (float)rand() / RAND_MAX;
			rect[rectCount].b = (float)rand() / RAND_MAX;
			rect[rectCount].movex = true;
			rect[rectCount].movey = false;
			rect[rectCount].growing = true;
			rect[rectCount].movingdown = false;
			rect[rectCount].targetY = y;
			rectCount++;
		}
		
	}
}

void GetCursorPose(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

	}
}
