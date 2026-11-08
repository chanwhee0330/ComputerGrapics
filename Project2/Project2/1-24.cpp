#include <GL/glew.h>
#include <GL/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cmath>
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

typedef struct Rail {
	float x, y, z, radian;
	// isTurn: 코너에서 회전 중, isOver: 코너를 지나 세로 구간에 있음.
	bool isTurn, isOver;
}Rail;

// 코너 중심은 월드 원점. 두 직선은 코너를 제외하고 각각 길이 10.
const float beltLength = 10.0f;
const float beltWidth = 2.0f;
const float beltHeight = 0.5f;
const float beltY = 0.0f;
const float beltTop = beltY + beltHeight / 2.0f;
const float beltBottom = beltY - beltHeight / 2.0f;
const float armCenter = (beltLength + beltWidth) / 2.0f;
const float beltEnd = beltWidth / 2.0f + beltLength;

const int railCount = 20;
const int parcelCount = 5;
const float railHeight = 0.06f;
const float parcelHeight = 1.0f;
const float turnRadius = beltWidth / 2.0f;
const float halfPi = glm::radians(90.0f);
const float cornerLength = turnRadius * halfPi;
const float pathLength = beltLength * 2.0f + cornerLength;
const float beltSpeed = 1.5f; // 초당 이동 거리. 상자와 줄무늬가 같은 속도로 움직인다.

Rail rail[railCount] = {};
Rail parcel[parcelCount] = {};
const glm::vec3 parcelColors[parcelCount] = {
	glm::vec3(0.85f, 0.20f, 0.18f),
	glm::vec3(0.95f, 0.55f, 0.12f),
	glm::vec3(0.90f, 0.78f, 0.12f),
	glm::vec3(0.20f, 0.65f, 0.30f),
	glm::vec3(0.20f, 0.40f, 0.85f)
};

//--- 실습 24: 트럭, 적재 상태와 상자 시점
const float floorHalfSize = 25.0f;
const float floorTop = beltBottom - 2.0f;
const float truckX = 4.0f;
const float truckHalfLength = 3.65f; // 전조등 돌출 부분까지 포함
const float truckLimit = floorHalfSize - truckHalfLength;
const float truckSpeed = 4.0f;
const float loadingDuration = 0.9f;
float truckZ = -6.0f;
int truckDirection = 1;
bool truckMoving = false;
bool firstPerson = false;
int cameraParcel = parcelCount - 1; // 처음 선택된 시점은 5번 상자
bool parcelLoaded[parcelCount] = {};
float loadingTime[parcelCount] = {};
glm::vec3 loadingStart[parcelCount] = {};
float loadingAngle[parcelCount] = {};

glm::vec3 truckParcelPosition(int index);
void loadParcel(int index);
void drawTruck(const glm::mat4& viewProjection, GLint location);

bool beltMoving = true;
bool perspectiveProjection = true;
double beltTravel = 0.0;
float cameraDistance = 36.0f;
float cameraYaw = glm::radians(-25.0f);
const float cameraElevation = glm::radians(45.0f);

void setPathPosition(Rail& item, float distance, float centerY);
void updateAnimation(float deltaTime);
void resetScene();
void drawCuboid(const glm::mat4& model, const glm::vec3& color,
	const glm::mat4& viewProjection, GLint location);
