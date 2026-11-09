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
GLuint frontOutlineEBO;

// 정점 하나: x, y, z만 저장. 큐브 0~35번, 사각뿔 36~53번. 색상은 drawScene()에서 지정한다.
const GLfloat vertices[] = {
	// 면 번호: 0 뒤 -Z, 1 오른쪽 +X, 2 앞 +Z, 3 왼쪽 -X, 4 위 +Y, 5 아래 -Y
	// 정육면체 뒷면 (z = -0.5)
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,

	 0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	 0.5f, 0.5f, -0.5f,

	 // 정육면체 오른쪽 면 (x = +0.5)
	  0.5f, -0.5f, 0.5f,
	  0.5f, -0.5f, -0.5f,
	  0.5f, 0.5f, -0.5f,

	  0.5f, -0.5f, 0.5f,
	  0.5f, 0.5f, -0.5f,
	  0.5f, 0.5f, 0.5f,

	  // 정육면체 앞면 (z = +0.5)
	  -0.5f, -0.5f, 0.5f,
	   0.5f, -0.5f, 0.5f,
	   0.5f, 0.5f, 0.5f,

	  -0.5f, -0.5f, 0.5f,
	   0.5f, 0.5f, 0.5f,
	  -0.5f, 0.5f, 0.5f,

	  // 정육면체 왼쪽 면 (x = -0.5)
	  -0.5f, -0.5f, -0.5f,
	  -0.5f, -0.5f, 0.5f,
	  -0.5f, 0.5f, 0.5f,

	  -0.5f, -0.5f, -0.5f,
	  -0.5f, 0.5f, 0.5f,
	  -0.5f, 0.5f, -0.5f,

	  // 정육면체 윗면 (y = +0.5)
	  -0.5f, 0.5f, 0.5f,
	   0.5f, 0.5f, 0.5f,
	   0.5f, 0.5f, -0.5f,

	  -0.5f, 0.5f, 0.5f,
	   0.5f, 0.5f, -0.5f,
	  -0.5f, 0.5f, -0.5f,

	  // 정육면체 아랫면 (y = -0.5)
	  -0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f, 0.5f,

	  -0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f, 0.5f,
	  -0.5f, -0.5f, 0.5f,

	  // 사각뿔: 밑면 y = -0.5, 꼭대기 (0, 0.5, 0). 바깥쪽에서 반시계 방향.
	  // 뒷면 (-Z): 36~38번
	   0.5f, -0.5f, -0.5f,
	  -0.5f, -0.5f, -0.5f,
	   0.0f,  0.5f,  0.0f,

	  // 오른쪽 면 (+X): 39~41번
	   0.5f, -0.5f,  0.5f,
	   0.5f, -0.5f, -0.5f,
	   0.0f,  0.5f,  0.0f,

	  // 앞면 (+Z): 42~44번
	  -0.5f, -0.5f,  0.5f,
	   0.5f, -0.5f,  0.5f,
	   0.0f,  0.5f,  0.0f,

	  // 왼쪽 면 (-X): 45~47번
	  -0.5f, -0.5f, -0.5f,
	  -0.5f, -0.5f,  0.5f,
	   0.0f,  0.5f,  0.0f,

	  // 밑면 (-Y), 삼각형 2개: 48~53번
	  -0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f,  0.5f,

	  -0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f,  0.5f,
	  -0.5f, -0.5f,  0.5f,

};

const GLsizei cubeVertexCount = 36;
const GLint pyramidVertexStart = cubeVertexCount;
const GLsizei pyramidVertexCount = 18;
const GLint orbitVertexStart = cubeVertexCount + pyramidVertexCount;
const GLsizei orbitVertexCount = 180;

void InitBuffer();

bool isN,isM=true,isY;

int isR;

float yAngle,lightAngle,zPos;

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

	glEnable(GL_CULL_FACE); // 컬링 활성화
	glFrontFace(GL_CCW);   // 반시계 방향 정점을 앞면으로 판정
	glCullFace(GL_BACK);  // 뒷면 제거
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
	glDeleteBuffers(1, &frontOutlineEBO);
	glDeleteVertexArrays(1, &VAO);
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}

