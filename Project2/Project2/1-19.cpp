#include <GL/glew.h>
#include <GL/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
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

// 정점 하나: x, y, z만 저장 (큐브 36개). 색상은 drawScene()에서 지정한다.
const GLfloat vertices[] = {
	// 면 이름은 변환 전 좌표 기준: 앞 -Z, 뒤 +Z, 오른쪽 +X, 왼쪽 -X, 위 +Y, 아래 -Y
	// 정육면체 앞면 (z = -0.1)
	 0.1f, -0.1f, -0.1f,
	-0.1f, -0.1f, -0.1f,
	-0.1f, 0.1f, -0.1f,

	 0.1f, -0.1f, -0.1f,
	-0.1f, 0.1f, -0.1f,
	 0.1f, 0.1f, -0.1f,

	 // 정육면체 오른쪽 면 (x = +0.1)
	  0.1f, -0.1f, 0.1f,
	  0.1f, -0.1f, -0.1f,
	  0.1f, 0.1f, -0.1f,

	  0.1f, -0.1f, 0.1f,
	  0.1f, 0.1f, -0.1f,
	  0.1f, 0.1f, 0.1f,

	  // 정육면체 뒷면 (z = +0.1)
	  -0.1f, -0.1f, 0.1f,
	   0.1f, -0.1f, 0.1f,
	   0.1f, 0.1f, 0.1f,

	  -0.1f, -0.1f, 0.1f,
	   0.1f, 0.1f, 0.1f,
	  -0.1f, 0.1f, 0.1f,

	  // 정육면체 왼쪽 면 (x = -0.1)
	  -0.1f, -0.1f, -0.1f,
	  -0.1f, -0.1f, 0.1f,
	  -0.1f, 0.1f, 0.1f,

	  -0.1f, -0.1f, -0.1f,
	  -0.1f, 0.1f, 0.1f,
	  -0.1f, 0.1f, -0.1f,

	  // 정육면체 윗면 (y = +0.1)
	  -0.1f, 0.1f, 0.1f,
	   0.1f, 0.1f, 0.1f,
	   0.1f, 0.1f, -0.1f,

	  -0.1f, 0.1f, 0.1f,
	   0.1f, 0.1f, -0.1f,
	  -0.1f, 0.1f, -0.1f,

	  // 정육면체 아랫면 (y = -0.1)
	  -0.1f, -0.1f, -0.1f,
	   0.1f, -0.1f, -0.1f,
	   0.1f, -0.1f, 0.1f,

	  -0.1f, -0.1f, -0.1f,
	   0.1f, -0.1f, 0.1f,
	  -0.1f, -0.1f, 0.1f,

};

const GLsizei cubeVertexCount = 36;

void InitBuffer();

float moveX, moveZ, sizeD, rotateT, leftx = -0.5f, rightx = 0.5f, tempx, rotateG, rotateP;
int isD, rotateDir = 1, camerax = 0, cameray = 0, cameraz = 0, isY, isR;
float cameraR = 0.0f;
bool isT, isI, isG, isP;

glm::vec3 lowerColor(0.251f, 0.251f, 0.251f); // 아래 몸체
glm::vec3 centerColor(0.827f, 0.827f, 0.827f); // 중앙 몸체
glm::vec3 upperColor(0.0f, 1.0f, 0.0f);       // 상부 몸체
glm::vec3 poleColor(0.769f, 0.604f, 0.0f);    // 깃대
glm::vec3 barrelColor(1.0f, 0.980f, 0.804f);  // 포신

// RGB 각각 0~1 사이의 무작위 값 생성
glm::vec3 randomColor()
{
	return glm::vec3(
		rand() / static_cast<float>(RAND_MAX),
		rand() / static_cast<float>(RAND_MAX),
		rand() / static_cast<float>(RAND_MAX));
}

