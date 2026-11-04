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

	glm::mat4 model(1.0f);

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f); // 원근투영
	/*float aspect = float(width) / float(height);
	glm::mat4 projection = glm::ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, 0.1f, 100.0f);*/ //직각투영

	glm::mat4 transform = projection * view * model;

	GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));

	glVertexAttrib3f(1, 0.25f, 0.25f, 0.25f);
	model = glm::mat4(1.0f);
	transform = projection * view * model;
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	location = glGetUniformLocation(shaderProgramID, "modelTransform");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));


	glBindVertexArray(0);

}

//--- 키보드 입력 콜백 함수
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
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

	const GLuint frontOutline[] = { 12, 13, 14, 17 };
	glGenBuffers(1, &frontOutlineEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frontOutlineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frontOutline), frontOutline, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

/*
화면 분할 - 지금 drawScene()에서 쓰는 코드와 같은 방식

- 아래 함수 부분만 복사해서 위에 있는 기존 drawScene() 전체와 교체하면 된다.
- for문으로 같은 장면을 두 번 그린다. n == 0은 왼쪽, n == 1은 오른쪽이다.
- if문 안에서는 그릴 화면의 위치, 카메라, 투영 방식만 정한다.
- if문 아래에서는 지금처럼 model -> transform -> uniform -> glDrawArrays 순서로 그린다.

GLvoid drawScene()
{
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	if (width < 2 || height < 1) return;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	// angle 증가처럼 물체를 움직이는 코드는 이곳에서 한 번만 실행한다.

	for (int n = 0; n < 2; n++)
	{
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 projection(1.0f);
		float aspect;

		if (n == 0) // 왼쪽 화면: 원근투영
		{
			// 왼쪽 아래 (0, 0)부터 창의 왼쪽 절반에 그린다.
			glViewport(0, 0, width / 2, height);
			aspect = float(width / 2) / float(height);

			view = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		}
		else // 오른쪽 화면: 직각투영
		{
			// x를 width / 2부터 시작해서 창의 오른쪽 절반에 그린다.
			// width - width / 2는 왼쪽을 제외한 나머지 너비다.
			glViewport(width / 2, 0, width - width / 2, height);
			aspect = float(width - width / 2) / float(height);

			view = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			projection = glm::ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, 0.1f, 100.0f);
		}

		// 여기부터는 네가 원래 물체를 그리던 코드와 같다.
		glm::mat4 transform = projection * view * model;
		GLint location = glGetUniformLocation(shaderProgramID, "modelTransform");

		glVertexAttrib3f(1, 0.25f, 0.25f, 0.25f);
		model = glm::mat4(1.0f);
		// 물체의 translate, rotate, scale은 여기에 넣는다.
		transform = projection * view * model;
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 바닥, 기둥, 그네, 사람 등 나머지 물체를 그리는 코드도 여기에 넣는다.
		// 각 물체마다 model을 초기화하고 transform을 다시 계산해서 그린다.
	}

	glBindVertexArray(0);
}

기억할 것
- n == 0에서 왼쪽 장면을 그리고, n == 1에서 같은 장면을 오른쪽에 다시 그린다.
- aspect에는 전체 창이 아니라 지금 그리고 있는 영역의 가로 / 세로를 넣는다.
- glViewport(x, y, 가로, 세로)의 x, y는 화면의 왼쪽 아래를 기준으로 한다.
- glClear는 for문 밖에서 한 번만 한다. 안에 넣으면 앞에서 그린 화면도 지워진다.
- angle도 for문 밖에서 증가시킨다. 안에 넣으면 두 화면의 각도가 달라진다.

22번처럼 왼쪽 하나 + 오른쪽 위/아래로 세 개를 그리고 싶을 때
- 위 예제의 for문을 for (int n = 0; n < 3; n++)로 바꾼다.
- 크기 검사도 if (width < 2 || height < 2) return; 으로 바꾼다.
- 위 예제의 if (n == 0) ... else 부분을 아래 코드로 교체한다.
- 그 아래의 model, transform, uniform, glDrawArrays 부분은 그대로 사용한다.

		if (n == 0) // 왼쪽 전체: 원근투영
		{
			glViewport(0, 0, width / 2, height);
			aspect = float(width / 2) / float(height);
			view = glm::lookAt(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		}
		else if (n == 1) // 오른쪽 위: 위에서 보는 직각투영
		{
			glViewport(width / 2, height / 2, width - width / 2, height - height / 2);
			aspect = float(width - width / 2) / float(height - height / 2);
			// 바로 아래를 볼 때는 up을 (0, 0, -1)로 정한다.
			view = glm::lookAt(glm::vec3(0, 20, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
			projection = glm::ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, 0.1f, 100.0f);
		}
		else // 오른쪽 아래: 정면에서 보는 직각투영
		{
			glViewport(width / 2, 0, width - width / 2, height / 2);
			aspect = float(width - width / 2) / float(height / 2);
			view = glm::lookAt(glm::vec3(0, 0, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			projection = glm::ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, 0.1f, 100.0f);
		}
*/


