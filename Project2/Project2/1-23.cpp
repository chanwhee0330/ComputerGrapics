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

};

const GLsizei cubeVertexCount = 36;

void InitBuffer();

float angle,px,py,pz,cx=0.0f,cz=8.0f;
float moveAngle = 0.0f;
int swingDir = 1;
float walkSpeed = 1.0f; // 기본 걷기 속도에 대한 배율 (0.5~1.5)

int dir = 0,temp;
int orbitDir = 1;
bool isY = false;
bool frontOpen = false;
float frontOpenAmount = 0.0f;

const float stageHalfWidth = 2.5f;
const float stageHalfDepth = 1.5f;
const float robotRadius = 0.4f;

struct Obstacle
{
	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 color;
};

Obstacle obstacles[3];
bool isJumping = false;
float jumpTime = 0.0f;
float fallVelocity = 0.0f;
glm::vec3 jumpStart(0.0f), jumpEnd(0.0f);
const float jumpDuration = 0.7f;

float randomRange(float low, float high)
{
	return low + (high - low) * rand() / static_cast<float>(RAND_MAX);
}

void InitObstacles()
{
	const glm::vec3 sizes[] = {
		{0.55f, 0.25f, 0.45f}, {0.70f, 0.40f, 0.55f}, {0.50f, 0.30f, 0.70f}
	};
	const glm::vec3 colors[] = {
		{0.75f, 0.47f, 0.22f}, {0.46f, 0.48f, 0.52f}, {0.55f, 0.38f, 0.68f}
	};
	// 서로 다른 구역 안에서 무작위로 배치해 장애물끼리 겹치지 않게 한다.
	const int thirdSide = rand() % 2 == 0 ? -1 : 1;
	const int xSigns[] = {-1, 1, thirdSide};
	const int zSigns[] = {1, -1, thirdSide};
	for (int i = 0; i < 3; ++i)
	{
		obstacles[i].size = sizes[i];
		obstacles[i].color = colors[i];
		obstacles[i].position = glm::vec3(
			xSigns[i] * randomRange(1.05f, 1.65f), sizes[i].y * 0.5f,
			zSigns[i] * randomRange(0.45f, 0.85f));
	}
}

bool overlapsObstacle(float x, float z, const Obstacle& obstacle, float padding)
{
	return glm::abs(x - obstacle.position.x) < obstacle.size.x * 0.5f + padding &&
		glm::abs(z - obstacle.position.z) < obstacle.size.z * 0.5f + padding;
}

bool positionBlocked(float x, float z, float feetY)
{
	if (glm::abs(x) > stageHalfWidth - robotRadius ||
		glm::abs(z) > stageHalfDepth - robotRadius)
		return true;
	for (const Obstacle& obstacle : obstacles)
	{
		if (feetY < obstacle.size.y - 0.001f &&
			overlapsObstacle(x, z, obstacle, robotRadius))
			return true;
	}
	return false;
}

int supportObstacle(float x, float z, float feetY)
{
	int result = -1;
	float highest = 0.0f;
	for (int i = 0; i < 3; ++i)
	{
		if (obstacles[i].size.y <= feetY + 0.001f &&
			obstacles[i].size.y > highest &&
			overlapsObstacle(x, z, obstacles[i], robotRadius))
		{
			highest = obstacles[i].size.y;
			result = i;
		}
	}
	return result;
}

glm::vec2 walkingDirection(int direction)
{
	const glm::vec2 directions[] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
	return directions[direction];
}

void ResetScene()
{
	angle = px = py = pz = cx = moveAngle = 0.0f;
	cz = 8.0f;
	walkSpeed = 1.0f;
	swingDir = orbitDir = 1;
	dir = temp = 0;
	isY = frontOpen = isJumping = false;
	frontOpenAmount = jumpTime = fallVelocity = 0.0f;
	jumpStart = jumpEnd = glm::vec3(0.0f);
}

