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

// 정점 하나: x, y, z만 저장 (큐브 36개). 색상은 drawScene()에서 지정한다.
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
		  -0.5f, -0.5f,  0.5f

};

const GLsizei cubeVertexCount = 36;
const GLint pyramidVertexStart = cubeVertexCount;
const GLsizei pyramidVertexCount = 18;
const GLint orbitVertexStart = cubeVertexCount + pyramidVertexCount;
const GLsizei orbitVertexCount = 180;
const GLint sphereVertexStart = orbitVertexStart + orbitVertexCount;
const int sphereStacks = 16;
const int sphereSlices = 24;
const GLsizei sphereVertexCount = sphereStacks * sphereSlices * 6;

void InitBuffer();

struct Planet {
	float radius;   // 공전 반지름
	float size;     // 행성 지름
	float speed;    // 초당 회전 각도
	float angle;
	float tilt;     // 궤도를 기울이는 각도
	float heading;  // 기울어진 궤도의 방향
	glm::vec3 color;
};

Planet planets[] = {
	{ 2.2f, 0.52f, 80.0f,  30.0f, 55.0f, -25.0f, glm::vec3(0.95f, 0.12f, 0.10f) }, // 수성
	{ 3.2f, 0.72f, 55.0f, 150.0f, 42.0f,  35.0f, glm::vec3(0.15f, 0.30f, 1.00f) }, // 금성
	{ 4.3f, 0.92f, 35.0f, 270.0f, 29.0f,   0.0f, glm::vec3(0.14f, 0.84f, 0.21f) }  // 지구
};

bool lightRotating = false;
float lightAngle = 0.0f;
float lightDistance = 1.0f;
float lightIntensity = 1.0f;
bool snowOn = false;
double snowTime = 0.0;

enum class SnowState { Falling, Waiting };
constexpr float snowTopY = 10.0f;

typedef struct Snow {
	float speed;
	float x, y, z;
	SnowState state;
	double landedAt;
}Snow;