//--- 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	//--- 세이더 코드 읽어오기
	std::string vertexSource = filetobuf("vertex8.glsl");
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
	std::string fragmentSource = filetobuf("fragment8.glsl");
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	if (isY)
		yAngle += 0.1f;

	if (isR == 1)
		lightAngle += 0.1f;
	else if (isR == 2)
		lightAngle -= 0.1f;

	glm::mat4 model(1.0f);

	glm::mat4 view = glm::lookAt(glm::vec3(-5.0f, 4.0f, 10.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f); // 원근투영
	/*float aspect = float(width) / float(height);
	glm::mat4 projection = glm::ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, 0.1f, 100.0f);*/ //직각투영

	glm::mat4 transform = projection * view * model;

	GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");

	// 조명
	glm::mat4 light(1.0f);
	light = glm::rotate(light, glm::radians(lightAngle), glm::vec3(0, 1, 0));
	glm::vec4 lightPos = light * glm::vec4(0, 0, 4+zPos, 1);
	location = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(location, lightPos.x,lightPos.y,lightPos.z);
	location = glGetUniformLocation(shaderProgramID, "lightColor");
	if (isM)
		glUniform3f(location, 1.0f, 1.0f, 1.0f);
	else
		glUniform3f(location, 0, 0, 0);
	location = glGetUniformLocation(shaderProgramID, "useLighting");
	glUniform1i(location, GL_TRUE);

	glVertexAttrib3f(1, 0.0f, 1.0f, 0.0f);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(yAngle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(2, 2, 2));
	transform = projection * view * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	location = glGetUniformLocation(shaderProgramID, "model");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
	if (isN == false)
		glDrawArrays(GL_TRIANGLES, 0, 36);
	else
		glDrawArrays(GL_TRIANGLES, 36, 18);

	glVertexAttrib3f(1, 0.75f, 0.75f, 0.75f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(lightPos));
	transform = projection * view * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	location = glGetUniformLocation(shaderProgramID, "model");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 조명 공전 궤도: 원점 중심, XZ 평면, 반지름은 광원과 동일하게 설정한다.
	if (4 + zPos != 0)
	{
		location = glGetUniformLocation(shaderProgramID, "useLighting");
		glUniform1i(location, GL_FALSE);
		glVertexAttrib3f(1, 0.0f, 0.5f, 0.0f);
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(4 + zPos, 1, 4 + zPos));
		transform = projection * view * model;
		location = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		location = glGetUniformLocation(shaderProgramID, "model");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_LINE_LOOP, orbitVertexStart, orbitVertexCount);
	}

	glBindVertexArray(0);

}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		isN = !isN;
	}
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		isM = !isM;
	}
	else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		isY = !isY;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (isR < 2)
			isR++;
		else
			isR = 0;
	}
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		if (mods & GLFW_MOD_SHIFT)
		{
			zPos += 0.5f;
		}
		else
		{
			zPos -= 0.5f;
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

	// 기존 좌표 배열에 삼각형별 법선을 붙인다. 정점 번호는 그대로 유지된다.
	constexpr int vertexCount = static_cast<int>(sizeof(vertices) / (3 * sizeof(GLfloat)));
	GLfloat lightingVertices[(vertexCount + orbitVertexCount) * 6];
	for (int i = 0; i < vertexCount; i += 3)
	{
		glm::vec3 a = glm::make_vec3(&vertices[(i + 0) * 3]);
		glm::vec3 b = glm::make_vec3(&vertices[(i + 1) * 3]);
		glm::vec3 c = glm::make_vec3(&vertices[(i + 2) * 3]);
		glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));

		// 같은 삼각형의 세 정점은 같은 면 법선을 사용한다.
		for (int j = 0; j < 3; ++j)
		{
			int index = i + j;
			for (int k = 0; k < 3; ++k)
			{
				lightingVertices[index * 6 + k] = vertices[index * 3 + k];
				lightingVertices[index * 6 + 3 + k] = normal[k];
			}
		}
	}
	// 기존 0~53번 정점 뒤에 반지름 1인 원의 정점 180개를 추가한다.
	for (int i = 0; i < orbitVertexCount; ++i)
	{
		float angle = glm::radians(float(i) * 360.0f / orbitVertexCount);
		int index = orbitVertexStart + i;
		lightingVertices[index * 6 + 0] = glm::sin(angle);
		lightingVertices[index * 6 + 1] = 0.0f;
		lightingVertices[index * 6 + 2] = glm::cos(angle);
		lightingVertices[index * 6 + 3] = 0.0f;
		lightingVertices[index * 6 + 4] = 1.0f;
		lightingVertices[index * 6 + 5] = 0.0f;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightingVertices), lightingVertices, GL_STATIC_DRAW);

	// 정점마다 float 6개: 앞의 3개는 위치, 뒤의 3개는 법선.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	// 색상은 VBO에서 읽지 않고, drawScene()의 glVertexAttrib3f() 값으로 사용한다.
	glDisableVertexAttribArray(1);

	// vertex8.glsl의 location 2로 법선을 전달한다.
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
		reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	const GLuint frontOutline[] = { 12, 13, 14, 17 };
	glGenBuffers(1, &frontOutlineEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontOutlineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frontOutline), frontOutline, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