void drawParcelDots(int number, const glm::mat4& parcelModel,
	const glm::mat4& viewProjection, GLint location);

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
	window = glfwCreateWindow(width, height,
		"Practice 24 | 1-5: Load | V: View | n/N: Next/Previous | G: Truck | B: Belt | R: Reset | Q: Quit",
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

	glEnable(GL_DEPTH_TEST);
	resetScene();
	double previousTime = glfwGetTime();
	//--- 메인 루프
	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		double currentTime = glfwGetTime();
		float deltaTime = static_cast<float>(currentTime - previousTime);
		previousTime = currentTime;
		updateAnimation(deltaTime);

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

// 경로 거리 하나로 위치와 각도를 함께 구한다.
// (-11, 0) -> (-1, 0) -> 원호 -> (0, -1) -> (0, -11), 좌표는 XZ 기준.
void setPathPosition(Rail& item, float distance, float centerY)
{
	float s = std::fmod(distance, pathLength);
	if (s < 0.0f) s += pathLength;
	item.y = centerY;
	item.isTurn = false;
	item.isOver = false;

	if (s < beltLength)
	{
		item.x = -beltEnd + s;
		item.z = 0.0f;
		item.radian = 0.0f;
	}
	else if (s < beltLength + cornerLength)
	{
		float angle = (s - beltLength) / turnRadius;
		item.x = -turnRadius + turnRadius * std::sin(angle);
		item.z = -turnRadius + turnRadius * std::cos(angle);
		item.radian = angle;
		item.isTurn = true;
	}
	else
	{
		float straightDistance = s - beltLength - cornerLength;
		item.x = 0.0f;
		item.z = -turnRadius - straightDistance;
		item.radian = halfPi;
		item.isOver = true;
	}
}

// 상자를 트럭의 로컬 좌표에 배치하므로 트럭이 움직여도 함께 이동한다.
glm::vec3 truckParcelPosition(int index)
{
	return glm::vec3(truckX + (index % 2 == 0 ? -0.65f : 0.65f),
		-0.20f, truckZ - 2.30f + (index / 2) * 1.40f);
}

void loadParcel(int index)
{
	if (parcelLoaded[index]) return;
	parcelLoaded[index] = true;
	loadingTime[index] = 0.0f;
	loadingStart[index] = glm::vec3(parcel[index].x, parcel[index].y, parcel[index].z);
	loadingAngle[index] = parcel[index].radian;
}

void updateAnimation(float deltaTime)
{
	if (truckMoving)
	{
		truckZ = glm::clamp(truckZ + truckSpeed * truckDirection * deltaTime,
			-truckLimit, truckLimit);
		if (truckZ >= truckLimit || truckZ <= -truckLimit)
		{
			truckMoving = false;
			truckDirection = -truckDirection; // 다음 g 입력 시 반대 방향으로 출발
		}
	}
	if (beltMoving)
		beltTravel = std::fmod(beltTravel + beltSpeed * deltaTime,
			static_cast<double>(pathLength));

	const float railSpacing = pathLength / railCount;
	for (int i = 0; i < railCount; ++i)
	{
		float distance = static_cast<float>(beltTravel) + i * railSpacing;
		setPathPosition(rail[i], distance, beltTop + railHeight / 2.0f);
	}

	const float parcelSpacing = pathLength / parcelCount;
	for (int i = 0; i < parcelCount; ++i)
	{
		if (parcelLoaded[i])
		{
			loadingTime[i] = glm::min(loadingTime[i] + deltaTime, loadingDuration);
			float t = loadingTime[i] / loadingDuration;
			float travel = glm::smoothstep(0.0f, 1.0f, t);
			glm::vec3 position = glm::mix(loadingStart[i], truckParcelPosition(i), travel);
			position.y += 4.0f * 3.0f * t * (1.0f - t); // 벽 위로 들어 올려 적재
			parcel[i].x = position.x;
			parcel[i].y = position.y;
			parcel[i].z = position.z;
			parcel[i].radian = glm::mix(loadingAngle[i], -halfPi, travel);
			parcel[i].isTurn = parcel[i].isOver = false;
			continue;
		}
		// 줄무늬 사이에 상자를 배치하고 동일한 beltTravel로 함께 이동한다.
		float distance = static_cast<float>(beltTravel) + i * parcelSpacing + railSpacing / 2.0f;
		setPathPosition(parcel[i], distance, beltTop + parcelHeight / 2.0f);
	}
}

void resetScene()
{
	truckZ = -6.0f;
	truckDirection = 1;
	truckMoving = firstPerson = false;
	cameraParcel = parcelCount - 1;
	for (int i = 0; i < parcelCount; ++i)
	{
		parcelLoaded[i] = false;
		loadingTime[i] = 0.0f;
		loadingStart[i] = glm::vec3(0.0f);
		loadingAngle[i] = 0.0f;
	}
	beltTravel = 0.0;
	beltMoving = true;
	perspectiveProjection = true;
	cameraDistance = 36.0f;
	cameraYaw = glm::radians(-25.0f);
	updateAnimation(0.0f);
}

// 같은 큐브 정점을 사용하되 면별 색을 조금 다르게 하여 입체감을 준다.
void drawCuboid(const glm::mat4& model, const glm::vec3& color,
	const glm::mat4& viewProjection, GLint location)
{
	glm::mat4 transform = viewProjection * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	const float faceBrightness[] = { 0.80f, 0.88f, 0.72f, 0.76f, 1.0f, 0.60f };
	for (int face = 0; face < 6; ++face)
	{
		glm::vec3 faceColor = color * faceBrightness[face];
		glVertexAttrib3f(1, faceColor.r, faceColor.g, faceColor.b);
		glDrawArrays(GL_TRIANGLES, face * 6, 6);
	}
}

// 기존 큐브를 납작하게 줄여 상자 윗면에 검은 네모를 1~5개씩 그린다.
void drawParcelDots(int number, const glm::mat4& parcelModel,
	const glm::mat4& viewProjection, GLint location)
{
	const float dotSpacing = 0.15f;
	const float dotSize = 0.11f;
	glVertexAttrib3f(1, 0.02f, 0.02f, 0.02f);
	for (int dot = 0; dot < number; ++dot)
	{
		float x = (dot - (number - 1) / 2.0f) * dotSpacing;
		glm::mat4 dotModel = glm::translate(parcelModel,
			glm::vec3(x, parcelHeight / 2.0f + 0.004f, 0));
		dotModel = glm::scale(dotModel, glm::vec3(dotSize, 0.008f, dotSize));
		glm::mat4 transform = viewProjection * dotModel;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	}
}

//--- 트럭: 차체, 운전석, 바퀴까지 기존 육면체 정점으로 그린다.
void drawTruck(const glm::mat4& viewProjection, GLint location)
{
	glm::mat4 groupMove = glm::translate(glm::mat4(1.0f), glm::vec3(truckX, 0, truckZ));
	const auto part = [&](const glm::vec3& position, const glm::vec3& size, const glm::vec3& color)
	{
		glm::mat4 model = glm::translate(groupMove, position);
		model = glm::scale(model, size);
		drawCuboid(model, color, viewProjection, location);
	};

	// 앞은 +Z. 적재함은 뒤쪽으로 열어 상자와 번호가 보이게 한다.
	part(glm::vec3(0, -1.15f, 0), glm::vec3(2.6f, 0.35f, 6.8f), glm::vec3(0.18f));
	part(glm::vec3(0, -0.85f, -0.8f), glm::vec3(2.8f, 0.3f, 4.8f), glm::vec3(0.65f));
	part(glm::vec3(-1.35f, -0.4f, -0.8f), glm::vec3(0.1f, 0.6f, 4.8f), glm::vec3(0.75f, 0.6f, 0.3f));
	part(glm::vec3( 1.35f, -0.4f, -0.8f), glm::vec3(0.1f, 0.6f, 4.8f), glm::vec3(0.75f, 0.6f, 0.3f));
	part(glm::vec3(0, -0.4f, -3.15f), glm::vec3(2.8f, 0.6f, 0.1f), glm::vec3(0.75f, 0.6f, 0.3f));

	part(glm::vec3(0, -0.05f, 2.6f), glm::vec3(2.7f, 2.0f, 2.0f), glm::vec3(0.8f, 0.83f, 0.88f));
	part(glm::vec3(0, 0.30f, 3.61f), glm::vec3(2.15f, 0.65f, 0.025f), glm::vec3(0.18f, 0.36f, 0.50f));
	for (int side = -1; side <= 1; side += 2)
	{
		part(glm::vec3(side * 1.36f, 0.3f, 2.6f), glm::vec3(0.025f, 0.65f, 1.45f), glm::vec3(0.18f, 0.36f, 0.50f));
		part(glm::vec3(side * 0.95f, -0.65f, 3.61f), glm::vec3(0.4f, 0.25f, 0.03f), glm::vec3(1.0f, 0.95f, 0.65f));
		for (float axle : {-2.1f, 2.2f})
		{
			part(glm::vec3(side * 1.4f, floorTop + 0.6f, axle),
				glm::vec3(0.4f, 1.2f, 1.2f), glm::vec3(0.07f));
			part(glm::vec3(side * 1.61f, floorTop + 0.6f, axle),
				glm::vec3(0.03f, 0.5f, 0.5f), glm::vec3(0.5f));
		}
	}
}

//--- 출력 함수
GLvoid drawScene()
{
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	if (width == 0 || height == 0) return;
	glViewport(0, 0, width, height);

	glClearColor(0.678f, 0.847f, 0.902f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float aspect = static_cast<float>(width) / height;

	glm::mat4 projection;
	if (firstPerson || perspectiveProjection)
		projection = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 150.0f);
	else
	{
		// 같은 거리의 원근 투영과 비슷한 화면 크기로 전환한다.
		float halfHeight = cameraDistance * std::tan(glm::radians(25.0f));
		projection = glm::ortho(-halfHeight * aspect, halfHeight * aspect,
			-halfHeight, halfHeight, 0.1f, 150.0f);
	}

	float horizontalDistance = cameraDistance * std::cos(cameraElevation);
	glm::vec3 cameraPosition(horizontalDistance * std::sin(cameraYaw),
		cameraDistance * std::sin(cameraElevation), horizontalDistance * std::cos(cameraYaw));
	const glm::vec3 sceneCenter(-2.0f, -0.5f, -4.0f);
	glm::mat4 view = glm::lookAt(cameraPosition + sceneCenter, sceneCenter, glm::vec3(0, 1, 0));
	if (firstPerson)
	{
		const Rail& p = parcel[cameraParcel];
		glm::vec3 forward(std::cos(p.radian), 0.0f, -std::sin(p.radian));
		// 상자 윗면 바로 위에서 진행 방향을 본다. 적재 후에는 트럭과 함께 이동한다.
		glm::vec3 eye(p.x, p.y + parcelHeight / 2.0f + 0.15f, p.z);
		view = glm::lookAt(eye, eye + forward, glm::vec3(0, 1, 0));
	}
	glm::mat4 viewProjection = projection * view;

	const float wallThickness = 0.25f;
	const float wallHeight = 1.0f;
	const float wallY = beltBottom + wallHeight / 2.0f;
	const float wallOffset = (beltWidth + wallThickness) / 2.0f;

	const float legHeight = 2.0f;
	const float legWidth = 0.4f;
	const float legY = floorTop + legHeight / 2.0f;

	glm::mat4 model(1.0f);
	glm::mat4 groupMove(1.0f);

	glm::mat4 transform = projection * view * model;

	glUseProgram(shaderProgramID);

	GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	glBindVertexArray(VAO);

	glVertexAttrib3f(1, 0.25f, 0.25f, 0.25f);
	model = glm::translate(model, glm::vec3(0, floorTop - 0.05f, 0));
	model = glm::scale(model, glm::vec3(floorHalfSize * 2.0f, 0.1f, floorHalfSize * 2.0f));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36); // 바닥

	// 코너: XZ 평면에서 2 x 2인 정사각형. 중심은 정확히 (0, 0, 0).
	glVertexAttrib3f(1, 0.42f, 0.42f, 0.42f);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, beltY, 0));
	model = glm::scale(model, glm::vec3(beltWidth, beltHeight, beltWidth));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 왼쪽 직선 벨트: X = -11 ~ -1, 길이 10.
	glVertexAttrib3f(1, 0.5f, 0.5f, 0.5f);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-armCenter, beltY, 0));
	model = glm::scale(model, glm::vec3(beltLength, beltHeight, beltWidth));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 위쪽 직선 벨트: Z = -11 ~ -1, 길이 10.
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, beltY, -armCenter));
	model = glm::scale(model, glm::vec3(beltWidth, beltHeight, beltLength));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 가로 바깥벽: 코너 바깥쪽까지 이어진다.
	glVertexAttrib3f(1, 0.8f, 0.65f, 0.15f);
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-beltLength / 2.0f, wallY, wallOffset));
	model = glm::scale(model, glm::vec3(beltLength + beltWidth, wallHeight, wallThickness));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 가로 안쪽벽
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-armCenter, wallY, -wallOffset));
	model = glm::scale(model, glm::vec3(beltLength, wallHeight, wallThickness));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 세로 안쪽벽: 가로 안쪽벽과 겹치지 않도록 두께만큼 짧게 만든다.
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-wallOffset, wallY, -armCenter - wallThickness / 2.0f));
	model = glm::scale(model, glm::vec3(wallThickness, wallHeight, beltLength - wallThickness));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 세로 바깥벽: 코너와 가로 바깥벽 끝까지 이어진다.
	model = glm::mat4(1.0f);
	groupMove = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(wallOffset, wallY, -beltLength / 2.0f + wallThickness / 2.0f));
	model = glm::scale(model, glm::vec3(wallThickness, wallHeight, beltLength + beltWidth + wallThickness));
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 양쪽 벨트 끝에 2개씩, 코너 아래에 2개: 총 6개의 다리.
	const glm::vec3 legPositions[] = {
		glm::vec3(-beltEnd + 1.0f, legY, -0.65f),
		glm::vec3(-beltEnd + 1.0f, legY,  0.65f),
		glm::vec3(-0.65f, legY, -beltEnd + 1.0f),
		glm::vec3( 0.65f, legY, -beltEnd + 1.0f),
		glm::vec3(-0.65f, legY, -0.65f),
		glm::vec3( 0.65f, legY,  0.65f)
	};
	glVertexAttrib3f(1, 0.3f, 0.3f, 0.3f);
	for (const glm::vec3& legPosition : legPositions)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, legPosition);
		model = glm::scale(model, glm::vec3(legWidth, legHeight, legWidth));
		transform = projection * view * model;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// 구조체 배열의 줄무늬 20개: 경로 전체에 같은 간격으로 배치한다.
	glVertexAttrib3f(1, 0, 0, 0);
	for (const Rail& r : rail)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(r.x, r.y, r.z));
		model = glm::rotate(model, r.radian, glm::vec3(0, 1, 0));
		// 벽에 걸리지 않도록 양 끝에 작은 여유를 둔다.
		model = glm::scale(model, glm::vec3(0.12f, railHeight, beltWidth - 0.12f));
		transform = viewProjection * model;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	drawTruck(viewProjection, location);

	// 적재 전에는 벨트를 따라, 적재 후에는 트럭을 따라 이동한다.
	for (int i = 0; i < parcelCount; ++i)
	{
		if (firstPerson && i == cameraParcel) continue; // 자기 상자가 시야를 가리지 않게 한다.
		const Rail& p = parcel[i];
		glm::mat4 parcelModel = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y, p.z));
		parcelModel = glm::rotate(parcelModel, p.radian, glm::vec3(0, 1, 0));
		model = glm::scale(parcelModel, glm::vec3(0.9f, parcelHeight, 0.9f));
		drawCuboid(model, parcelColors[i], viewProjection, location);
		drawParcelDots(i + 1, parcelModel, viewProjection, location);
	}
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
	const bool firstPress = action == GLFW_PRESS;
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5 && firstPress)
		loadParcel(key - GLFW_KEY_1);
	else if (key == GLFW_KEY_V && firstPress)
		firstPerson = !firstPerson;
	else if (key == GLFW_KEY_N && firstPress && firstPerson)
	{
		int step = (mods & GLFW_MOD_SHIFT) == 0 ? 1 : -1;
		cameraParcel = (cameraParcel + step + parcelCount) % parcelCount;
	}
	else if (key == GLFW_KEY_G && firstPress)
		truckMoving = !truckMoving;
	else if (key == GLFW_KEY_B && firstPress)
		beltMoving = !beltMoving;
	else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
		cameraDistance = std::max(12.0f, cameraDistance - 1.0f);
	else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
		cameraDistance = std::min(65.0f, cameraDistance + 1.0f);
	else if (key == GLFW_KEY_P && firstPress)
		perspectiveProjection = (mods & GLFW_MOD_SHIFT) == 0; // p: 원근, P: 직각
	else if (key == GLFW_KEY_LEFT)
		cameraYaw += glm::radians(5.0f); // +Y에서 내려다볼 때 시계 방향
	else if (key == GLFW_KEY_RIGHT)
		cameraYaw -= glm::radians(5.0f);
	else if (key == GLFW_KEY_R && firstPress)
		resetScene();
	else if (key == GLFW_KEY_Q && firstPress)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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

