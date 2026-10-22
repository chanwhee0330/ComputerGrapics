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
#include <vector>
#include <cerrno>
#include <cmath>
#include <limits>

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

typedef struct {
	float x, y, z;
} Vertex;

typedef struct {
	unsigned int v1, v2, v3;
} Face;

// OBJ의 l을 저장하는 선분 구조체: 정점 번호 2개와 RGB 색상
typedef struct {
	unsigned int v1, v2;
	size_t faces_before; // 이 선보다 앞에 나온 면의 개수: OBJ의 f/l 순서를 유지한다.
	float r, g, b;
} Line;

typedef struct {
	Vertex* vertices;
	size_t vertex_count;

	Face* faces;
	size_t face_count;

	Line* lines;
	size_t line_count;
} Model;

Model objModel = {};
std::vector<GLfloat> vertices; // 1-16.cpp처럼 도형과 축을 함께 저장: x, y, z, r, g, b
// 기본 cube.obj의 시작 위치/개수: 축 0/6, 큐브 6/36, 원뿔 42/192, 구 234/2880
GLsizei vertexCount = 0; // 도형과 축을 합친 전체 정점 개수

bool read_obj_file(const char* filename, Model* model);
void free_model(Model* model);
void make_model_vertices(const Model& model);
void InitBuffer();

//--- 실습 18: 전체 이동, 투영/그리기 모드, 애니메이션 상태
bool isP, isM, is1, is2, is3;
float moveX, moveY, moveZ;
float radi1[3] = { 0.0f, 180.0f, 0.0f }; // 행성 공전 각도
float radi2[3] = { 0.0f, 120.0f, 240.0f }; // 달 공전 각도
float radi3, radiS;
float size1 = 1.0f; // 모든 공전 궤도의 반지름 배율
float size2 = 1.0f; // 중심 구의 크기 배율
float deltaTime;
double lastTime;

const float radius[3] = { 1.6f, 1.95f, 2.3f };
const float tilt[3] = { 0.0f, 45.0f, -45.0f };
const float speed[3] = { 25.0f, 38.0f, -18.0f };
const float moonSpeed[3] = { 95.0f, -125.0f, 155.0f };
const float planetSize[3] = { 0.38f, 0.44f, 0.34f };
const glm::vec3 planetColor[3] = {
	glm::vec3(0.9f, 0.2f, 0.2f),
	glm::vec3(0.1f, 0.65f, 0.35f),
	glm::vec3(0.2f, 0.4f, 0.95f)
};
const glm::vec3 moonColor[3] = {
	glm::vec3(0.85f, 0.25f, 0.65f),
	glm::vec3(0.15f, 0.75f, 0.85f),
	glm::vec3(0.55f, 0.25f, 0.8f)
};

// cube.obj의 구를 그대로 사용하고, 두 종류의 공전 경로를 VBO 끝에 추가한다.
const GLint sphereStart = 234;
const GLsizei sphereCount = 2880;
const GLsizei orbitCount = 180;
GLint orbitXZ, orbitXY;
void make_orbit_vertices();

