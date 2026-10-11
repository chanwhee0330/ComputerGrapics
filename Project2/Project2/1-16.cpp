#include <GL/glew.h>
#include <GL/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

//--- 셰이더 관련 함수
std::string filetobuf(const char* filePath);
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();

// 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void GetCursorPose(GLFWwindow* window, double xpos, double ypos);

//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO;
GLuint VBO;

bool ish = true, shape, isT, isS, is_1, is_2, is_3, is_4,is_7,is_8;
int isX, radiX, radiY, radiZ, isF, radiF, radiS, radiT, isB, set1, set2,is_6,is_9,count7,set8;
float moveX, moveY, isY, radiB, sizeB = 1.0f, radi1, radi2, radi3, radi4,radi6,radi7,radi8,radi9;

const GLfloat vertices[] = {
	// 면 이름은 변환 전 좌표 기준: 앞 -Z, 뒤 +Z, 오른쪽 +X, 왼쪽 -X, 위 +Y, 아래 -Y
	// 정육면체 앞면 (z = -0.5)
	 0.5f,-0.5f,-0.5f,  0,1,1,
	-0.5f,-0.5f,-0.5f,  1,0,1,
	-0.5f, 0.5f,-0.5f,  1,0.5f,0,

	 0.5f,-0.5f,-0.5f,  0,1,1,
	-0.5f, 0.5f,-0.5f,  1,0.5f,0,
	 0.5f, 0.5f,-0.5f,  1,1,1,

	 // 정육면체 오른쪽 면 (x = +0.5)
	  0.5f,-0.5f, 0.5f,  0,1,0,
	  0.5f,-0.5f,-0.5f,  0,1,1,
	  0.5f, 0.5f,-0.5f,  1,1,1,

	  0.5f,-0.5f, 0.5f,  0,1,0,
	  0.5f, 0.5f,-0.5f,  1,1,1,
	  0.5f, 0.5f, 0.5f,  0,0,1,

	  // 정육면체 뒷면 (z = +0.5)
	  -0.5f,-0.5f, 0.5f,  1,0,0,
	   0.5f,-0.5f, 0.5f,  0,1,0,
	   0.5f, 0.5f, 0.5f,  0,0,1,

	  -0.5f,-0.5f, 0.5f,  1,0,0,
	   0.5f, 0.5f, 0.5f,  0,0,1,
	  -0.5f, 0.5f, 0.5f,  1,1,0,

	  // 정육면체 왼쪽 면 (x = -0.5)
	  -0.5f,-0.5f,-0.5f,  1,0,1,
	  -0.5f,-0.5f, 0.5f,  1,0,0,
	  -0.5f, 0.5f, 0.5f,  1,1,0,

	  -0.5f,-0.5f,-0.5f,  1,0,1,
	  -0.5f, 0.5f, 0.5f,  1,1,0,
	  -0.5f, 0.5f,-0.5f,  1,0.5f,0,

	  // 정육면체 윗면 (y = +0.5)
	  -0.5f, 0.5f, 0.5f,  1,1,0,
	   0.5f, 0.5f, 0.5f,  0,0,1,
	   0.5f, 0.5f,-0.5f,  1,1,1,

	  -0.5f, 0.5f, 0.5f,  1,1,0,
	   0.5f, 0.5f,-0.5f,  1,1,1,
	  -0.5f, 0.5f,-0.5f,  1,0.5f,0,

	  // 정육면체 아랫면 (y = -0.5)
	  -0.5f,-0.5f,-0.5f,  1,0,1,
	   0.5f,-0.5f,-0.5f,  0,1,1,
	   0.5f,-0.5f, 0.5f,  0,1,0,

	  -0.5f,-0.5f,-0.5f,  1,0,1,
	   0.5f,-0.5f, 0.5f,  0,1,0,
	  -0.5f,-0.5f, 0.5f,  1,0,0,

	  // X축: 빨강
	  -1.5f,0.0f,0.0f,  1,0,0,
	   1.5f,0.0f,0.0f,  1,0,0,

	   // Y축: 초록
	   0.0f,-1.5f,0.0f,  0,1,0,
	   0.0f, 1.5f,0.0f,  0,1,0,

	   // Z축: 파랑
	   0.0f,0.0f,-1.5f,  0,0,1,
	   0.0f,0.0f, 1.5f,  0,0,1,

	   // 사각뿔 뒷면 (밑변 z = +0.5)
	-0.5f, -0.5f,  0.5f,  1, 0, 0,
	 0.5f, -0.5f,  0.5f,  1, 0, 0,
	 0.0f,  0.5f,  0.0f,  1, 0, 0,

	 // 사각뿔 오른쪽 면 (밑변 x = +0.5)
	  0.5f, -0.5f,  0.5f,  0, 1, 0,
	  0.5f, -0.5f, -0.5f,  0, 1, 0,
	  0.0f,  0.5f,  0.0f,  0, 1, 0,

	  // 사각뿔 앞면 (밑변 z = -0.5)
	   0.5f, -0.5f, -0.5f,  0, 0, 1,
	  -0.5f, -0.5f, -0.5f,  0, 0, 1,
	   0.0f,  0.5f,  0.0f,  0, 0, 1,

	   // 사각뿔 왼쪽 면 (밑변 x = -0.5)
	   -0.5f, -0.5f, -0.5f,  1, 1, 0,
	   -0.5f, -0.5f,  0.5f,  1, 1, 0,
		0.0f,  0.5f,  0.0f,  1, 1, 0,

		// 사각뿔 아랫면 (y = -0.5): 삼각형 2개
		-0.5f, -0.5f,  0.5f,  1, 0, 1,
		-0.5f, -0.5f, -0.5f,  1, 0, 1,
		 0.5f, -0.5f, -0.5f,  1, 0, 1,

		-0.5f, -0.5f,  0.5f,  1, 0, 1,
		 0.5f, -0.5f, -0.5f,  1, 0, 1,
		 0.5f, -0.5f,  0.5f,  1, 0, 1
};

