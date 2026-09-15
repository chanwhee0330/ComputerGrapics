#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool settimer;
int main() {
	srand((unsigned int)time(NULL));
	//--- GLFW 초기화
	if (!glfwInit()) {
		std::cerr << "GLFW 초기화 실패!" << std::endl;
		return -1;
	}
	//--- OpenGL 버전 설정 (예: 3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//--- 윈도우 생성
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
	if (!window) {
		std::cerr << "윈도우 생성 실패!" << std::endl;
		printf("윈도우 생성 실패!");
		glfwTerminate();
		return -1;
	}
	//--- 컨텍스트 설정
	glfwMakeContextCurrent(window);
	//--- GLEW 초기화
	glewExperimental = GL_TRUE; // 최신 기능 사용
	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW 초기화 실패!" << std::endl;
		return -1;
	}
	//--- 뷰포트 설정
	glViewport(0, 0, 800, 600);

	glfwSetKeyCallback(window, keyCallback);
	//--- 메인 루프
	while (!glfwWindowShouldClose(window)) {
		// 입력 처리
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
			glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
			glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
		else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		if (settimer == true)
		{
			double timer = 0;
			timer = glfwGetTime();
			if (timer >=0.5f)
			{
				float r, g, b;
				r = (float)rand() / RAND_MAX;
				g = (float)rand() / RAND_MAX;
				b = (float)rand() / RAND_MAX;
				glClearColor(r, g, b, 1.0f);
				glfwSetTime(0.0);
			}
		}

		// 화면 지우기 (파란색)
		glClear(GL_COLOR_BUFFER_BIT);
		// 버퍼 교체
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//--- 종료 처리
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		float r, g, b;
		r = (float)rand() / RAND_MAX;
		g = (float)rand() / RAND_MAX;
		b = (float)rand() / RAND_MAX;
		glClearColor(r, g, b, 1.0f);
	}
	else if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		settimer = true;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		settimer = false;
	}
}