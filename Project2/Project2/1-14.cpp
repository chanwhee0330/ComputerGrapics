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

bool ish = true;

const GLfloat vertices[] = {
	// 앞면
	-0.5f,-0.5f, 0.5f,  1,0,0,
	 0.5f,-0.5f, 0.5f,  0,1,0,
	 0.5f, 0.5f, 0.5f,  0,0,1,

	-0.5f,-0.5f, 0.5f,  1,0,0,
	 0.5f, 0.5f, 0.5f,  0,0,1,
	-0.5f, 0.5f, 0.5f,  1,1,0,

	// 오른쪽 면
	 0.5f,-0.5f, 0.5f,  0,1,0,
	 0.5f,-0.5f,-0.5f,  0,1,1,
	 0.5f, 0.5f,-0.5f,  1,1,1,

	 0.5f,-0.5f, 0.5f,  0,1,0,
	 0.5f, 0.5f,-0.5f,  1,1,1,
	 0.5f, 0.5f, 0.5f,  0,0,1,

	 // 뒷면
	  0.5f,-0.5f,-0.5f,  0,1,1,
	 -0.5f,-0.5f,-0.5f,  1,0,1,
	 -0.5f, 0.5f,-0.5f,  1,0.5f,0,

	  0.5f,-0.5f,-0.5f,  0,1,1,
	 -0.5f, 0.5f,-0.5f,  1,0.5f,0,
	  0.5f, 0.5f,-0.5f,  1,1,1,

	  // 왼쪽 면
	  -0.5f,-0.5f,-0.5f,  1,0,1,
	  -0.5f,-0.5f, 0.5f,  1,0,0,
	  -0.5f, 0.5f, 0.5f,  1,1,0,

	  -0.5f,-0.5f,-0.5f,  1,0,1,
	  -0.5f, 0.5f, 0.5f,  1,1,0,
	  -0.5f, 0.5f,-0.5f,  1,0.5f,0,

	  // 윗면
	  -0.5f, 0.5f, 0.5f,  1,1,0,
	   0.5f, 0.5f, 0.5f,  0,0,1,
	   0.5f, 0.5f,-0.5f,  1,1,1,

	  -0.5f, 0.5f, 0.5f,  1,1,0,
	   0.5f, 0.5f,-0.5f,  1,1,1,
	  -0.5f, 0.5f,-0.5f,  1,0.5f,0,

	  // 아랫면
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
	   0.0f,0.0f, 1.5f,  0,0,1
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
	cube = glm::rotate(cube, glm::radians(30.0f), glm::vec3(1, 0, 0));
	cube = glm::rotate(cube, glm::radians(30.0f), glm::vec3(0, 1, 0));

	// model: 사각형 한 면의 변환
	// transform: 면 배치 -> 정육면체 전체 회전 (오른쪽부터 적용)
	glm::mat4 model(1.0f);
	glm::mat4 transform(1.0f);
	
	transform = cube * model;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

	glDrawArrays(GL_TRIANGLES, 0, 36);
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