int main(void)
{
	srand(static_cast<unsigned int>(std::time(nullptr)));
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
	if (shaderProgramID == 0)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	//--- 메인 루프
	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		//--- 화면 출력
		drawScene();

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
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	if (width == 0 || height == 0) return;
	glViewport(0, 0, width, height);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float aspect = static_cast<float>(width) / height;

	glm::mat4 projection = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 100.0f);

	glm::mat4 view;
	if (isR == false)
		view = glm::lookAt(glm::vec3(camerax, cameray + 0.5f, cameraz + 5.0f), glm::vec3(camerax + 5.0f * glm::sin(glm::radians(cameraR)), cameray, cameraz + 5.0f - 5.0f * glm::cos(glm::radians(cameraR))), glm::vec3(0.0f, 1.0f, 0.0f));
	else
		view = glm::lookAt(glm::vec3(5.0f * glm::sin(glm::radians(cameraR)), 0.5f, 5.0f * glm::cos(glm::radians(cameraR))), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 model(1.0f);
	glm::mat4 groupMove(1.0f);

	glm::mat4 transform = projection * view * model;

	glUseProgram(shaderProgramID);

	GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	glBindVertexArray(VAO);

	if (isD == 1)
	{
		if (sizeD < 1)
			sizeD += 0.01f;
	}
	else if (isD == 2)
	{
		if (sizeD > 0)
			sizeD -= 0.01f;
	}

	if (isT)
		rotateT += 0.1f;

	if (isI)
	{
		if (leftx < tempx)
			leftx = glm::min(leftx + 0.01f, tempx); // 오른쪽으로 이동
		else if (leftx > tempx)
			leftx = glm::max(leftx - 0.01f, tempx); // 왼쪽으로 이동

		rightx = -leftx; // 현재처럼 좌우 대칭 배치일 때 반대로 이동

		if (leftx == tempx)
			isI = false;
	}

	if (isG)
	{
		rotateG += 0.1f * rotateDir;

		if (rotateG >= 90.0f)
		{
			rotateG = 90.0f; // 범위를 넘지 않도록 보정
			rotateDir = -1;
		}
		else if (rotateG <= -90.0f)
		{
			rotateG = -90.0f;
			rotateDir = 1;
		}
	}

	if (isP)
	{
		rotateP += 0.1f * rotateDir;
		if (rotateP >= 90.0f)
		{
			rotateP = 90.0f;
			rotateDir = -1;
		}
		else if (rotateP <= -90.0f)
		{
			rotateP = -90.0f;
			rotateDir = 1;
		}
	}

	glVertexAttrib3f(1, 0.678f, 0.847f, 0.902f);
	model = glm::scale(model, glm::vec3(100, 0.1, 100));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 바닥

	glVertexAttrib3f(1, lowerColor.r, lowerColor.g, lowerColor.b);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-0.5f, 0.21f + 0.1f * sizeD, 0.0f));
	//model = glm::rotate(model, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 2.0f + sizeD, 2.0f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	transform = projection * view * groupMove * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 왼쪽 아래 몸체

	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.5f, 0.21f + 0.1f * sizeD, 0.0f));
	//model = glm::rotate(model, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 2.0f + sizeD, 2.0f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	transform = projection * view * groupMove * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 오른쪽 아래 몸체

	glVertexAttrib3f(1, centerColor.r, centerColor.g, centerColor.b);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.51f + 0.2f * sizeD, 0.0f));
	model = glm::rotate(model, glm::radians(rotateT), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	transform = projection * view * groupMove * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 중앙 몸체

	glVertexAttrib3f(1, upperColor.r, upperColor.g, upperColor.b);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(leftx, 0.71f + 0.2f * sizeD, 0.0f));
	//model = glm::rotate(model, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	transform = projection * view * groupMove * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 왼쪽 상부 몸체

	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(rightx, 0.71f + 0.2f * sizeD, 0.0f));
	//model = glm::rotate(model, glm::radians(x), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.0f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	transform = projection * view * groupMove * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 오른쪽 상부 몸체

	glVertexAttrib3f(1, poleColor.r, poleColor.g, poleColor.b);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	glm::mat4 roP(1.0f);
	model = glm::translate(model, glm::vec3(-0.5f, 1.11f + 0.2f * sizeD, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 3.0f, 0.3f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	roP = glm::translate(roP, glm::vec3(-0.5f, (1.11f + 0.2f * sizeD) - 0.3f, 0.0f));
	roP = glm::rotate(roP, glm::radians(rotateP), glm::vec3(1, 0, 0));
	roP = glm::translate(roP, glm::vec3(0.5f, -((1.11f + 0.2f * sizeD) - 0.3f), 0.0f));
	transform = projection * view * groupMove * roP * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 왼쪽 깃대

	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	roP = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.5f, 1.11f + 0.2f * sizeD, 0.0f));
	model = glm::scale(model, glm::vec3(0.3f, 3.0f, 0.3f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	roP = glm::translate(roP, glm::vec3(0.5f, (1.11f + 0.2f * sizeD) - 0.3f, 0.0f));
	roP = glm::rotate(roP, glm::radians(-rotateP), glm::vec3(1, 0, 0));
	roP = glm::translate(roP, glm::vec3(-0.5f, -((1.11f + 0.2f * sizeD) - 0.3f), 0.0f));
	transform = projection * view * groupMove * roP * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 오른쪽 깃대

	glVertexAttrib3f(1, barrelColor.r, barrelColor.g, barrelColor.b);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	glm::mat4 roG(1.0f);
	model = glm::translate(model, glm::vec3(-0.5f, 0.71f + 0.2f * sizeD, 0.4f));
	model = glm::scale(model, glm::vec3(0.3f, 0.2f, 3.0f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	roG = glm::translate(roG, glm::vec3(-0.5f, 0.71f + 0.2f * sizeD, 0.1f));
	roG = glm::rotate(roG, glm::radians(rotateG), glm::vec3(0.0f, 1.0f, 0.0f));
	roG = glm::translate(roG, glm::vec3(0.5f, (0.71f + 0.2f * sizeD) * -1, -0.1f));
	transform = projection * view * groupMove * roG * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 왼쪽 포신

	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	roG = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.5f, 0.71f + 0.2f * sizeD, 0.4f));
	model = glm::scale(model, glm::vec3(0.3f, 0.2f, 3.0f));
	groupMove = glm::translate(groupMove, glm::vec3(moveX, 0, moveZ));
	roG = glm::translate(roG, glm::vec3(0.5f, 0.71f + 0.2f * sizeD, 0.1f));
	roG = glm::rotate(roG, glm::radians(-rotateG), glm::vec3(0.0f, 1.0f, 0.0f));
	roG = glm::translate(roG, glm::vec3(-0.5f, (0.71f + 0.2f * sizeD) * -1, -0.1f));
	transform = projection * view * groupMove * roG * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 오른쪽 포신

	if (isY == 1 ||isR==1)
	{
		cameraR += 0.1f;
	}
	else if (isY == 2||isR==2)
	{
		cameraR -= 0.1f;
	}
	else
		cameraR = 0;
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		// 위치, 회전, 카메라 및 애니메이션을 시작 상태로 복원
		moveX = moveZ = sizeD = rotateT = rotateG = rotateP = cameraR = tempx = 0.0f;
		leftx = -0.5f;
		rightx = 0.5f;
		camerax = cameray = cameraz = isD = isY = isR = 0;
		rotateDir = 1;
		isT = isI = isG = isP = false;

		lowerColor = glm::vec3(0.251f, 0.251f, 0.251f);
		centerColor = glm::vec3(0.827f, 0.827f, 0.827f);
		upperColor = glm::vec3(0.0f, 1.0f, 0.0f);
		poleColor = glm::vec3(0.769f, 0.604f, 0.0f);
		barrelColor = glm::vec3(1.0f, 0.980f, 0.804f);
		return;
	}

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_LEFT)
		{
			moveX -= 0.1f;
		}
		if (key == GLFW_KEY_RIGHT)
		{
			moveX += 0.1f;
		}
		if (key == GLFW_KEY_UP)
		{
			moveZ -= 0.1f;
		}
		if (key == GLFW_KEY_DOWN)
		{
			moveZ += 0.1f;
		}

		if (key == GLFW_KEY_W)
		{
			cameraz--;
		}
		if (key == GLFW_KEY_A)
		{
			camerax--;
		}
		if (key == GLFW_KEY_S)
		{
			cameraz++;
		}
		if (key == GLFW_KEY_D)
		{
			camerax++;
		}
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		lowerColor = randomColor();
		centerColor = randomColor();
		upperColor = randomColor();
		poleColor = randomColor();
		barrelColor = randomColor();
	}

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		if (isD < 2)
			isD++;
		else
			isD = 0;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		isT = !isT;
		rotateT = 0;
	}
	if (key == GLFW_KEY_I && action == GLFW_PRESS && !isI)
	{
		tempx = rightx;
		isI = true;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		isG = !isG;
		if (isG == false)
			rotateG = 0;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		isP = !isP;
		if (isP == false)
			rotateP = 0;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (isY < 2)
			isY++;
		else
			isY = 0;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (isR < 2)
			isR++;
		else
			isR = 0;
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

	// 정점마다 좌표 float 3개만 읽는다.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	// 색상은 VBO에서 읽지 않고, drawScene()의 glVertexAttrib3f() 값으로 사용한다.
	glDisableVertexAttribArray(1);

	glBindVertexArray(0);
}