int main(void)
{
	// 1-17.cpp와 같은 OBJ에서 구의 정점 데이터를 읽는다.
	const char* objPath = "cube.obj";
	if (!read_obj_file(objPath, &objModel))
		return EXIT_FAILURE;

	std::cout << "OBJ: " << objPath << " / vertices: " << objModel.vertex_count
		<< " / triangles: " << objModel.face_count << " / lines: " << objModel.line_count << '\n';
	make_model_vertices(objModel);
	free_model(&objModel); // 좌표를 vertices에 옮겼으므로 원본 메모리 해제

	if (vertexCount < sphereStart + sphereCount)
	{
		std::cerr << "cube.obj에 실습에 사용할 구 데이터가 없습니다.\n";
		return EXIT_FAILURE;
	}
	make_orbit_vertices();
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
	window = glfwCreateWindow(width, height, "OpenGL Practice 18", NULL, NULL);
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
	lastTime = glfwGetTime();
	std::cout << "p/P: orthographic/perspective, m/M: solid/wire\n"
		<< "WASD: move X/Y, +/-: move Z, 1: orbit size\n"
		<< "2: center animation, 3: moon orbit rotation, Q: quit\n";
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
	// 프레임 수와 관계없이 같은 속도로 공전하도록 경과 시간을 사용한다.
	double currentTime = glfwGetTime();
	deltaTime = static_cast<float>(currentTime - lastTime);
	lastTime = currentTime;
	if (deltaTime > 0.1f) deltaTime = 0.1f;

	for (int i = 0; i < 3; i++)
	{
		radi1[i] = std::fmod(radi1[i] + speed[i] * deltaTime, 360.0f);
		radi2[i] = std::fmod(radi2[i] + moonSpeed[i] * deltaTime, 360.0f);
	}

	// 1을 누를 때마다 모든 궤도가 함께 커지거나 원래 크기로 작아진다.
	if (is1 && size1 < 1.3f)
	{
		size1 += 0.25f * deltaTime;
		if (size1 > 1.3f) size1 = 1.3f;
	}
	else if (!is1 && size1 > 1.0f)
	{
		size1 -= 0.25f * deltaTime;
		if (size1 < 1.0f) size1 = 1.0f;
	}

	if (is2)
	{
		radiS = std::fmod(radiS + 100.0f * deltaTime, 360.0f);
		size2 = 1.0f + 0.25f * std::sin(glm::radians(radiS));
	}
	if (is3)
		radi3 = std::fmod(radi3 + 45.0f * deltaTime, 360.0f);

	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	if (width == 0 || height == 0) return;
	glViewport(0, 0, width, height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, isM ? GL_LINE : GL_FILL);

	// 기존 셰이더를 유지하면서 각 구와 궤도에 한 가지 색을 지정한다.
	glDisableVertexAttribArray(1);
	GLint modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	glm::mat4 projection(1.0f);
	if (isP)
		projection = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 100.0f);
	else
		projection = glm::ortho(-3.8f * aspect, 3.8f * aspect, -3.8f, 3.8f, 0.1f, 100.0f);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 3.5f, 8.0f),
		glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 scene(1.0f);
	scene = glm::translate(scene, glm::vec3(moveX, moveY, moveZ));
	glm::mat4 model(1.0f);
	glm::mat4 transform(1.0f);

	// 중심 구: 크기와 색은 2번 애니메이션을 멈추면 현재 상태를 유지한다.
	model = glm::scale(scene, glm::vec3(0.9f * size2));
	transform = projection * view * model;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
	glVertexAttrib3f(1, 0.95f, 0.55f + 0.2f * std::sin(glm::radians(radiS)),
		0.15f + 0.1f * std::sin(glm::radians(radiS)));
	glDrawArrays(GL_TRIANGLES, sphereStart, sphereCount);

	// view의 회전을 반대로 적용해서 달의 기본 궤도를 화면과 평행하게 둔다.
	glm::mat4 screen = glm::transpose(glm::mat4(glm::mat3(view)));
	for (int i = 0; i < 3; i++)
	{
		// 행성 궤도: xz 평면을 z축에 대해 0, +45, -45도 기울인다.
		glm::mat4 orbit(1.0f);
		orbit = glm::rotate(orbit, glm::radians(tilt[i]), glm::vec3(0, 0, 1));
		model = glm::scale(scene * orbit, glm::vec3(radius[i] * size1));
		transform = projection * view * model;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glVertexAttrib3f(1, 0.55f, 0.55f, 0.55f);
		glDrawArrays(GL_LINE_LOOP, orbitXZ, orbitCount);

		// 기울기 -> 공전 -> 반지름만큼 이동한 위치가 행성의 중심이다.
		orbit = glm::rotate(orbit, glm::radians(radi1[i]), glm::vec3(0, 1, 0));
		glm::vec3 position = glm::vec3(orbit * glm::vec4(radius[i] * size1, 0, 0, 1));
		glm::mat4 planet = glm::translate(scene, position);
		model = glm::scale(planet, glm::vec3(planetSize[i]));
		transform = projection * view * model;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glVertexAttrib3f(1, planetColor[i].r, planetColor[i].g, planetColor[i].b);
		glDrawArrays(GL_TRIANGLES, sphereStart, sphereCount);

		// 행성의 중심을 원점으로 한 달의 좌표계. 3번은 이 좌표계의 z축 회전이다.
		// 타원으로 그려서 궤도 자체가 회전하는 모습도 구별할 수 있게 한다.
		glm::mat4 moonOrbit = planet * screen;
		moonOrbit = glm::rotate(moonOrbit, glm::radians(radi3), glm::vec3(0, 0, 1));
		moonOrbit = glm::scale(moonOrbit,
			glm::vec3((0.48f + i * 0.05f) * size1, (0.28f + i * 0.02f) * size1, 1.0f));
		transform = projection * view * moonOrbit;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glVertexAttrib3f(1, 0.65f, 0.65f, 0.65f);
		glDrawArrays(GL_LINE_LOOP, orbitXY, orbitCount);

		// 궤도 위의 달 위치만 가져온 뒤 구를 그려 타원 배율이 달에 적용되지 않게 한다.
		glm::vec3 moonPosition = glm::vec3(moonOrbit * glm::vec4(
			std::cos(glm::radians(radi2[i])), std::sin(glm::radians(radi2[i])), 0, 1));
		model = glm::translate(glm::mat4(1.0f), moonPosition);
		model = glm::scale(model, glm::vec3(0.14f));
		transform = projection * view * model;
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		glVertexAttrib3f(1, moonColor[i].r, moonColor[i].g, moonColor[i].b);
		glDrawArrays(GL_TRIANGLES, sphereStart, sphereCount);
	}
	glEnableVertexAttribArray(1);
}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

	// 이동은 키를 누르고 있으면 계속 적용한다.
	if (key == GLFW_KEY_W) moveY += 0.1f;
	else if (key == GLFW_KEY_S) moveY -= 0.1f;
	else if (key == GLFW_KEY_A) moveX -= 0.1f;
	else if (key == GLFW_KEY_D) moveX += 0.1f;
	else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) moveZ += 0.1f;
	else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) moveZ -= 0.1f;

	if (action != GLFW_PRESS) return;
	if (key == GLFW_KEY_P)
	{
		// p: 직각 투영, Shift+P: 원근 투영
		isP = (mods & GLFW_MOD_SHIFT) != 0;
	}
	else if (key == GLFW_KEY_M)
	{
		// m: 솔리드 모델, Shift+M: 와이어 모델
		isM = (mods & GLFW_MOD_SHIFT) != 0;
	}
	else if (key == GLFW_KEY_1)
		is1 = !is1;
	else if (key == GLFW_KEY_2)
		is2 = !is2;
	else if (key == GLFW_KEY_3)
		is3 = !is3;
	else if (key == GLFW_KEY_Q)
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// Location 0에서 정점마다 float 6개씩 읽도록 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

