#include <GL/glew.h>
#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int acction, int mods);
void GetCursorPose(GLFWwindow* window, double xpos, double ypos);
void changeColor(int i);

typedef struct Rect {
	float x, y, sizex,sizey,r,g,b;
	int move,color,random;
	bool isAlive;
}Rect;

typedef struct Block {
	Rect rect[8];
	bool isSelect;
}Block;

Block block[10];

int rectCount;

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

	rectCount = rand() % 6 + 5;
	for (int i = 0; i < rectCount; i++)
	{
		float r, g, b,x,y,sizex,sizey;
		float minsize = 0.025f, maxsize = 0.05f;

		sizex = minsize + (maxsize - minsize) * ((float)rand() / RAND_MAX);
		sizey = minsize + (maxsize - minsize) * ((float)rand() / RAND_MAX);

		x = (-1.0f + sizex) + (2.0f - 8.0f * sizex) * ((float)rand() / RAND_MAX);
		y = (-1.0f + sizey) + (2.0f - 4.0f * sizey) * ((float)rand() / RAND_MAX);

		r = (float)rand() / RAND_MAX;
		g = (float)rand() / RAND_MAX;
		b = (float)rand() / RAND_MAX;
		for (int j = 0; j < 8; j++)
		{
			block[i].rect[j].sizex = sizex;
			block[i].rect[j].sizey = sizey;
			if (j < 4)
			{
				block[i].rect[j].x = x + sizex * j*2;
				block[i].rect[j].y = y;
			}
			else
			{
				block[i].rect[j].x = x + sizex * (j%4)*2;
				block[i].rect[j].y = y + sizey*2;
			}

			block[i].rect[j].r = r;
			block[i].rect[j].g = g;
			block[i].rect[j].b = b;
			block[i].rect[j].isAlive = true;
		}
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//--- 메인 루프
	while (glfwWindowShouldClose(window) == GLFW_FALSE) {
		//--- 배경색 설정 후 화면 지우기
		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < rectCount; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (block[i].isSelect == true)
				{
					if (block[i].rect[j].move == 0) // 위 아래로 이동
					{
						if (j >= 0 && j <= 1)
						{
							block[i].rect[j].y-=0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 0)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
						else if (j >= 2 && j <= 3)
						{
							block[i].rect[j].y -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 2)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
						else if (j == 4 || j == 5)
						{
							block[i].rect[j].y += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 4)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
						else
						{
							block[i].rect[j].y += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 6)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}


					}
					else if (block[i].rect[j].move == 1) // 왼쪽 오른쪽으로 이동
					{
						if (j == 0 || j == 1)
						{
							block[i].rect[j].x -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 0)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;

						}
						else if (j == 2 || j == 3)
						{
							block[i].rect[j].x += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 2)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
						else if (j == 4 || j == 5)
						{
							block[i].rect[j].x -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 4)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
						else
						{
							block[i].rect[j].x += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 6)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
					}
					else if (block[i].rect[j].move == 2) // 대각선으로 이동
					{
						if (j == 0 || j == 1)
						{
							block[i].rect[j].x -= 0.002f;
							block[i].rect[j].y -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 0)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;

						}
						else if (j == 2 || j == 3)
						{
							block[i].rect[j].x += 0.002f;
							block[i].rect[j].y -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 2)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
						else if (j == 4 || j == 5)
						{
							block[i].rect[j].x -= 0.002f;
							block[i].rect[j].y += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 4)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
						else
						{
							block[i].rect[j].x += 0.002f;
							block[i].rect[j].y += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
							if (j == 6)
								block[i].rect[j].x += 0.0005f;
							else
								block[i].rect[j].x -= 0.0005f;
						}
					}
					else if (block[i].rect[j].move == 3) // 왼쪽이동
					{
						block[i].rect[j].x -= 0.002f;
						block[i].rect[j].sizex -= 0.0005f;
						block[i].rect[j].sizey -= 0.0005f;
						if (j % 2 == 0)
							block[i].rect[j].x += 0.0005f;
						else
							block[i].rect[j].x -= 0.0005f;
					}
					else if (block[i].rect[j].move == 4) // 오른쪽이동
					{
						block[i].rect[j].x += 0.002f;
						block[i].rect[j].sizex -= 0.0005f;
						block[i].rect[j].sizey -= 0.0005f;
						if (j % 2 == 0)
							block[i].rect[j].x += 0.0005f;
						else
							block[i].rect[j].x -= 0.0005f;
					}
					else if (block[i].rect[j].move == 5) // 위 이동
					{
						block[i].rect[j].y += 0.002f;
						block[i].rect[j].sizex -= 0.0005f;
						block[i].rect[j].sizey -= 0.0005f;
						if (j % 2 == 0)
							block[i].rect[j].x += 0.0005f;
						else
							block[i].rect[j].x -= 0.0005f;
					}
					else if (block[i].rect[j].move == 6) // 아래 이동
					{
						block[i].rect[j].y -= 0.002f;
						block[i].rect[j].sizex -= 0.0005f;
						block[i].rect[j].sizey -= 0.0005f;
						if (j % 2 == 0)
							block[i].rect[j].x += 0.0005f;
						else
							block[i].rect[j].x -= 0.0005f;
					}
					else // 8조각 
					{
						if (block[i].rect[j].random == 0)
						{
							block[i].rect[j].x -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						else if (block[i].rect[j].random == 1)
						{
							block[i].rect[j].x += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						else if (block[i].rect[j].random == 2)
						{
							block[i].rect[j].y -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						else if (block[i].rect[j].random == 3)
						{
							block[i].rect[j].y += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						else if (block[i].rect[j].random == 4)
						{
							block[i].rect[j].x -= 0.002f;
							block[i].rect[j].y += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						else if (block[i].rect[j].random == 5)
						{
							block[i].rect[j].x += 0.002f;
							block[i].rect[j].y += 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						else if (block[i].rect[j].random == 6)
						{
							block[i].rect[j].x += 0.002f;
							block[i].rect[j].y -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						else if (block[i].rect[j].random == 7)
						{
							block[i].rect[j].x -= 0.002f;
							block[i].rect[j].y -= 0.002f;
							block[i].rect[j].sizex -= 0.0005f;
							block[i].rect[j].sizey -= 0.0005f;
						}
						
					}

					if (block[i].rect[j].sizex <= 0||block[i].rect[j].sizey<=0)
						block[i].rect[j].isAlive = false;
					changeColor(i);
				}


				if (block[i].rect[j].isAlive == true)
				{
					glColor3f(block[i].rect[j].r, block[i].rect[j].g, block[i].rect[j].b);
					glRectf(block[i].rect[j].x - block[i].rect[j].sizex, block[i].rect[j].y - block[i].rect[j].sizey, block[i].rect[j].x + block[i].rect[j].sizex, block[i].rect[j].y + block[i].rect[j].sizey);
				}

			}

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
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{

	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double mousex, mousey;
		float x, y;
		int winx, winy;
		glfwGetCursorPos(window, &mousex, &mousey);
		glfwGetWindowSize(window, &winx, &winy);
		x = -1.0f + 2.0f * (float)(mousex / winx);
		y = 1.0f - 2.0f * (float)(mousey / winy);
		for (int i = 0; i < rectCount; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if (x >= block[i].rect[j].x - block[i].rect[j].sizex && x <= block[i].rect[j].x + block[i].rect[j].sizex && y >= block[i].rect[j].y - block[i].rect[j].sizey && y <= block[i].rect[j].y + block[i].rect[j].sizey)
				{
					block[i].isSelect = true;
					int move = rand() % 8;
					if (move <= 6)
					{
						for (int j = 0;j < 8;j+=2)
						{
							block[i].rect[j].move = move;
							block[i].rect[j].color = rand() % 2;
							block[i].rect[j+1].move = move;
							block[i].rect[j + 1].color = block[i].rect[j].color;
						}
					}
					else
					{
						for (int j = 0; j < 8; j++)
						{
							block[i].rect[j].move = move;
							block[i].rect[j].color = rand() % 2;
							block[i].rect[j].random = rand() % 8;
						}
					}
				}
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

void GetCursorPose(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

	}
}


void changeColor(int i)
{
		for (int j = 0;j < 8;j++)
		{
			if (block[i].rect[j].color == 0)
			{
				block[i].rect[j].r += 0.00125f;
				block[i].rect[j].g += 0.00125f;
				block[i].rect[j].b += 0.00125f;

				if (block[i].rect[j].r >= 1.0f)
					block[i].rect[j].r = 1.0f;
				if (block[i].rect[j].g >= 1.0f)
					block[i].rect[j].g = 1.0f;
				if (block[i].rect[j].b >= 1.0f)
					block[i].rect[j].b = 1.0f;
			}
			else
			{
				block[i].rect[j].r -= 0.00125f;
				block[i].rect[j].g -= 0.00125f;
				block[i].rect[j].b -= 0.00125f;

				if (block[i].rect[j].r <= 0.0f)
					block[i].rect[j].r = 0.0f;
				if (block[i].rect[j].g <= 0.0f)
					block[i].rect[j].g = 0.0f;
				if (block[i].rect[j].b <= 0.0f)
					block[i].rect[j].b = 0.0f;
			}
		}
}