Snow snow[100];
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

	for (int i = 0; i < 100; i++)
	{
		snow[i].state = SnowState::Falling;
		snow[i].speed = static_cast<float>(rand() % 20 + 1);
		snow[i].x = static_cast<float>(rand() % 9 - 4);
		snow[i].y = snowTopY;
		snow[i].z = static_cast<float>(rand() % 9 - 4);
	}

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
	const double now = glfwGetTime();
	static double previousTime = now;
	const float elapsed = static_cast<float>(now - previousTime);
	previousTime = now;

	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	if (width == 0 || height == 0) return;
	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	glm::mat4 model(1.0f);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f); // 원근투영
	/*float aspect = float(width) / float(height);
	glm::mat4 projection = glm::ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, 0.1f, 100.0f);*/ //직각투영

	glm::mat4 transform = projection * view * model;

	const GLint transformLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	const GLint modelLocation = glGetUniformLocation(shaderProgramID, "model");
	const GLint lightingLocation = glGetUniformLocation(shaderProgramID, "useLighting");

	// 조명
	if (lightRotating)
		lightAngle = glm::mod(lightAngle + 60.0f * elapsed, 360.0f);

	glm::mat4 light(1.0f);
	light = glm::rotate(light, glm::radians(lightAngle), glm::vec3(0, 1, 0));
	glm::vec4 lightPos = light * glm::vec4(6.0f * lightDistance, 6.0f * lightDistance, 0.0f, 1.0f);
	GLint location = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(location, lightPos.x, lightPos.y, lightPos.z);
	location = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(location, lightIntensity, lightIntensity, lightIntensity);
	glUniform1i(lightingLocation, GL_TRUE);

	glVertexAttrib3f(1, 0.34f, 0.34f, 0.34f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, -0.1, 0));
	model = glm::scale(model, glm::vec3(10, 0.1, 10));
	transform = projection * view * model;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);

	glVertexAttrib3f(1, 0.0f, 0.9f, 0.95f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(2, 2, 2));
	transform = projection * view * model;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, pyramidVertexStart, pyramidVertexCount);

	for (Planet& planet : planets)
	{
		// 궤도선과 행성에 같은 기울기를 적용한다.
		glm::mat4 orbit(1.0f);
		orbit = glm::translate(orbit, glm::vec3(0, 2.6f, 0));
		orbit = glm::rotate(orbit, glm::radians(planet.heading), glm::vec3(0, 1, 0));
		orbit = glm::rotate(orbit, glm::radians(planet.tilt), glm::vec3(1, 0, 0));

		// 궤도선
		glUniform1i(lightingLocation, GL_FALSE);
		glVertexAttrib3f(1, planet.color.r * 0.45f, planet.color.g * 0.45f, planet.color.b * 0.45f);
		model = glm::scale(orbit, glm::vec3(planet.radius, 1.0f, planet.radius));
		transform = projection * view * model;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_LINE_LOOP, orbitVertexStart, orbitVertexCount);

		// 행성: 중심을 기준으로 회전한 뒤 공전 반지름만큼 이동
		glUniform1i(lightingLocation, GL_TRUE);
		glVertexAttrib3f(1, planet.color.r, planet.color.g, planet.color.b);
		planet.angle = glm::mod(planet.angle + planet.speed * elapsed, 360.0f);
		model = glm::rotate(orbit, glm::radians(planet.angle), glm::vec3(0, 1, 0));
		model = glm::translate(model, glm::vec3(0, 0, planet.radius));
		model = glm::scale(model, glm::vec3(planet.size));
		transform = projection * view * model;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, sphereVertexStart, sphereVertexCount);
	}

	// 조명 위치에 밝은 육면체를 그린다.
	glUniform1i(lightingLocation, GL_FALSE);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(lightPos));
	model = glm::rotate(model, glm::radians(25.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(25.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.65f));
	transform = projection * view * model;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	const float lightFaceColors[] = { 0.7f, 0.65f, 1.0f, 0.78f, 0.85f, 0.6f };
	for (int face = 0; face < 6; ++face)
	{
		float shade = lightFaceColors[face];
		glVertexAttrib3f(1, shade, shade, shade);
		glDrawArrays(GL_TRIANGLES, face * 6, 6);
	}

	if (snowOn)
		snowTime += elapsed;

	glUniform1i(lightingLocation, GL_TRUE);
	glVertexAttrib3f(1, 1.0f, 1.0f, 1.0f);
	for (int i = 0; snowTime > 0.0 && i < 100; i++)
	{
		if (snowOn && snow[i].state == SnowState::Falling)
		{
			snow[i].y -= snow[i].speed * 0.6f * elapsed;
			if (snow[i].y <= 0.0f)
			{
				snow[i].y = 0.0f;
				snow[i].landedAt = snowTime;
				snow[i].state = SnowState::Waiting;
			}
		}
		else if (snowOn && snowTime - snow[i].landedAt >= 0.5)
		{
			snow[i].y = snowTopY;
			snow[i].state = SnowState::Falling;
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(snow[i].x, snow[i].y, snow[i].z));
		model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
		transform = projection * view * model;
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	}

	glBindVertexArray(0);

}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
	case GLFW_KEY_S:
		snowOn = !snowOn;
		break;
	case GLFW_KEY_R:
		lightRotating = !lightRotating;
		break;
	case GLFW_KEY_N:
		lightDistance = glm::clamp(lightDistance - 0.1f, 0.4f, 1.15f);
		break;
	case GLFW_KEY_F:
		lightDistance = glm::clamp(lightDistance + 0.1f, 0.4f, 1.15f);
		break;
	case GLFW_KEY_EQUAL: // 일반 키보드의 +/= 키
	case GLFW_KEY_KP_ADD:
		lightIntensity = glm::clamp(lightIntensity + 0.15f, 0.0f, 2.5f);
		break;
	case GLFW_KEY_MINUS:
	case GLFW_KEY_KP_SUBTRACT:
		lightIntensity = glm::clamp(lightIntensity - 0.15f, 0.0f, 2.5f);
		break;
	case GLFW_KEY_Q:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
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
	GLfloat lightingVertices[(vertexCount + orbitVertexCount + sphereVertexCount) * 6];
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

	// 위도와 경도로 구 표면을 나누고 각 칸을 삼각형 2개로 만든다.
	int sphereIndex = sphereVertexStart;
	for (int stack = 0; stack < sphereStacks; ++stack)
	{
		for (int slice = 0; slice < sphereSlices; ++slice)
		{
			const int stackOffsets[] = { 0, 0, 1, 0, 1, 1 };
			const int sliceOffsets[] = { 0, 1, 0, 1, 1, 0 };
			for (int j = 0; j < 6; ++j)
			{
				float phi = glm::radians(180.0f * (stack + stackOffsets[j]) / sphereStacks);
				float theta = glm::radians(360.0f * (slice + sliceOffsets[j]) / sphereSlices);
				glm::vec3 normal(glm::sin(phi) * glm::cos(theta), glm::cos(phi),
					glm::sin(phi) * glm::sin(theta));
				for (int k = 0; k < 3; ++k)
				{
					lightingVertices[sphereIndex * 6 + k] = 0.5f * normal[k];
					lightingVertices[sphereIndex * 6 + 3 + k] = normal[k];
				}
				++sphereIndex;
			}
		}
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

