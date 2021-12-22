// 1. Отрисовать два треугольника один за другим с помощью
// glDrawArrays (с добавлением большего числа вершин)
// 2. Создать два треугольника с использованием разных VAO и VBO
// 3. Создать второй фрагментный шейдер вывода жёлтого цвета.
// Второй треугольник должен быть жёлтого цвета. 

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::cout;

GLfloat firstVertices[] = {
    0.0f,  0.5f, 0.0f,
   -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
   -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, -1.0f, 0.0f
};

GLfloat secondVertices[] = {
   -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, -1.0f, 0.0f
};

const char * vertexShaderSource =

"#version 330 core													\n"
"																	\n"
"layout (location = 0) in vec3 position;							\n"
"																	\n"
"void main()														\n"
"{																	\n"
"	gl_Position = vec4(position.x, position.y, position.z, 1.0);	\n"
"}																	\0";

const char * fragmentShaderSource =

"#version 330 core													\n"
"																	\n"
"out vec4 color;													\n"
"																	\n"
"void main()														\n"
"{																	\n"
"	color = vec4(1.0f, 0.5f, 0.2f, 1.0f);							\n"
"}																	\0";

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode);

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char * argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow * window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        cout << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialize GLEW.\n";
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, key_callback);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstVertices), firstVertices, GL_STATIC_DRAW);

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << '\n';
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glfwSwapBuffers(window);
    }    

    // Фрагментный шейдер - это второй и последний шейдер ...
}