void InitBuffer();

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

	InitBuffer();

	glEnable(GL_DEPTH_TEST);

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
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

//--- 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	//--- 세이더 코드 읽어오기
	std::string vertexSource = filetobuf("vertex7.glsl");
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
	std::string fragmentSource = filetobuf("fragment7.glsl");
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
	// 1. 배경 지우기
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. 사용할 셰이더와 정점 데이터 선택
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	// 3. 셰이더에 선언한 uniform 변수의 위치 가져오기
	GLint modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	GLint colorLocation = glGetUniformLocation(shaderProgramID, "vColor");

	// 4. 정육면체 전체 회전
	glm::mat4 cube(1.0f);
	cube = glm::rotate(cube, glm::radians(-30.0f), glm::vec3(1, 0, 0));
	cube = glm::rotate(cube, glm::radians(-30.0f), glm::vec3(0, 1, 0));

	// model: 사각형 한 면의 변환
	// transform: 면 배치 -> 정육면체 전체 회전 (오른쪽부터 적용)
	glm::mat4 model(1.0f);
	glm::mat4 transform(1.0f);

	if (isX == 1)
	{
		radiX += 1;
		model = glm::rotate(model, glm::radians((float)radiX), glm::vec3(1, 0, 0));
	}
	else if (isX == 2)
	{
		radiX -= 1;
		model = glm::rotate(model, glm::radians((float)radiX), glm::vec3(1, 0, 0));
	}

	if (isY)
	{
		radiY += 1;
		model = glm::rotate(model, glm::radians((float)radiY), glm::vec3(0, 1, 0));
	}
	transform = cube * model;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));


	if (shape == false)
	{
		glm::mat4 temp(1.0f);
		glm::mat4 matT(1.0f);
		glm::mat4 matF(1.0f);
		glm::mat4 matS(1.0f);
		glm::mat4 matB(1.0f);
		temp = transform;
		if (is_1 == false && is_2 == false && is_3 == false && is_4 == false)
		{
			if (isF == 1)
			{
				if (radiF < 90)
					radiF++;
				matF = glm::translate(matF, glm::vec3(0.5f, 0.5f, -0.5f));
				matF = glm::rotate(matF, glm::radians((float)radiF), glm::vec3(1, 0, 0));
				matF = glm::translate(matF, glm::vec3(-0.5f, -0.5f, 0.5f));
				transform = temp * matF;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6); // 정육면체 앞면 (z = -0.5)
			}
			else if (isF == 2)
			{
				if (radiF > 0)
					radiF--;
				matF = glm::translate(matF, glm::vec3(0.5f, 0.5f, -0.5f));
				matF = glm::rotate(matF, glm::radians((float)radiF), glm::vec3(1, 0, 0));
				matF = glm::translate(matF, glm::vec3(-0.5f, -0.5f, 0.5f));
				transform = temp * matF;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			}
			glDrawArrays(GL_TRIANGLES, 0, 6); // 정육면체 앞면 (z = -0.5)
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 6, 6); // 정육면체 오른쪽 면 (x = +0.5)
			glDrawArrays(GL_TRIANGLES, 12, 6); // 정육면체 뒷면 (z = +0.5)
			if (isS)
			{
				radiS++;
				matS = glm::rotate(matS, glm::radians((float)radiS), glm::vec3(1, 0, 0));
				transform = temp * matS;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			}
			else if (isB == 1)
			{
				radiB++;
				if (sizeB > 0)
					sizeB -= 0.001f;
				matB = glm::rotate(matB, glm::radians(radiB), glm::vec3(1, 0, 0));
				matB = glm::translate(matB, glm::vec3(-0.5f, 0, 0));
				matB = glm::scale(matB, glm::vec3(sizeB));
				matB = glm::translate(matB, glm::vec3(0.5f, 0, 0));
				transform = temp * matB;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			}
			else if (isB == 2)
			{
				if (sizeB < 1.0f)
				{
					radiB++;
					sizeB += 0.001f;
				}
				else
					radiB = 0;
				matB = glm::rotate(matB, glm::radians(radiB), glm::vec3(1, 0, 0));
				matB = glm::translate(matB, glm::vec3(-0.5f, 0, 0));
				matB = glm::scale(matB, glm::vec3(sizeB));
				matB = glm::translate(matB, glm::vec3(0.5f, 0, 0));
				transform = temp * matB;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			}
			glDrawArrays(GL_TRIANGLES, 18, 6); // 정육면체 왼쪽 면 (x = -0.5)
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			if (isT)
			{
				radiT += 1;
				matT = glm::translate(matT, glm::vec3(0, 0.5f, 0));
				matT = glm::rotate(matT, glm::radians((float)radiT), glm::vec3(0, 0, 1));
				matT = glm::translate(matT, glm::vec3(0, -0.5f, 0));
				transform = cube * matT;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			}
			glDrawArrays(GL_TRIANGLES, 24, 6); // 정육면체 윗면 (y = +0.5)
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 30, 6); // 정육면체 아랫면 (y = -0.5)
		}
		else if (is_1)
		{
			if (set1 == 0)
			{
				glm::mat4 mat_1(1.0f);
				radi1++;
				mat_1 = glm::translate(mat_1, glm::vec3(0.5f, 0, -0.5f));
				mat_1 = glm::rotate(mat_1, glm::radians(radi1), glm::vec3(0, 1, 0));
				mat_1 = glm::translate(mat_1, glm::vec3(-0.5f, 0, 0.5f));
				transform = temp * mat_1;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 12);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 24);
			}
			else if (set1 == 1)
			{
				glm::mat4 mat_1(1.0f);
				radi1++;
				mat_1 = glm::translate(mat_1, glm::vec3(0.5f, 0, 0.5f));
				mat_1 = glm::rotate(mat_1, glm::radians(radi1), glm::vec3(0, 1, 0));
				mat_1 = glm::translate(mat_1, glm::vec3(-0.5f, 0, -0.5f));
				transform = temp * mat_1;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 12);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 18, 18);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			else if (set1 == 2)
			{
				glm::mat4 mat_1(1.0f);
				radi1++;
				mat_1 = glm::translate(mat_1, glm::vec3(-0.5f, 0, 0.5f));
				mat_1 = glm::rotate(mat_1, glm::radians(radi1), glm::vec3(0, 1, 0));
				mat_1 = glm::translate(mat_1, glm::vec3(0.5f, 0, -0.5f));
				transform = temp * mat_1;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 12);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 24, 12);
				glDrawArrays(GL_TRIANGLES, 0, 12);
			}
			else if (set1 == 3)
			{
				glm::mat4 mat_1(1.0f);
				radi1++;
				mat_1 = glm::translate(mat_1, glm::vec3(-0.5f, 0, -0.5f));
				mat_1 = glm::rotate(mat_1, glm::radians(radi1), glm::vec3(0, 1, 0));
				mat_1 = glm::translate(mat_1, glm::vec3(0.5f, 0, 0.5f));
				transform = temp * mat_1;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 12);
				glDrawArrays(GL_TRIANGLES, 24, 12);
			}
		}
		else if (is_2)
		{
			if (set2 == 0)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 24);
			}
			else if (set2 == 1)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 24, 6);
				glDrawArrays(GL_TRIANGLES, 30, 6);
			}
			else if (set2 == 2)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 18, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 24, 6);
				glDrawArrays(GL_TRIANGLES, 30, 6);
			}
			else if (set2 == 3)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 1, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 18, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 24, 6);
				glDrawArrays(GL_TRIANGLES, 30, 6);
			}
			else if (set2 == 4)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 24, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 30, 6);
			}
			else if (set2 == 5)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 24, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 30, 6);
			}
			else if (set2 == 6)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 24, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 30, 6);
			}
			else if (set2 == 7)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 18, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 24, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glDrawArrays(GL_TRIANGLES, 30, 6);
			}
			else if (set2 == 8)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 30, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 24, 6);
			}
			else if (set2 == 9)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 6, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 30, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 24, 6);
			}
			else if (set2 == 10)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 12, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(1, 0, 0));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 30, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 18, 6);
				glDrawArrays(GL_TRIANGLES, 24, 6);
			}
			else if (set2 == 11)
			{
				glm::mat4 mat_2(1.0f);
				if (radi2 < 90)
					radi2++;
				mat_2 = glm::rotate(mat_2, glm::radians(radi2), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 18, 6);
				mat_2 = glm::mat4(1.0f);
				mat_2 = glm::rotate(mat_2, glm::radians(radi2 * -1), glm::vec3(0, 0, 1));
				transform = temp * mat_2;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 30, 6);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDrawArrays(GL_TRIANGLES, 12, 6);
				glDrawArrays(GL_TRIANGLES, 6, 6);
				glDrawArrays(GL_TRIANGLES, 24, 6);
			}
		}
		else if (is_3)
		{
			glm::mat4 mat_3(1.0f);
			radi3++;
			mat_3 = glm::rotate(mat_3, glm::radians(radi3), glm::vec3(0, 1, 0));
			transform = temp * mat_3;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 0, 24);
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 24, 12);
		}
		else if (is_4)
		{
			glm::mat4 mat_3(1.0f);
			radi3++;
			mat_3 = glm::rotate(mat_3, glm::radians(radi3), glm::vec3(1, 0, 0));
			transform = temp * mat_3;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDrawArrays(GL_TRIANGLES, 12, 6);
			glDrawArrays(GL_TRIANGLES, 24, 12);
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 6, 6);
			glDrawArrays(GL_TRIANGLES, 18, 6);
		}
	}
	else
	{
		glm::mat4 temp(1.0f);
		temp = transform;
		if (is_6 == 1)
		{
			if (radi6 > -233)
				radi6--;
			glm::mat4 mat_6(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(0.0, -0.5f, -0.5f));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6), glm::vec3(1, 0, 0));
			mat_6 = glm::translate(mat_6, glm::vec3(0.0, 0.5f, 0.5f));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 48, 3);
			mat_6 = glm::mat4(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, -0.5f, 0));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6 * -1), glm::vec3(0, 0, 1));
			mat_6 = glm::translate(mat_6, glm::vec3(0.5f, 0.5f, 0));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 51, 3);
			mat_6 = glm::mat4(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(0, -0.5f, 0.5f));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6 * -1), glm::vec3(1, 0, 0));
			mat_6 = glm::translate(mat_6, glm::vec3(0, 0.5f, -0.5f));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 42, 3);
			mat_6 = glm::mat4(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(0.5f, -0.5f, 0));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6), glm::vec3(0, 0, 1));
			mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, 0.5f, 0));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 45, 3);
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 54, 6);

		}
		else if (is_6 == 2)
		{
			if (radi6 <0)
				radi6++;
			glm::mat4 mat_6(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(0.0, -0.5f, -0.5f));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6), glm::vec3(1, 0, 0));
			mat_6 = glm::translate(mat_6, glm::vec3(0.0, 0.5f, 0.5f));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 48, 3);
			mat_6 = glm::mat4(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, -0.5f, 0));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6 * -1), glm::vec3(0, 0, 1));
			mat_6 = glm::translate(mat_6, glm::vec3(0.5f, 0.5f, 0));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 51, 3);
			mat_6 = glm::mat4(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(0, -0.5f, 0.5f));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6 * -1), glm::vec3(1, 0, 0));
			mat_6 = glm::translate(mat_6, glm::vec3(0, 0.5f, -0.5f));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 42, 3);
			mat_6 = glm::mat4(1.0f);
			mat_6 = glm::translate(mat_6, glm::vec3(0.5f, -0.5f, 0));
			mat_6 = glm::rotate(mat_6, glm::radians(radi6), glm::vec3(0, 0, 1));
			mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, 0.5f, 0));
			transform = temp * mat_6;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 45, 3);
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 54, 6);
		}
		else if (is_7)
		{
			if (count7 == 0)
			{
				if (radi7 > -90)
					radi7--;
				else
					count7++;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(0.0, -0.5f, -0.5f));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7), glm::vec3(1, 0, 0));
				mat_6 = glm::translate(mat_6, glm::vec3(0.0, 0.5f, 0.5f));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 48, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 51, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
			else if (count7 == 1)
			{
				if (radi7 < 0)
					radi7++;
				else
					count7++;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(0.0, -0.5f, -0.5f));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7), glm::vec3(1, 0, 0));
				mat_6 = glm::translate(mat_6, glm::vec3(0.0, 0.5f, 0.5f));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 48, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 51, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
			else if (count7 == 2)
			{
				if (radi7 > -90)
					radi7--;
				else
					count7++;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::mat4(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, -0.5f, 0));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7 * -1), glm::vec3(0, 0, 1));
				mat_6 = glm::translate(mat_6, glm::vec3(0.5f, 0.5f, 0));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 51, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
			else if (count7 == 3)
			{
				if (radi7 < 0)
					radi7++;
				else
					count7++;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::mat4(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, -0.5f, 0));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7 * -1), glm::vec3(0, 0, 1));
				mat_6 = glm::translate(mat_6, glm::vec3(0.5f, 0.5f, 0));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 51, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
			else if (count7 == 4)
			{
				if (radi7 > -90)
					radi7--;
				else
					count7++;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(0, -0.5f, 0.5f));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7 * -1), glm::vec3(1, 0, 0));
				mat_6 = glm::translate(mat_6, glm::vec3(0, 0.5f, -0.5f));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 51, 3);
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
			else if (count7 == 5)
			{

				if (radi7 < 0)
					radi7++;
				else
					count7++;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(0, -0.5f, 0.5f));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7 * -1), glm::vec3(1, 0, 0));
				mat_6 = glm::translate(mat_6, glm::vec3(0, 0.5f, -0.5f));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 51, 3);
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
			else if (count7 == 6)
			{
				if (radi7 > -90)
					radi7--;
				else
					count7++;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(0.5f, -0.5f, 0));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7), glm::vec3(0, 0, 1));
				mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, 0.5f, 0));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 45, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 51, 3);
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
			else if (count7 == 7)
			{
				if (radi7 < 0)
					radi7++;
				else
					count7=0;
				glm::mat4 mat_6(1.0f);
				mat_6 = glm::translate(mat_6, glm::vec3(0.5f, -0.5f, 0));
				mat_6 = glm::rotate(mat_6, glm::radians(radi7), glm::vec3(0, 0, 1));
				mat_6 = glm::translate(mat_6, glm::vec3(-0.5f, 0.5f, 0));
				transform = temp * mat_6;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 45, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 51, 3);
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);
			}
		}
		else if (is_8)
		{
			if (set8 == 0)
			{
				if(radi8<180)
					radi8++;
				glm::mat4 mat_8(1.0f);
				mat_8 = glm::rotate(mat_8, glm::radians(radi8), glm::vec3(0, 1, 0));
				transform = temp * mat_8;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 51, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);

			}
			else if (set8 == 1)
			{
				if (radi8 < 180)
					radi8++;
				glm::mat4 mat_8(1.0f);
				mat_8 = glm::rotate(mat_8, glm::radians(radi8), glm::vec3(0, 1, 0));
				transform = temp * mat_8;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 45, 3);
				glDrawArrays(GL_TRIANGLES, 51, 3);
				transform = temp;
				glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
				glDrawArrays(GL_TRIANGLES, 42, 3);
				glDrawArrays(GL_TRIANGLES, 48, 3);
				glDrawArrays(GL_TRIANGLES, 54, 6);

			}
		}
		else if (is_9==1)
		{
			if (radi9 < 63.43495f)
				radi9 = glm::min(radi9 + 1.0f, 63.43495f);
			glm::mat4 mat_9(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9), glm::vec3(1, 0, 0));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 48, 3);
			mat_9 = glm::mat4(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9 * -1), glm::vec3(0, 0, 1));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 51, 3);
			mat_9 = glm::mat4(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9 * -1), glm::vec3(1, 0, 0));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 42, 3);
			mat_9 = glm::mat4(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9), glm::vec3(0, 0, 1));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 45, 3);
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 54, 6);
		}
		else if (is_9 == 2)
		{
			if (radi9 > 0)
				radi9 = glm::max(radi9 - 1.0f, 0.0f);
			glm::mat4 mat_9(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9), glm::vec3(1, 0, 0));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 48, 3);
			mat_9 = glm::mat4(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9 * -1), glm::vec3(0, 0, 1));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 51, 3);
			mat_9 = glm::mat4(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9 * -1), glm::vec3(1, 0, 0));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 42, 3);
			mat_9 = glm::mat4(1.0f);
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, 0.5f, 0.0f));
			mat_9 = glm::rotate(mat_9, glm::radians(radi9), glm::vec3(0, 0, 1));
			mat_9 = glm::translate(mat_9, glm::vec3(0.0f, -0.5f, 0.0f));
			transform = temp * mat_9;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 45, 3);
			transform = temp;
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 54, 6);
		}
		else
			glDrawArrays(GL_TRIANGLES, 42, 18); // 사각뿔 전체 (옆면 4개 + 아랫면)

	}
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cube));
	glDrawArrays(GL_LINES, 36, 6);
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		ish = !ish;
		if (ish)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		isY = !isY;
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		shape = !shape;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		shape = false, ish = true, isY = isT = false;
	}
	else if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		isT = !isT;
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		isF++;
		if (isF > 2)
			isF = 0;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		isS = !isS;
	}
	else if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		isB++;
		if (isB > 2)
		{
			isB = 0;
			sizeB = 1.0f;
		}
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		is_2 = false;
		radi2 = 0;
		is_3 = false;
		radi3 = 0;
		is_4 = false;
		radi4 = 0;
		is_1 = !is_1;
		set1 = rand() % 4;
		radi1 = 0;
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		is_1 = false;
		radi1 = 0;
		is_3 = false;
		radi3 = 0;
		is_4 = false;
		radi4 = 0;
		is_2 = !is_2;
		set2 = rand() % 12;
		radi2 = 0;
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		is_1 = false;
		radi1 = 0;
		is_2 = false;
		radi2 = 0;
		is_4 = false;
		radi4 = 0;
		is_3 = !is_3;
		radi3 = 0;
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		is_1 = false;
		radi1 = 0;
		is_2 = false;
		radi2 = 0;
		is_3 = false;
		radi3 = 0;
		is_4 = !is_4;
		radi4 = 0;
	}
	else if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		shape = true;
		is_7 = false;
		radi7 = 0;
		count7 = 0;
		is_8 = false;
		radi8 = 0;
		is_9 = 0;
		radi9 = 0;
		is_6++;
		if (is_6 > 2)
		{
			is_6 = 0;
			radi6 = 0;
		}
	}
	else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		shape = true;
		is_6 = 0;
		radi6 = 0;
		is_8 = false;
		radi8 = 0;
		is_9 = 0;
		radi9 = 0;
		is_7 = !is_7;
		radi7 = 0;
		count7 = 0;
	}
	else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		shape = true;
		is_6 = 0;
		radi6 = 0;
		is_7 = false;
		radi7 = 0;
		count7 = 0;
		is_9 = 0;
		radi9 = 0;
		is_8 = !is_8;
		set8 = rand() % 2;
		radi8 = 0;
	}
	else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		shape = true;
		is_6 = 0;
		radi6 = 0;
		is_7 = false;
		radi7 = 0;
		count7 = 0;
		is_8 = false;
		radi8 = 0;
		is_9++;
		if (is_9 > 2)
		{
			is_9 = 0;
			radi9 = 0;
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

void InitBuffer()
{
	glBindVertexArray(VAO);

	// 정점 데이터를 저장할 VBO 생성
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// 좌표 배열을 GPU로 전달
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Location 0에서 정점마다 float 6개씩 읽도록 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