//--- malloc으로 만든 정점 배열과 면 배열 해제
void free_model(Model* model)
{
	free(model->vertices);
	free(model->faces);
	free(model->lines);
	*model = {};
}

//--- f/l의 정점 번호에서 첫 번째 숫자만 읽는다.
// 양수 번호는 1부터 시작하고, 음수 번호는 해당 줄 앞의 정점들을 역순으로 가리킨다.
bool read_obj_index(const std::string& token, size_t vertex_count,
	size_t vertices_seen, unsigned int* index)
{
	std::string number = token.substr(0, token.find('/'));
	char* end = nullptr;
	errno = 0;
	long long value = strtoll(number.c_str(), &end, 10);
	if (errno == ERANGE || end == number.c_str() || *end != '\0' || value == 0)
		return false;

	long long resolved = value > 0 ? value - 1 : static_cast<long long>(vertices_seen) + value;
	if (resolved < 0 || static_cast<unsigned long long>(resolved) >= vertex_count)
		return false;

	*index = static_cast<unsigned int>(resolved);
	return true;
}

//--- 자료와 같이 두 번 읽는다: 개수 세기 -> 메모리 할당 -> 좌표와 면/선 저장
// f는 삼각형, l은 연결된 선분으로 읽고, vt/vn/재질 정보는 읽어 넘긴다.
bool read_obj_file(const char* filename, Model* model)
{
	free_model(model);
	std::ifstream file(filename);
	if (!file)
	{
		std::cerr << "OBJ 파일을 열 수 없습니다: " << filename << '\n';
		return false;
	}

	std::string line, type;
	while (std::getline(file, line))
	{
		std::istringstream row(line);
		if (!(row >> type)) continue;
		if (type == "v") model->vertex_count++;
		else if (type == "f") model->face_count++;
		else if (type == "l")
		{
			std::string token;
			size_t count = 0;
			while ((row >> token) && token[0] != '#') count++;
			if (count >= 2) model->line_count += count - 1;
		}
	}
	if (file.bad() || model->vertex_count == 0 || (model->face_count == 0 && model->line_count == 0) ||
		model->vertex_count > (std::numeric_limits<unsigned int>::max)() ||
		model->face_count > static_cast<size_t>((std::numeric_limits<GLsizei>::max)()) / 3 ||
		model->line_count > (static_cast<size_t>((std::numeric_limits<GLsizei>::max)()) - model->face_count * 3) / 2)
	{
		std::cerr << "OBJ에 읽을 수 있는 정점과 면 또는 선이 필요합니다.\n";
		free_model(model);
		return false;
	}

	model->vertices = static_cast<Vertex*>(malloc(model->vertex_count * sizeof(Vertex)));
	model->faces = model->face_count > 0 ? static_cast<Face*>(malloc(model->face_count * sizeof(Face))) : nullptr;
	model->lines = model->line_count > 0 ? static_cast<Line*>(malloc(model->line_count * sizeof(Line))) : nullptr;
	if (!model->vertices || (model->face_count > 0 && !model->faces) || (model->line_count > 0 && !model->lines))
	{
		std::cerr << "OBJ 메모리를 할당할 수 없습니다.\n";
		free_model(model);
		return false;
	}

	file.clear();
	file.seekg(0);
	size_t vertex_index = 0, face_index = 0, line_index = 0, line_number = 0;
	glm::vec3 lineColor(0.15f); // 축 이름이 아닌 그룹의 선은 회색
	auto fail = [&](const char* message) {
		std::cerr << filename << " (" << line_number << "번째 줄): " << message << '\n';
		free_model(model);
		return false;
	};

	while (std::getline(file, line))
	{
		line_number++;
		std::istringstream row(line);
		if (!(row >> type)) continue;
		if (type == "g")
		{
			std::string group;
			row >> group;
			// 축 색상 설정 위치: X=R, Y=G, Z=B
			if (group == "x" || group == "X") lineColor = glm::vec3(1.0f, 0.0f, 0.0f);
			else if (group == "y" || group == "Y") lineColor = glm::vec3(0.0f, 1.0f, 0.0f);
			else if (group == "z" || group == "Z") lineColor = glm::vec3(0.0f, 0.0f, 1.0f);
			else lineColor = glm::vec3(0.15f);
		}
		else if (type == "v")
		{
			if (vertex_index >= model->vertex_count) return fail("정점 개수가 변경되었습니다.");
			Vertex& v = model->vertices[vertex_index];
			if (!(row >> v.x >> v.y >> v.z) ||
				!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z))
				return fail("v 뒤에는 유효한 x, y, z 좌표가 필요합니다.");
			vertex_index++;
		}
		else if (type == "f")
		{
			if (face_index >= model->face_count) return fail("면 개수가 변경되었습니다.");
			std::string a, b, c, extra;
			if (!(row >> a >> b >> c) || ((row >> extra) && extra[0] != '#'))
				return fail("삼각형 면만 지원합니다. OBJ의 면을 삼각형으로 나누어 주세요.");

			Face& f = model->faces[face_index];
			if (!read_obj_index(a, model->vertex_count, vertex_index, &f.v1) ||
				!read_obj_index(b, model->vertex_count, vertex_index, &f.v2) ||
				!read_obj_index(c, model->vertex_count, vertex_index, &f.v3))
				return fail("면의 정점 번호가 잘못되었거나 범위를 벗어났습니다.");
			face_index++;
		}
		else if (type == "l")
		{
			std::string token;
			unsigned int previous;
			if (!(row >> token) || !read_obj_index(token, model->vertex_count, vertex_index, &previous))
				return fail("선의 첫 번째 정점 번호가 잘못되었습니다.");

			size_t segments = 0;
			while ((row >> token) && token[0] != '#')
			{
				unsigned int next;
				if (!read_obj_index(token, model->vertex_count, vertex_index, &next))
					return fail("선의 정점 번호가 잘못되었거나 범위를 벗어났습니다.");
				if (line_index >= model->line_count) return fail("선분 개수가 변경되었습니다.");
				model->lines[line_index++] = { previous, next, face_index, lineColor.r, lineColor.g, lineColor.b };
				previous = next;
				segments++;
			}
			if (segments == 0) return fail("l 뒤에는 정점 번호가 최소 2개 필요합니다.");
		}
	}
	if (file.bad() || vertex_index != model->vertex_count || face_index != model->face_count ||
		line_index != model->line_count)
		return fail("OBJ 파일을 끝까지 읽을 수 없습니다.");
	return true;
}