/*
자전과 공전 - 회전 중심을 기준으로 구분하기

- 자전: 물체가 자기 중심을 기준으로 돈다.
- 공전: 물체가 다른 지점을 중심으로 돈다.
- TRT는 특정 점을 회전 중심으로 잡는 방법이다. 자전과 공전 모두에 쓸 수 있다.
- 아래 예제는 지금 vertices처럼 물체의 중심이 원점인 큐브를 기준으로 한다.
- 예제들은 각각 따로 사용하는 방법이다. 필요한 예제 하나를 물체 그리는 곳에 넣는다.

행렬식으로 보기 (T: 이동, R: 회전, S: 크기)
    자기 중심 자전:         T(물체 위치) * R(자전) * S(크기)
    다른 점을 중심으로 공전: T(공전 중심) * R(공전) * T(반지름만큼 이동)
    공전하면서 자전:        T(공전 중심) * R(공전) * T(반지름만큼 이동) * R(자전) * S(크기)
    이미 배치된 물체 회전:  T(P) * R * T(-P) * 기존 model

GLM은 아래처럼 함수를 작성하면 위 행렬식 순서대로 만들어진다.
정점에 실제로 적용되는 순서는 행렬의 오른쪽부터다.

각도가 필요하면 전역변수로 추가한다.
    float orbitAngle = 0.0f;
    float spinAngle = 0.0f;

애니메이션을 할 때는 drawScene()에서 한 프레임에 한 번만 증가시킨다.
화면 분할 for문이 있다면 그 for문 전에 넣는다.
    orbitAngle += 0.5f;
    spinAngle += 2.0f;

1. 원점에서 만든 큐브를 (5, 0, 0)에 놓고 자기 중심으로 자전

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.0f, 0, 0));
    model = glm::rotate(model, glm::radians(spinAngle), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(0.5f));

    // 위치는 (5, 0, 0)에 유지되고 물체의 방향이 바뀐다.
    // 물체가 원점 중심으로 만들어져 있으므로 자전 자체는 R로 처리할 수 있다.

2. (10, 0, 0)을 중심으로 반지름 5로 공전하면서 자전

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10.0f, 0, 0));                 // 공전 중심
    model = glm::rotate(model, glm::radians(orbitAngle), glm::vec3(0, 1, 0)); // 공전
    model = glm::translate(model, glm::vec3(5.0f, 0, 0));                  // 반지름
    model = glm::rotate(model, glm::radians(spinAngle), glm::vec3(0, 1, 0));  // 자전
    model = glm::scale(model, glm::vec3(0.5f));

    // 두 각도가 0이면 물체 중심은 (15, 0, 0)에 있다.
    // 공전 중심이 원점이면 맨 앞의 translate(10, 0, 0)을 빼면 된다.
    // spinAngle은 공전 좌표계 안에서 추가로 도는 자전 각도다.

3. 이미 배치된 model을 특정 점 P 주위로 돌리고 싶을 때: TRT

    // model이 이미 만들어져 있다고 가정한다. 여기서는 model을 초기화하지 않는다.
    // P도 model이 배치된 것과 같은 월드 좌표 기준으로 정한다.
    glm::vec3 p(10.0f, 0, 0);
    glm::mat4 move(1.0f);
    move = glm::translate(move, p);
    move = glm::rotate(move, glm::radians(orbitAngle), glm::vec3(0, 1, 0));
    move = glm::translate(move, -p);
    model = move * model;

    // 정점에는 -P 이동 -> 회전 -> +P 이동 순서로 적용되어 P가 회전 중심이 된다.
    // P를 물체 자신의 중심으로 정하면 자전, 다른 지점으로 정하면 공전이다.

model을 만든 다음에는 지금처럼 셰이더에 전달하고 그린다.
    transform = projection * view * model;
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawArrays(GL_TRIANGLES, 0, 36);
*/