void StartJump()
{
	if (isJumping) return;
	const int platform = supportObstacle(px, pz, py);
	const float ground = platform < 0 ? 0.0f : obstacles[platform].size.y;
	if (glm::abs(py - ground) > 0.002f) return; // 떨어지는 도중의 중복 점프 방지

	jumpStart = jumpEnd = glm::vec3(px, py, pz);
	if (platform >= 0)
	{
		// 장애물 위에서는 진행 방향의 빈 바닥으로 뛰어내린다.
		const int candidates[] = {dir, (dir + 2) % 4, (dir + 1) % 4, (dir + 3) % 4};
		for (int candidate : candidates)
		{
			const glm::vec2 direction = walkingDirection(candidate);
			const float halfLength = candidate % 2 == 0 ?
				obstacles[platform].size.z * 0.5f : obstacles[platform].size.x * 0.5f;
			const glm::vec2 target = glm::vec2(obstacles[platform].position.x,
				obstacles[platform].position.z) + direction * (halfLength + robotRadius + 0.15f);
			if (!positionBlocked(target.x, target.y, 0.0f))
			{
				jumpEnd = glm::vec3(target.x, 0.0f, target.y);
				break;
			}
		}
	}
	else
	{
		// 평지에서는 제자리 점프, 장애물 가까이에서는 그 위로 뛰어오른다.
		float nearest = robotRadius + 0.12f;
		for (const Obstacle& obstacle : obstacles)
		{
			const float dx = glm::max(glm::abs(px - obstacle.position.x) - obstacle.size.x * 0.5f, 0.0f);
			const float dz = glm::max(glm::abs(pz - obstacle.position.z) - obstacle.size.z * 0.5f, 0.0f);
			const float distance = glm::length(glm::vec2(dx, dz));
			if (distance < nearest && !positionBlocked(obstacle.position.x, obstacle.position.z, obstacle.size.y))
			{
				nearest = distance;
				jumpEnd = glm::vec3(obstacle.position.x, obstacle.size.y, obstacle.position.z);
			}
		}
	}
	isJumping = true;
	jumpTime = fallVelocity = 0.0f;
	temp = dir;
	dir = -1;
}

