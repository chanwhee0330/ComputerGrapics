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
	GLfloat x, y, r, g, b, scale;
	GLint shapeType;
	bool isRight,isClick;
}Shape;

Shape shape[40];

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

	for (int i = 0; i < 14; i++)
	{
		shape[i].r = (float)rand() / RAND_MAX;
		shape[i].g = (float)rand() / RAND_MAX;
		shape[i].b = (float)rand() / RAND_MAX;

		if (i < 4)
		{
			shape[i].shapeType = 0;
			shape[i].scale = 1.0f;
			shape[i].x = -0.95f + (float)rand() / RAND_MAX * 1.4f;
			shape[i].y = -0.95f + (float)rand() / RAND_MAX * 1.9f;
		}
		else if (i < 8)
		{
			if(i==4)
				shape[i].shapeType = 1;
			else if ( i==5)
				shape[i].shapeType = 2;
			else if ( i==6)
				shape[i].shapeType = 3;
			else if (i ==7)
				shape[i].shapeType = 4;
			shape[i].scale = 1.0f;
			shape[i].x = -0.9f + (float)rand() / RAND_MAX * 1.3f;
			shape[i].y = -0.92f + (float)rand() / RAND_MAX * 1.76f;
		}
		else if(i < 9)
		{
			shape[i].shapeType = 5;
			shape[i].scale = 0.8f;
			shape[i].x = -0.88f + (float)rand() / RAND_MAX * 1.26f;
			shape[i].y = -0.944f + (float)rand() / RAND_MAX * 1.888f;
		}
		else if (i < 10)
		{
			shape[i].shapeType = 6;
			shape[i].scale = 0.8f;
			shape[i].x = -0.88f + (float)rand() / RAND_MAX * 1.26f;
			shape[i].y = -0.944f + (float)rand() / RAND_MAX * 1.888f;
		}
		else if (i == 10)
		{
			shape[i].shapeType = 0;
			shape[i].scale = 1.0f;
			shape[i].x = -0.95f + (float)rand() / RAND_MAX * 1.4f;
			shape[i].y = -0.95f + (float)rand() / RAND_MAX * 1.9f;
		}
		else if (i == 11)
		{
			shape[i].shapeType = 1;
			shape[i].scale = 1.0f;
			shape[i].x = -0.9f + (float)rand() / RAND_MAX * 1.3f;
			shape[i].y = -0.92f + (float)rand() / RAND_MAX * 1.76f;
		}
		else if (i == 12)
		{
			shape[i].shapeType = 1;
			shape[i].scale = 1.0f;
			shape[i].x = -0.9f + (float)rand() / RAND_MAX * 1.3f;
			shape[i].y = -0.92f + (float)rand() / RAND_MAX * 1.76f;
		}
		else if (i == 13)
		{
			shape[i].shapeType = 2;
			shape[i].scale = 1.0f;
			shape[i].x = -0.9f + (float)rand() / RAND_MAX * 1.3f;
			shape[i].y = -0.92f + (float)rand() / RAND_MAX * 1.76f;
		}
	}

	for (int i = 20; i < 34; i++)
	{
		if (i < 24)
		{
			shape[i].shapeType = 0;
			shape[i].scale = 1.0f;
			shape[i].x = 0.70f + ((i - 20) % 2) * 0.10f;
			shape[i].y = 0.90f - ((i - 20) / 2) * 0.10f;
		}
		else if (i < 28)
		{
			shape[i].scale = 1.0f;
			if (i == 24)
			{
				shape[i].shapeType = 1;
				shape[i].x = 0.75f;
				shape[i].y = 0.09f;
			}
			else if (i == 25)
			{
				shape[i].shapeType = 2;
				shape[i].x = 0.75f;
				shape[i].y = 0.41f;
			}
			else if (i == 26)
			{
				shape[i].shapeType = 3;
				shape[i].x = 0.91f;
				shape[i].y = 0.25f;
			}
			else if (i == 27)
			{	
				shape[i].shapeType = 4;
				shape[i].x = 0.59f;
				shape[i].y = 0.25f;
			}
		}
		else if( i <30)
		{
			shape[i].scale = 0.8f;
			shape[i].x = 0.75f;

			if (i == 28)
			{
				shape[i].y = -0.07f;
				shape[i].shapeType = 5;
			}
			else
			{
				shape[i].y = -0.05f;
				shape[i].shapeType = 6;
			}
		}
		else if (i == 30)
		{
			shape[i].shapeType = 1;
			shape[i].scale = 1.0f;
			shape[i].x = 0.75f;
			shape[i].y = -0.28f;
		}
		else if (i == 31)
		{
			shape[i].shapeType = 0;
			shape[i].scale = 1.0f;
			shape[i].x = 0.75f;
			shape[i].y = -0.40f;
		}
		else if (i == 32)
		{
			shape[i].shapeType = 1;
			shape[i].scale = 1.0f;
			shape[i].x = 0.75f;
			shape[i].y = -0.66f;
		}
		else if (i == 33)
		{
			shape[i].shapeType = 2;
			shape[i].scale = 1.0f;
			shape[i].x = 0.75f;
			shape[i].y = -0.82f;
		}
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
	std::string vertexSource = filetobuf("vertex3.glsl");
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
	glPointSize(50.0f);

	glUniform2f(offsetLocation, 0.0f, 0.0f);
	glUniform1f(scaleLocation, 1.0f);
	glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 19, 2);
	for (int i = 20; i < 34; i++)
	{
		glUniform2f(offsetLocation, shape[i].x, shape[i].y);
		glUniform4f(colorLocation, 0, 0, 0, 1.0f);
		glUniform1f(scaleLocation, shape[i].scale);
		if (shape[i].shapeType == 0)
			glDrawArrays(GL_POINTS, 0, 1);
		else if (shape[i].shapeType == 1)
			glDrawArrays(GL_TRIANGLES, 1, 3);
		else if (shape[i].shapeType == 2)
			glDrawArrays(GL_TRIANGLES, 4, 3);
		else if (shape[i].shapeType == 3)
			glDrawArrays(GL_TRIANGLES, 7, 3);
		else if (shape[i].shapeType == 4)
			glDrawArrays(GL_TRIANGLES, 10, 3);
		else if (shape[i].shapeType == 5)
			glDrawArrays(GL_TRIANGLES, 13, 3);
		else if (shape[i].shapeType == 6)
			glDrawArrays(GL_TRIANGLES, 16, 3);
	}
	
	for (int i = 0; i < 14; i++)
	{
		glUniform2f(offsetLocation, shape[i].x, shape[i].y);
		glUniform4f(colorLocation, shape[i].r, shape[i].g, shape[i].b, 1.0f);
		glUniform1f(scaleLocation, shape[i].scale);
		if (shape[i].shapeType == 0)
			glDrawArrays(GL_POINTS, 0, 1);
		else if (shape[i].shapeType == 1)
			glDrawArrays(GL_TRIANGLES, 1, 3);
		else if (shape[i].shapeType == 2)
			glDrawArrays(GL_TRIANGLES, 4, 3);
		else if (shape[i].shapeType == 3)
			glDrawArrays(GL_TRIANGLES, 7, 3);
		else if (shape[i].shapeType == 4)
			glDrawArrays(GL_TRIANGLES, 10, 3);
		else if (shape[i].shapeType == 5)
			glDrawArrays(GL_TRIANGLES, 13, 3);
		else if (shape[i].shapeType == 6)
			glDrawArrays(GL_TRIANGLES, 16, 3);
		
	}
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		for (int i = 0; i < 14; i++)
		{
			shape[i].r = (float)rand() / RAND_MAX;
			shape[i].g = (float)rand() / RAND_MAX;
			shape[i].b = (float)rand() / RAND_MAX;

			shape[i].isClick = false;
			shape[i].isRight = false;

			if (i < 4)
			{
				shape[i].shapeType = 0;
				shape[i].scale = 1.0f;
				shape[i].x = -0.95f + (float)rand() / RAND_MAX * 1.4f;
				shape[i].y = -0.95f + (float)rand() / RAND_MAX * 1.9f;
			}
			else if (i < 8)
			{
				if (i == 4)
					shape[i].shapeType = 1;
				else if (i == 5)
					shape[i].shapeType = 2;
				else if (i == 6)
					shape[i].shapeType = 3;
				else
					shape[i].shapeType = 4;
				shape[i].scale = 1.0f;
				shape[i].x = -0.9f + (float)rand() / RAND_MAX * 1.3f;
				shape[i].y = -0.92f + (float)rand() / RAND_MAX * 1.76f;
			}
			else if (i < 9)
			{
				shape[i].shapeType = 5;
				shape[i].scale = 0.8f;
				shape[i].x = -0.88f + (float)rand() / RAND_MAX * 1.26f;
				shape[i].y = -0.944f + (float)rand() / RAND_MAX * 1.888f;
			}
			else if (i < 10)
			{
				shape[i].shapeType = 6;
				shape[i].scale = 0.8f;
				shape[i].x = -0.88f + (float)rand() / RAND_MAX * 1.26f;
				shape[i].y = -0.944f + (float)rand() / RAND_MAX * 1.888f;
			}
			else if (i == 10)
			{
				shape[i].shapeType = 0;
				shape[i].scale = 1.0f;
				shape[i].x = -0.95f + (float)rand() / RAND_MAX * 1.4f;
				shape[i].y = -0.95f + (float)rand() / RAND_MAX * 1.9f;
			}
			else if (i == 11 || i == 12)
			{
				shape[i].shapeType = 1;
				shape[i].scale = 1.0f;
				shape[i].x = -0.9f + (float)rand() / RAND_MAX * 1.3f;
				shape[i].y = -0.92f + (float)rand() / RAND_MAX * 1.76f;
			}
			else if (i == 13)
			{
				shape[i].shapeType = 2;
				shape[i].scale = 1.0f;
				shape[i].x = -0.9f + (float)rand() / RAND_MAX * 1.3f;
				shape[i].y = -0.92f + (float)rand() / RAND_MAX * 1.76f;
			}
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
		
		for (int i = 13; i >= 0; i--)
		{
			if (shape[i].shapeType == 0 && shape[i].isRight == false)
			{
				if (x > shape[i].x - 0.05f && x < shape[i].x + 0.05f &&
					y > shape[i].y - 0.05f && y < shape[i].y + 0.05f)
				{
					shape[i].isClick = true;
					break;
				}
			}
			else if (shape[i].shapeType == 1 && shape[i].isRight == false)
			{
				if (x > shape[i].x - 0.1f * shape[i].scale && x < shape[i].x + 0.1f * shape[i].scale &&
					y > shape[i].y - 0.08f * shape[i].scale && y < shape[i].y + 0.16f * shape[i].scale)
				{
					shape[i].isClick = true;
					break;
				}
			}
			else if (shape[i].shapeType == 2 && shape[i].isRight == false)
			{
				if (x > shape[i].x - 0.1f * shape[i].scale && x < shape[i].x + 0.1f * shape[i].scale &&
					y > shape[i].y - 0.16f * shape[i].scale && y < shape[i].y + 0.08f * shape[i].scale)
				{
					shape[i].isClick = true;
					break;
				}
			}
			else if (shape[i].shapeType == 3 && shape[i].isRight == false)
			{
				if (x > shape[i].x - 0.16f * shape[i].scale && x < shape[i].x + 0.08f * shape[i].scale &&
					y > shape[i].y - 0.1f * shape[i].scale && y < shape[i].y + 0.1f * shape[i].scale)
				{
					shape[i].isClick = true;
					break;
				}
			}
			else if (shape[i].shapeType == 4 && shape[i].isRight == false)
			{
				if (x > shape[i].x - 0.08f * shape[i].scale && x < shape[i].x + 0.16f * shape[i].scale &&
					y > shape[i].y - 0.1f * shape[i].scale && y < shape[i].y + 0.1f * shape[i].scale)
				{
					shape[i].isClick = true;
					break;
				}
			}
			else if ((shape[i].shapeType == 5 || shape[i].shapeType == 6) && shape[i].isRight == false)
			{
				if (x > shape[i].x - 0.15f * shape[i].scale && x < shape[i].x + 0.15f * shape[i].scale &&
					y > shape[i].y - 0.07f * shape[i].scale && y < shape[i].y + 0.07f * shape[i].scale)
				{
					shape[i].isClick = true;
					break;
				}
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		for (int i = 0; i < 14; i++)
		{
			for (int j = 20; j < 34; j++)
			{
				if (shape[i].shapeType == 0 && shape[i].isRight == false)
				{
					if (shape[i].x >= shape[j].x - 0.05f && shape[i].x <= shape[j].x + 0.05f &&
						shape[i].y >= shape[j].y - 0.05f && shape[i].y <= shape[j].y + 0.05f &&
						shape[j].shapeType == 0)
					{
						shape[i].x = shape[j].x;
						shape[i].y = shape[j].y;
						shape[i].isRight = true;
						shape[i].r = shape[i].r * 0.7f + 0.15f;
						shape[i].g = shape[i].g * 0.7f + 0.15f;
						shape[i].b = shape[i].b * 0.7f + 0.15f;
					}
				}
				else if (shape[i].shapeType == 1 && shape[i].isRight == false)
				{
					if (shape[i].x >= shape[j].x - 0.10f * shape[j].scale && shape[i].x <= shape[j].x + 0.10f * shape[j].scale &&
						shape[i].y >= shape[j].y - 0.08f * shape[j].scale && shape[i].y <= shape[j].y + 0.16f * shape[j].scale &&
						shape[j].shapeType == 1)
					{
						shape[i].x = shape[j].x;
						shape[i].y = shape[j].y;
						shape[i].isRight = true;
						shape[i].r = shape[i].r * 0.7f + 0.15f;
						shape[i].g = shape[i].g * 0.7f + 0.15f;
						shape[i].b = shape[i].b * 0.7f + 0.15f;
					}
				}
				else if (shape[i].shapeType == 2 && shape[i].isRight == false)
				{
					if (shape[i].x >= shape[j].x - 0.10f * shape[j].scale && shape[i].x <= shape[j].x + 0.10f * shape[j].scale &&
						shape[i].y >= shape[j].y - 0.16f * shape[j].scale && shape[i].y <= shape[j].y + 0.08f * shape[j].scale &&
						shape[j].shapeType == 2)
					{
						shape[i].x = shape[j].x;
						shape[i].y = shape[j].y;
						shape[i].isRight = true;
						shape[i].r = shape[i].r * 0.7f + 0.15f;
						shape[i].g = shape[i].g * 0.7f + 0.15f;
						shape[i].b = shape[i].b * 0.7f + 0.15f;
					}
				}
				else if (shape[i].shapeType == 3 && shape[i].isRight == false)
				{
					if (shape[i].x >= shape[j].x - 0.16f * shape[j].scale && shape[i].x <= shape[j].x + 0.08f * shape[j].scale &&
						shape[i].y >= shape[j].y - 0.10f * shape[j].scale && shape[i].y <= shape[j].y + 0.10f * shape[j].scale &&
						shape[j].shapeType == 3)
					{
						shape[i].x = shape[j].x;
						shape[i].y = shape[j].y;
						shape[i].isRight = true;
						shape[i].r = shape[i].r * 0.7f + 0.15f;
						shape[i].g = shape[i].g * 0.7f + 0.15f;
						shape[i].b = shape[i].b * 0.7f + 0.15f;
					}
				}
				else if (shape[i].shapeType == 4 && shape[i].isRight == false)
				{
					if (shape[i].x >= shape[j].x - 0.08f * shape[j].scale && shape[i].x <= shape[j].x + 0.16f * shape[j].scale &&
						shape[i].y >= shape[j].y - 0.10f * shape[j].scale && shape[i].y <= shape[j].y + 0.10f * shape[j].scale &&
						shape[j].shapeType == 4)
					{
						shape[i].x = shape[j].x;
						shape[i].y = shape[j].y;
						shape[i].isRight = true;
						shape[i].r = shape[i].r * 0.7f + 0.15f;
						shape[i].g = shape[i].g * 0.7f + 0.15f;
						shape[i].b = shape[i].b * 0.7f + 0.15f;
					}
				}
				else if (shape[i].shapeType == 5 && shape[i].isRight == false)
				{
					if (shape[i].x >= shape[j].x - 0.15f * shape[j].scale && shape[i].x <= shape[j].x + 0.15f * shape[j].scale &&
						shape[i].y >= shape[j].y - 0.07f * shape[j].scale && shape[i].y <= shape[j].y + 0.07f * shape[j].scale &&
						shape[j].shapeType == 5)
					{
						shape[i].x = shape[j].x;
						shape[i].y = shape[j].y;
						shape[i].isRight = true;
						shape[i].r = shape[i].r * 0.7f + 0.15f;
						shape[i].g = shape[i].g * 0.7f + 0.15f;
						shape[i].b = shape[i].b * 0.7f + 0.15f;
					}
				}
				else if (shape[i].shapeType == 6 && shape[i].isRight == false)
				{
					if (shape[i].x >= shape[j].x - 0.15f * shape[j].scale && shape[i].x <= shape[j].x + 0.15f * shape[j].scale &&
						shape[i].y >= shape[j].y - 0.07f * shape[j].scale && shape[i].y <= shape[j].y + 0.07f * shape[j].scale &&
						shape[j].shapeType == 6)
					{
						shape[i].x = shape[j].x;
						shape[i].y = shape[j].y;
						shape[i].isRight = true;
						shape[i].r = shape[i].r * 0.7f + 0.15f;
						shape[i].g = shape[i].g * 0.7f + 0.15f;
						shape[i].b = shape[i].b * 0.7f + 0.15f;
					}
				}
			}
			shape[i].isClick = false;
		}
	}
}

//--- 마우스 위치 입력 콜백 함수
void GetCursorPose(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		double mousex, mousey;
		int windowx, windowy;
		glfwGetCursorPos(window, &mousex, &mousey);
		glfwGetWindowSize(window, &windowx, &windowy);
		float x = -1.0f + 2.0f * (float)(mousex / windowx);
		float y = 1.0f - 2.0f * (float)(mousey / windowy);
		for (int i = 0; i < 14; i++)
		{
			if (shape[i].isClick)
			{
				shape[i].x = x;
				shape[i].y = y;
			}
		}
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