/*
솔리드와 와이어 전환

- GL_FILL: 면을 채워서 그리는 솔리드 모드다.
- GL_LINE: 삼각형의 모서리를 선으로 그리는 와이어 모드다.
- OpenGL의 기본값은 GL_FILL이므로 처음에는 설정하지 않아도 솔리드로 보인다.
- 와이어로 바꾼 뒤 다시 솔리드로 돌아오고 싶을 때 GL_FILL로 설정하면 된다.

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 와이어로 변경
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 솔리드로 복구

위 두 줄은 각각의 사용 예다. 원하는 모드 한 줄을 실행하면 된다.
위처럼 두 줄을 연속 실행하면 마지막 설정인 GL_FILL이 적용된다.

m은 솔리드, Shift+M은 와이어로 바꾸려면 keyCallback() 안에 아래 코드를 추가한다.

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        if (mods & GLFW_MOD_SHIFT)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // M: 와이어
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // m: 솔리드
        }
    }

- 설정은 다음에 바꿀 때까지 유지되므로 매 프레임 다시 설정할 필요는 없다.
- drawScene()에서 매번 GL_FILL로 설정하면 키로 바꾼 GL_LINE이 다시 덮어써진다.
- glDrawArrays(GL_TRIANGLES, 0, 36);은 그대로 사용한다.
- 큐브의 각 사각형 면은 삼각형 2개이므로 와이어에서는 대각선도 보인다.
- 컬링이 켜져 있다면 제거된 면의 선은 와이어 모드에서도 그려지지 않는다.
*/


/*
카메라가 제자리에서 자전하기 (좌우로 돌아보기)

카메라 위치 (0, 10, 20)는 유지하고, lookAt의 두 번째 벡터인 바라보는 점만 바꾼다.
cameraAngle이 0이면 기존 코드처럼 원점 (0, 0, 0)을 바라본다.
각도가 바뀌어도 내려다보는 기울기는 유지된다.

1. 함수 밖의 전역 변수에 추가한다.

    float cameraAngle = 0.0f;

2. drawScene()에서 기존 view를 만드는 부분을 아래 코드로 교체한다.

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 10.0f, 20.0f), // 카메라 위치는 고정
        glm::vec3(
            -20.0f * glm::sin(glm::radians(cameraAngle)),
            0.0f,
            20.0f - 20.0f * glm::cos(glm::radians(cameraAngle))
        ),
        glm::vec3(0, 1, 0)
    );

    // 바라보는 점 = 카메라 위치 + 회전된 시선 방향
    // 시선 방향은 (-20 * sin(각도), -10, -20 * cos(각도))이다.
    // cameraAngle은 도 단위이므로 sin, cos에 넣기 전에 radians로 변환한다.

3. keyCallback() 안에 아래 코드를 추가한다.

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_LEFT)
            cameraAngle += 5.0f;

        if (key == GLFW_KEY_RIGHT)
            cameraAngle -= 5.0f;
    }

    // 왼쪽 방향키: 왼쪽으로 돌아보기
    // 오른쪽 방향키: 오른쪽으로 돌아보기
    // cameraAngle을 drawScene() 안의 지역 변수로 선언하면 매번 0으로 초기화되므로
    // 함수 밖의 전역 변수로 선언한다.
*/