void UpdateJump()
{
	jumpTime = glm::min(jumpTime + 1.0f / 60.0f, jumpDuration);
	const float t = jumpTime / jumpDuration;
	// 발이 장애물보다 높이 올라간 구간에서만 수평으로 이동한다.
	const float travel = glm::smoothstep(0.25f, 0.75f, t);
	px = glm::mix(jumpStart.x, jumpEnd.x, travel);
	pz = glm::mix(jumpStart.z, jumpEnd.z, travel);
	py = glm::mix(jumpStart.y, jumpEnd.y, t) + 4.0f * 0.65f * t * (1.0f - t);
	dir = t < 0.5f ? -1 : -2;
	if (jumpTime >= jumpDuration)
	{
		px = jumpEnd.x;
		py = jumpEnd.y;
		pz = jumpEnd.z;
		isJumping = false;
		dir = temp;
	}
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
	InitObstacles();
	ResetScene();

	glEnable(GL_CULL_FACE); // 컬링 활성화
	glFrontFace(GL_CCW);   // 반시계 방향 정점을 앞면으로 판정
	glCullFace(GL_FRONT);  // 앞면 제거
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

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	if (isY)
		angle += 0.005f * orbitDir;
	frontOpenAmount = glm::clamp(frontOpenAmount + (frontOpen ? 0.025f : -0.025f), 0.0f, 1.0f);

	// 빠르게 걸을수록 팔다리를 더 크게, 더 빠르게 흔든다.
	const float maxMoveAngle = 30.0f * walkSpeed;
	moveAngle = glm::clamp(moveAngle, -maxMoveAngle, maxMoveAngle);
	moveAngle += 2.0f * walkSpeed * walkSpeed * swingDir;
	if (moveAngle >= maxMoveAngle)
	{
		moveAngle = maxMoveAngle;
		swingDir = -1;
	}
	else if (moveAngle <= -maxMoveAngle)
	{
		moveAngle = -maxMoveAngle;
		swingDir = 1;
	}

	glm::mat4 model(1.0f);
	glm::mat4 groupmove(1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(cz*glm::sin(angle)+cx*glm::cos(angle), 1.5f, cz*glm::cos(angle)-cx*glm::sin(angle)), glm::vec3(cx*glm::cos(angle), 1.5f, -cx*glm::sin(angle)), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 transform = projection * view * model;
	GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	glCullFace(GL_FRONT);
	for (int face = 0; face < 6; ++face)
	{
		if (face == 2 && frontOpenAmount >= 1.0f) continue;
		model = glm::mat4(1.0f);
		// 앞벽은 상단을 고정한 채 높이를 줄여 위쪽으로 열린다.
		const float opening = face == 2 ? frontOpenAmount : 0.0f;
		model = glm::translate(model, glm::vec3(0, 1.5f + 1.5f * opening, 0));
		model = glm::scale(model, glm::vec3(5, 3.0f * (1.0f - opening), 3));
		if (face == 0)
			glVertexAttrib3f(1, 0.80f, 0.50f, 0.50f);
		else if (face == 1)
			glVertexAttrib3f(1, 0.50f, 0.80f, 0.50f);
		else if (face == 2)
			glVertexAttrib3f(1, 0.50f, 0.50f, 0.80f);
		else if (face == 3)
			glVertexAttrib3f(1, 0.80f, 0.50f, 0.80f);
		else if (face == 4)
			glVertexAttrib3f(1, 0.80f, 0.80f, 0.50f);
		else if (face == 5)
			glVertexAttrib3f(1, 0.50f, 0.80f, 0.80f);
		glm::mat4 transform = projection * view * model;
		GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, face * 6, 6);
		if (face == 2)
		{
			// 앞면이 컬링되는 시점에서도 윤곽선으로 개폐 위치를 확인한다.
			glVertexAttrib3f(1, 0.30f, 0.30f, 0.55f);
			glDepthFunc(GL_LEQUAL);
			glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, nullptr);
			glDepthFunc(GL_LESS);
		}
	}

	if (dir == 0)
	{
		if (pz > -1.5f && !positionBlocked(px, pz - 0.01f * walkSpeed, py))
			pz = glm::max(pz - 0.01f * walkSpeed, -1.5f);
		else
			dir = 2;
	}
	else if (dir == 1)
	{
		if (px > -2.5f && !positionBlocked(px - 0.01f * walkSpeed, pz, py))
			px = glm::max(px - 0.01f * walkSpeed, -2.5f);
		else
			dir = 3;
	}
	else if (dir == 2)
	{
		if (pz < 1.5f && !positionBlocked(px, pz + 0.01f * walkSpeed, py))
			pz = glm::min(pz + 0.01f * walkSpeed, 1.5f);
		else
			dir = 0;
	}
	else if (dir == 3)
	{
		if (px < 2.5f && !positionBlocked(px + 0.01f * walkSpeed, pz, py))
			px = glm::min(px + 0.01f * walkSpeed, 2.5f);
		else
			dir = 1;
	}
	else if (dir == -1)
	{
		UpdateJump();
	}
	else if (dir == -2)
	{
		UpdateJump();
	}

	// 장애물 가장자리에서 걸어 나가면 바닥까지 내려온다.
	if (!isJumping)
	{
		const int platform = supportObstacle(px, pz, py);
		const float ground = platform < 0 ? 0.0f : obstacles[platform].size.y;
		if (py > ground + 0.001f)
		{
			fallVelocity -= 9.8f / 60.0f;
			py = glm::max(ground, py + fallVelocity / 60.0f);
			if (py <= ground) fallVelocity = 0.0f;
		}
		else
		{
			py = ground;
			fallVelocity = 0.0f;
		}
	}

	glCullFace(GL_BACK);
	for (const Obstacle& obstacle : obstacles)
	{
		model = glm::translate(glm::mat4(1.0f), obstacle.position);
		model = glm::scale(model, obstacle.size);
		transform = projection * view * model;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		glVertexAttrib3f(1, obstacle.color.r, obstacle.color.g, obstacle.color.b);
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	}

	// 점프 중에는 점프 직전의 방향을 계속 바라본다.
	const int facingDir = (dir < 0) ? temp : dir;
	float turnAngle = 0.0f;
	if (facingDir == 0) turnAngle = 180.0f;
	else if (facingDir == 1)turnAngle = -90.0f;
	else if (facingDir == 2)turnAngle = 0.0f;
	else if (facingDir == 3)turnAngle = 90.0f;

	groupmove = glm::translate(groupmove, glm::vec3(px, py, pz));
	groupmove = glm::rotate(groupmove, glm::radians(turnAngle), glm::vec3(0, 1, 0));
	glCullFace(GL_BACK);
	glVertexAttrib3f(1, 0, 1, 1); // 왼쪽 발
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-0.05f,0.25f,0));
	model = glm::translate(model, glm::vec3(0, 0.25f, 0));
	model = glm::rotate(model, glm::radians(moveAngle), glm::vec3(1, 0, 0));
	model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.1f));
	transform = projection * view *groupmove * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glVertexAttrib3f(1, 1,0, 1); // 오른쪽 발
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.05f, 0.25f, 0));
	model = glm::translate(model, glm::vec3(0, 0.25f, 0));
	model = glm::rotate(model, glm::radians(-moveAngle), glm::vec3(1, 0, 0));
	model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.1f));
	transform = projection * view * groupmove * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glVertexAttrib3f(1, 0, 0, 1); // 몸체
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, 0.75f, 0));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.2f));
	transform = projection * view * groupmove * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glVertexAttrib3f(1, 0, 1, 1); // 왼쪽팔
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-0.3f, 0.75f, 0));
	model = glm::translate(model, glm::vec3(-0.3f, 0.20f, 0));
	model = glm::rotate(model, glm::radians(-moveAngle), glm::vec3(1, 0, 0));
	model = glm::translate(model, glm::vec3(0.3f, -0.20f, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.4f, 0.1f));
	transform = projection * view * groupmove * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glVertexAttrib3f(1, 1, 0, 1); // 오른쪽팔
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.3f, 0.75f, 0));
	model = glm::translate(model, glm::vec3(0.3f, 0.20f, 0));
	model = glm::rotate(model, glm::radians(moveAngle), glm::vec3(1, 0, 0));
	model = glm::translate(model, glm::vec3(-0.3f, -0.20f, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.4f, 0.1f));
	transform = projection * view * groupmove * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glVertexAttrib3f(1, 1, 0.8f, 0.7f); // 얼굴
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, 1.1f, 0));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	transform = projection * view * groupmove * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glVertexAttrib3f(1, 0, 0, 0); // 코
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, 1.1f, 0.1025f));
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	transform = projection * view * groupmove * model;
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
	const bool shifted = (mods & GLFW_MOD_SHIFT) != 0;
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		ResetScene();
		return;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		frontOpen = !frontOpen;
		return;
	}
	if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
	{
		walkSpeed = glm::min(walkSpeed + 0.25f, 1.5f);
		return;
	}
	if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
	{
		walkSpeed = glm::max(walkSpeed - 0.25f, 0.5f);
		return;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		const int requested = shifted ? -1 : 1;
		if (isY && orbitDir == requested)
			isY = false;
		else
		{
			isY = true;
			orbitDir = requested;
		}
		return;
	}
	if (key == GLFW_KEY_Z || key == GLFW_KEY_UP || key == GLFW_KEY_DOWN)
	{
		const float change = key == GLFW_KEY_DOWN || (key == GLFW_KEY_Z && shifted) ? 0.1f : -0.1f;
		cz = glm::clamp(cz + change, 3.0f, 20.0f);
		return;
	}
	if (key == GLFW_KEY_X || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
	{
		const float change = key == GLFW_KEY_RIGHT || (key == GLFW_KEY_X && shifted) ? 0.1f : -0.1f;
		cx += change;
		return;
	}

	// 점프와 낙하 중에는 방향을 유지하되 카메라와 속도 키는 계속 사용할 수 있다.
	if (dir < 0 || fallVelocity < 0.0f) return;
	if (key == GLFW_KEY_W && action == GLFW_PRESS) dir = 0;
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) dir = 1;
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) dir = 2;
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) dir = 3;
	else if (key == GLFW_KEY_J && action == GLFW_PRESS) StartJump();
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

	const GLuint frontOutline[] = {12, 13, 14, 17};
	glGenBuffers(1, &frontOutlineEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontOutlineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frontOutline), frontOutline, GL_STATIC_DRAW);

	glBindVertexArray(0);
}
