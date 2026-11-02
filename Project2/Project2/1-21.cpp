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
#include <time.h>
#include <vector>

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

struct Ball
{
	glm::vec2 position;
	glm::vec2 velocity;
};

Ball balls[5];
int ballCount = 0;
const float ballRadius = 0.35f;

GLuint ballVAO, ballVBO;
GLsizei ballVertexCount;

void InitBall();

const GLfloat vertices[] = {
	// 면 이름은 변환 전 좌표 기준: 앞 -Z, 뒤 +Z, 오른쪽 +X, 왼쪽 -X, 위 +Y, 아래 -Y
	// 정육면체 앞면 (z = -0.5)
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

	  // 정육면체 뒷면 (z = +0.5)
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

};

const GLsizei cubeVertexCount = 36;

void InitBuffer();

float Zangle,Yangle;
int isZ,isY;
bool floorOpen = false;
float floorAngle = 0.0f;
bool dragging = false;
double lastMouseX = 0.0;

int main(void)
{
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
	window = glfwCreateWindow(width, height,
		"Practice 21",
		NULL, NULL);
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
	InitBall();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
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
	glDeleteBuffers(1, &ballVBO);
	glDeleteVertexArrays(1, &ballVAO);
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
	static double lastTime = glfwGetTime();
	double now = glfwGetTime();
	float dt = glm::clamp(float(now - lastTime), 0.0f, 0.05f);
	lastTime = now;

	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	if (width == 0 || height == 0) return;
	glViewport(0, 0, width, height);

	// O 키로 바닥을 뒤쪽 모서리에서 아래로 90도 여닫는다.
	if (floorOpen)
		floorAngle = glm::min(floorAngle + 90.0f * dt, 90.0f);
	else
		floorAngle = glm::max(floorAngle - 90.0f * dt, 0.0f);
	const bool hasFloor = !floorOpen && floorAngle == 0.0f;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	glm::mat4 model(1.0f);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);

	glm::mat4 transform = projection * view * model;
	
	GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	if (isZ == 1)
		Zangle += 6.0f * dt;
	else if (isZ == 2)
		Zangle -= 6.0f * dt;

	if (isY == 1)
		Yangle += 6.0f * dt;
	else if (isY == 2)
		Yangle -= 6.0f * dt;

	// 방만 앞면을 컬링한다. 회전해도 카메라 쪽 외벽은 사라지고 안쪽이 보인다.
	glCullFace(GL_FRONT);
	glVertexAttrib3f(1, 0.25f, 0.25f, 0.25f);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(10, 10, 10));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glVertexAttrib3f(1, 0.30f, 0.30f, 0.30f);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(10, 10, 10));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 6, 6);

	// +Z 면도 그려야 Y축 회전 후 반대편에서 방 안쪽을 볼 수 있다.
	glVertexAttrib3f(1, 0.28f, 0.28f, 0.28f);
	glDrawArrays(GL_TRIANGLES, 12, 6);

	glVertexAttrib3f(1, 0.35f, 0.35f, 0.35f);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(10, 10, 10));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 18, 6);

	glVertexAttrib3f(1, 0.40f, 0.40f, 0.40f);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(10, 10, 10));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 24, 6);

	glVertexAttrib3f(1, 0.45f, 0.45f, 0.45f);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));
	// 바닥 뒤쪽 모서리 (0, -5, -5)를 경첩으로 사용한다.
	model = glm::translate(model, glm::vec3(0, -5, -5));
	model = glm::rotate(model, glm::radians(floorAngle), glm::vec3(1, 0, 0));
	model = glm::translate(model, glm::vec3(0, 5, 5));
	model = glm::scale(model, glm::vec3(10, 10, 10));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	// 열린 바닥은 양쪽에서 보이게 한다.
	if (floorAngle > 0.0f)
		glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 30, 6);
	glEnable(GL_CULL_FACE);
	// 내부 상자와 공은 바깥 면이 보여야 하므로 뒷면을 컬링한다.
	glCullFace(GL_BACK);

	static glm::vec3 position(0, -4.5f, 0);
	static glm::vec3 velocity(0);

	if ((isY == 1 || isY == 2) && hasFloor)
	{
		// Y축 회전 중에는 방 내부 위치 유지
		velocity = glm::vec3(0);
	}
	else
	{
		// Z축 기울기에 따라 미끄러짐
		velocity.x -= 9.8f * glm::sin(glm::radians(Zangle)) * dt;
		velocity.y -= 9.8f * glm::cos(glm::radians(Zangle)) * dt;
		position += velocity * dt;

		// 좌우 벽 충돌
		if (position.y >= -5.5f && (position.x < -4.5f || position.x > 4.5f))
		{
			position.x = glm::clamp(position.x, -4.5f, 4.5f);
			velocity.x = 0.0f;
		}

		// 바닥과 천장 충돌
		if (position.y > 4.5f || (hasFloor && position.y >= -5.5f && position.y < -4.5f))
		{
			position.y = glm::clamp(position.y, -4.5f, 4.5f);
			velocity.y = 0.0f;
		}
	}

	// 회색 방과 같은 순서로 회전
	model = glm::rotate(glm::mat4(1.0f), glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));

	// 회전한 방 안에 빨간 상자 배치
	model = glm::translate(model, position);

	glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);

	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 두 번째 상자는 별도의 상태를 유지하고 같은 dt를 사용한다.
	static glm::vec3 position2(0, -4.0f, -2.0f);
	static glm::vec3 velocity2(0);

	if ((isY == 1 || isY == 2) && hasFloor)
	{
		// Y축 회전 중에는 방 내부 위치 유지
		velocity2 = glm::vec3(0);
	}
	else
	{
		// Z축 기울기에 따라 미끄러짐
		velocity2.x -= 9.8f * glm::sin(glm::radians(Zangle)) * dt;
		velocity2.y -= 9.8f * glm::cos(glm::radians(Zangle)) * dt;
		position2 += velocity2 * dt;

		// 한 변이 2이므로 중심의 이동 범위는 ±4이다.
		if (position2.y >= -6.0f && (position2.x < -4.0f || position2.x > 4.0f))
		{
			position2.x = glm::clamp(position2.x, -4.0f, 4.0f);
			velocity2.x = 0.0f;
		}

		if (position2.y > 4.0f || (hasFloor && position2.y >= -6.0f && position2.y < -4.0f))
		{
			position2.y = glm::clamp(position2.y, -4.0f, 4.0f);
			velocity2.y = 0.0f;
		}
	}

	// 회색 방과 같은 순서로 회전
	model = glm::rotate(glm::mat4(1.0f), glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));

	// 이동 후 확대해 상자의 위치가 확대되지 않도록 한다.
	model = glm::translate(model, position2);
	model = glm::scale(model, glm::vec3(2, 2, 2));

	glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);

	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 세 번째 상자는 기존 상자보다 앞쪽(+Z)에 배치한다.
	static glm::vec3 position3(0, -4.75f, 2.0f);
	static glm::vec3 velocity3(0);

	if ((isY == 1 || isY == 2) && hasFloor)
	{
		velocity3 = glm::vec3(0);
	}
	else
	{
		velocity3.x -= 9.8f * glm::sin(glm::radians(Zangle)) * dt;
		velocity3.y -= 9.8f * glm::cos(glm::radians(Zangle)) * dt;
		position3 += velocity3 * dt;

		// 한 변이 0.5이므로 중심의 이동 범위는 ±4.75이다.
		if (position3.y >= -5.25f && (position3.x < -4.75f || position3.x > 4.75f))
		{
			position3.x = glm::clamp(position3.x, -4.75f, 4.75f);
			velocity3.x = 0.0f;
		}

		if (position3.y > 4.75f || (hasFloor && position3.y >= -5.25f && position3.y < -4.75f))
		{
			position3.y = glm::clamp(position3.y, -4.75f, 4.75f);
			velocity3.y = 0.0f;
		}
	}

	model = glm::rotate(glm::mat4(1.0f), glm::radians(Zangle), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(Yangle), glm::vec3(0, 1, 0));
	model = glm::translate(model, position3);
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

	glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 공은 방 내부의 X, Y 방향으로만 이동한다.
	const float limit = 5.0f - ballRadius;
	glm::mat4 roomRotation(1.0f);
	roomRotation = glm::rotate(roomRotation, glm::radians(Zangle), glm::vec3(0, 0, 1));
	roomRotation = glm::rotate(roomRotation, glm::radians(Yangle), glm::vec3(0, 1, 0));

	glBindVertexArray(ballVAO);
	glVertexAttrib3f(1, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < ballCount; ++i)
	{
		// 바닥이 열려 있는 동안은 공도 중력을 받아 방 아래로 떨어진다.
		if (!hasFloor)
		{
			balls[i].velocity.x -= 9.8f * glm::sin(glm::radians(Zangle)) * dt;
			balls[i].velocity.y -= 9.8f * glm::cos(glm::radians(Zangle)) * dt;
		}
		balls[i].position.x += balls[i].velocity.x * dt;
		balls[i].position.y += balls[i].velocity.y * dt;

		// 왼쪽 벽: 오른쪽으로 튕김
		if (balls[i].position.y >= -5.0f - ballRadius && balls[i].position.x < -limit)
		{
			balls[i].position.x = -limit;
			balls[i].velocity.x = glm::abs(balls[i].velocity.x);
		}

		// 오른쪽 벽: 왼쪽으로 튕김
		if (balls[i].position.y >= -5.0f - ballRadius && balls[i].position.x > limit)
		{
			balls[i].position.x = limit;
			balls[i].velocity.x = -glm::abs(balls[i].velocity.x);
		}

		// 바닥: 위쪽으로 튕김
		if (hasFloor && balls[i].position.y >= -5.0f - ballRadius && balls[i].position.y < -limit)
		{
			balls[i].position.y = -limit;
			balls[i].velocity.y = glm::abs(balls[i].velocity.y);
		}

		// 천장: 아래쪽으로 튕김
		if (balls[i].position.y > limit)
		{
			balls[i].position.y = limit;
			balls[i].velocity.y = -glm::abs(balls[i].velocity.y);
		}

		// Z 위치는 방 내부에서 3으로 고정한다.
		model = glm::translate(roomRotation, glm::vec3(balls[i].position, 3.0f));
		model = glm::scale(model, glm::vec3(ballRadius));
		transform = projection * view * model;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, ballVertexCount);
	}

	glBindVertexArray(0);
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
		floorOpen = !floorOpen;

	if (key == GLFW_KEY_B && action == GLFW_PRESS && ballCount < 5)
	{
		balls[ballCount].position = glm::vec2(-3.0f + ballCount * 1.5f, 0.0f);
		balls[ballCount].velocity = glm::vec2(
			ballCount % 2 == 0 ? 2.0f : -2.0f,
			2.5f + ballCount * 0.3f);
		ballCount++;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		isZ = (isZ + 1) % 3;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		isY = (isY + 1) % 3;
	}
}

