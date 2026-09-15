#include <GL/glew.h>
#include <GL/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int acction, int mods);
typedef struct Rect {
    float x, y, width, height,r,g,b;
    bool isselect,isalive;
}Rect;

typedef struct Place {
    int square;
    Rect rect[5];
}Place;

typedef struct Sector {
    float r, g, b;
}Sector;

Sector sector[4];
Place place[4];
Rect rectPlace[4];
double x, y;
bool click;
int main(void)
{
    srand((unsigned int)time(NULL));
    GLFWwindow* window;
    GLenum glewResult;

    //--- GLFW 초기화
    if (glfwInit() != GLFW_TRUE) {
        printf("GLFW initialization failed.\n");
        return EXIT_FAILURE;
    }

    //--- OpenGL 버전 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    //--- 윈도우 생성
    window = glfwCreateWindow(1500, 900, "OpenGL Basic Window (C)", NULL, NULL);
    if (window == NULL) {
        printf("Window creation failed.\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    //--- OpenGL 컨텍스트 설정
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //--- GLEW 초기화
    glewExperimental = GL_TRUE;
    glewResult = glewInit();
    if (glewResult != GLEW_OK) {
        printf("GLEW initialization failed: %s\n",
               (const char*)glewGetErrorString(glewResult));
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    //--- 뷰포트 설정
    glViewport(0, 0, 1500, 900);


    sector[0].r = 0.5f; sector[0].g = 1.0f; sector[0].b = 1.0f;
    sector[1].r = 0.4f; sector[1].g = 0.8f; sector[1].b = 1.0f;
    sector[2].r = 1.0f; sector[2].g = 0.5f; sector[2].b = 0.0f;
    sector[3].r = 0.5f; sector[3].g = 0.0f; sector[3].b = 0.5f;


    //--- 메인 루프
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        //--- 배경색 설정 후 화면 지우기
        glClear(GL_COLOR_BUFFER_BIT);
        
        //--- 1사분면
        glColor3f(sector[0].r, sector[0].g, sector[0].b);
        glRectf(0.0f, 0.0f, 1.0f, 1.0f);

        //--- 2사분면
        glColor3f(sector[1].r, sector[1].g, sector[1].b);
        glRectf(-1.0f, 0.0f, 0.0f, 1.0f);

        //--- 3사분면
        glColor3f(sector[2].r, sector[2].g, sector[2].b);
        glRectf(-1.0f, -1.0f, 0.0f, 0.0f);

        //--- 4사분면
        glColor3f(sector[3].r, sector[3].g, sector[3].b);
        glRectf(0.0f, -1.0f, 1.0f, 0.0f);
        
        //--- ESC 키를 누르면 종료
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (click == true)
        {
            bool found = false;
            click = false;

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < place[i].square; j++)
                {
                    place[i].rect[j].isselect = false;
                }
            }

            for (int i = 3; i >= 0&&found==false; i--)
            {
                for (int j = 0; j < place[i].square; j++)
                {
                    if (x >= place[i].rect[j].x && x <= place[i].rect[j].x + place[i].rect[j].width && y >= place[i].rect[j].y && y <= place[i].rect[j].y + place[i].rect[j].height)
                    {
                        place[i].rect[j].isselect = true;
                        found = true;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < place[i].square; j++)
            {
                if (place[i].rect[j].isalive == true)
                {
                    glColor3f(place[i].rect[j].r, place[i].rect[j].g, place[i].rect[j].b);
                    glRectf((float)place[i].rect[j].x, (float)place[i].rect[j].y, (float)place[i].rect[j].x + (float)place[i].rect[j].width, (float)place[i].rect[j].y + (float)place[i].rect[j].height);
                    if (place[i].rect[j].isselect == true)
                    {
                        glLineWidth(3.0f);
                        glColor3f(1.0f, 0.0f, 0.0f);

                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                        glRectf(place[i].rect[j].x, place[i].rect[j].y, place[i].rect[j].x + place[i].rect[j].width, place[i].rect[j].y + place[i].rect[j].height);

                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    }
                }
            }
        }

        //--- 버퍼 교체 및 이벤트 처리
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //--- 종료 처리
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        int index = place[0].square;
        if (index < 5)
        {
            place[0].rect[index].isalive = true;
            place[0].rect[index].isselect = false;
            place[0].rect[index].width = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;
            place[0].rect[index].height = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;

            place[0].rect[index].x = ((float)rand() / RAND_MAX) * (1.0f - place[0].rect[index].width);
            place[0].rect[index].y = ((float)rand() / RAND_MAX) * (1.0f - place[0].rect[index].height);
            place[0].rect[index].r = (float)rand() / RAND_MAX;
            place[0].rect[index].g = (float)rand() / RAND_MAX;
            place[0].rect[index].b = (float)rand() / RAND_MAX;

            place[0].square++;
        }
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        int index = place[1].square;
        if (index < 5)
        {
            place[1].rect[index].isalive = true;
            place[1].rect[index].isselect = false;
            place[1].rect[index].width = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;
            place[1].rect[index].height = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;

            place[1].rect[index].x = -1.0f + ((float)rand() / RAND_MAX) * (1.0f - place[1].rect[index].width);
            place[1].rect[index].y = ((float)rand() / RAND_MAX) * (1.0f - place[1].rect[index].height);
            place[1].rect[index].r = (float)rand() / RAND_MAX;
            place[1].rect[index].g = (float)rand() / RAND_MAX;
            place[1].rect[index].b = (float)rand() / RAND_MAX;

            place[1].square++;
        }
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        int index = place[2].square;
        if (index < 5)
        {
            place[2].rect[index].isalive = true;
            place[2].rect[index].isselect = false;
            place[2].rect[index].width = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;
            place[2].rect[index].height = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;

            place[2].rect[index].x = -1.0f + ((float)rand() / RAND_MAX) * (1.0f - place[2].rect[index].width);
            place[2].rect[index].y = -1.0f + ((float)rand() / RAND_MAX) * (1.0f - place[2].rect[index].height);
            place[2].rect[index].r = (float)rand() / RAND_MAX;
            place[2].rect[index].g = (float)rand() / RAND_MAX;
            place[2].rect[index].b = (float)rand() / RAND_MAX;

            place[2].square++;
        }
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        int index = place[3].square;
        if (index < 5)
        {
            place[3].rect[index].isalive = true;
            place[3].rect[index].isselect = false;
            place[3].rect[index].width = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;
            place[3].rect[index].height = 0.1f + ((float)rand() / RAND_MAX) * 0.3f;

            place[3].rect[index].x = ((float)rand() / RAND_MAX) * (1.0f - place[3].rect[index].width);
            place[3].rect[index].y = -1.0f + ((float)rand() / RAND_MAX) * (1.0f - place[3].rect[index].height);
            place[3].rect[index].r = (float)rand() / RAND_MAX;
            place[3].rect[index].g = (float)rand() / RAND_MAX;
            place[3].rect[index].b = (float)rand() / RAND_MAX;

            place[3].square++;
        }
    }
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS && (mods & GLFW_MOD_SHIFT))
    {
        for (int i = 0;i<4; i++)
        {
            for (int j = 0; j < place[i].square; j++)
            {
                if (place[i].rect[j].isselect == true)
                {
                    if (i == 0)
                    {
                        if (place[i].rect[j].x + place[i].rect[j].width+0.05f <=1.0f)
                            place[i].rect[j].width += 0.05f;
                        if (place[i].rect[j].y + place[i].rect[j].height+0.05f <= 1.0f)
                            place[i].rect[j].height += 0.05f;
                    }
                    else if (i == 1)
                    {
                        if (place[i].rect[j].x + place[i].rect[j].width + 0.05f <= 0.0f)
                            place[i].rect[j].width += 0.05f;
                        if (place[i].rect[j].y + place[i].rect[j].height + 0.05f <= 1.0f)
                            place[i].rect[j].height += 0.05f;
                    }
                    else if (i == 2)
                    {
                        if (place[i].rect[j].x + place[i].rect[j].width + 0.05f <= 0.0f)
                            place[i].rect[j].width += 0.05f;
                        if (place[i].rect[j].y + place[i].rect[j].height + 0.05f <= 0.0f)
                            place[i].rect[j].height += 0.05f;
                    }
                    else if (i == 3)
                    {
                        if (place[i].rect[j].x + place[i].rect[j].width + 0.05f <= 1.0f)
                            place[i].rect[j].width += 0.05f;
                        if (place[i].rect[j].y + place[i].rect[j].height + 0.05f <= 0.0f)
                            place[i].rect[j].height += 0.05f;
                    }
                }
            }
        }
    }
    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < place[i].square; j++)
            {
                if (place[i].rect[j].isselect == true)
                {
                    if (place[i].rect[j].width - 0.05f >= 0.0f)
                        place[i].rect[j].width -= 0.05f;
                    if (place[i].rect[j].height - 0.05f >= 0.0f)
                        place[i].rect[j].height -= 0.05f;
                }
            }
        }
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < place[i].square; j++)
            {
                if (place[i].rect[j].isselect == true)
                {
                    place[i].rect[j].r = (float)rand() / RAND_MAX;
                    place[i].rect[j].g = (float)rand() / RAND_MAX;
                    place[i].rect[j].b = (float)rand() / RAND_MAX;
                }
            }
        }
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < place[i].square; j++)
            {
                place[i].rect[j].isalive = false;
                place[i].square = 0;
            }
            sector[i].r = (float)rand() / RAND_MAX;
            sector[i].g = (float)rand() / RAND_MAX;
            sector[i].b = (float)rand() / RAND_MAX;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &x, &y);

        x = x / 750.0 - 1.0;
        y = 1.0 - y / 450;
        click = true;
    }
}