//--- OBJ의 면과 선을 읽은 순서대로 하나의 vertices 배열에 펼친다.
void make_model_vertices(const Model& model)
{
	vertices.clear();
	vertexCount = 0;
	vertices.reserve((model.face_count * 3 + model.line_count * 2) * 6);
	size_t line_index = 0;
	for (size_t i = 0; i <= model.face_count; i++)
	{
		// 이 면 앞에 있던 선을 먼저 추가한다. 기본 OBJ의 축은 모든 면 앞에 온다.
		while (line_index < model.line_count && model.lines[line_index].faces_before == i)
		{
			const Line& line = model.lines[line_index++];
			const unsigned int indices[] = { line.v1, line.v2 };
			for (unsigned int index : indices)
			{
				const Vertex& v = model.vertices[index];
				vertices.insert(vertices.end(), { v.x, v.y, v.z, line.r, line.g, line.b });
			}
		}
		if (i == model.face_count) break;

		const Face& f = model.faces[i];
		const unsigned int indices[] = { f.v1, f.v2, f.v3 };
		const Vertex& a = model.vertices[f.v1];
		const Vertex& b = model.vertices[f.v2];
		const Vertex& c = model.vertices[f.v3];
		glm::vec3 normal = glm::cross(glm::vec3(b.x - a.x, b.y - a.y, b.z - a.z),
			glm::vec3(c.x - a.x, c.y - a.y, c.z - a.z));
		float length = glm::length(normal);
		glm::vec3 color(0.2f, 0.6f, 0.9f);
		// 면 방향에 따라 색을 달리해서 도형의 면을 구별한다.
		if (length > 0.0f && std::isfinite(length))
			color = glm::vec3(0.2f) + 0.65f * glm::abs(normal / length);

		for (unsigned int index : indices)
		{
			const Vertex& v = model.vertices[index];
			vertices.insert(vertices.end(), { v.x, v.y, v.z, color.r, color.g, color.b });
		}
	}
	vertexCount = static_cast<GLsizei>(vertices.size() / 6);
}

//--- xz 평면의 행성 경로와 xy 평면의 달 경로를 같은 VBO에 추가한다.
void make_orbit_vertices()
{
	orbitXZ = vertexCount;
	for (int i = 0; i < orbitCount; i++)
	{
		float angle = glm::radians(360.0f * i / orbitCount);
		vertices.insert(vertices.end(), {
			std::cos(angle), 0.0f, std::sin(angle), 0.55f, 0.55f, 0.55f
		});
	}
	orbitXY = orbitXZ + orbitCount;
	for (int i = 0; i < orbitCount; i++)
	{
		float angle = glm::radians(360.0f * i / orbitCount);
		vertices.insert(vertices.end(), {
			std::cos(angle), std::sin(angle), 0.0f, 0.65f, 0.65f, 0.65f
		});
	}
	vertexCount = static_cast<GLsizei>(vertices.size() / 6);
}