//--- 마우스 버튼 입력 콜백 함수
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		dragging = true;
		double mouseY;
		glfwGetCursorPos(window, &lastMouseX, &mouseY);
		isZ = 0; // 드래그 중에는 자동 Z축 회전을 멈춘다.
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		dragging = false;
	}
}

//--- 마우스 위치 입력 콜백 함수
void GetCursorPose(GLFWwindow* window, double xpos, double ypos)
{
	if (dragging && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// 왼쪽 드래그는 반시계 방향, 오른쪽 드래그는 시계 방향이다.
		Zangle -= static_cast<float>(xpos - lastMouseX) * 0.25f;
		lastMouseX = xpos;
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

	// 큐브의 정점 데이터를 GPU로 전달한다.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 정점마다 좌표 float 3개만 읽는다.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	// 색상 속성은 필요할 때 설정한다.
	glDisableVertexAttribArray(1);

	glBindVertexArray(0);
}

// 반지름 1인 구의 정점은 한 번 만들고 모든 공이 공유한다.
void InitBall()
{
	std::vector<GLfloat> data;
	const float pi = 3.14159265f;
	const int rows = 12;
	const int columns = 24;

	auto point = [](float latitude, float longitude)
	{
		return glm::vec3(
			glm::cos(latitude) * glm::cos(longitude),
			glm::sin(latitude),
			glm::cos(latitude) * glm::sin(longitude));
	};

	for (int i = 0; i < rows; ++i)
	{
		float a = -pi / 2 + pi * i / rows;
		float b = -pi / 2 + pi * (i + 1) / rows;
		for (int j = 0; j < columns; ++j)
		{
			float c = 2 * pi * j / columns;
			float d = 2 * pi * (j + 1) / columns;
			glm::vec3 points[] = {
				point(a, c), point(b, d), point(a, d),
				point(a, c), point(b, c), point(b, d)
			};
			for (glm::vec3 p : points)
			{
				data.push_back(p.x);
				data.push_back(p.y);
				data.push_back(p.z);
			}
		}
	}

	ballVertexCount = static_cast<GLsizei>(data.size() / 3);
	glGenVertexArrays(1, &ballVAO);
	glGenBuffers(1, &ballVBO);
	glBindVertexArray(ballVAO);
	glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

