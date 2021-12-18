// GLEW нужно подключать до GLFW.
// GLEW
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

// Массив вершин в в нормализованном виде:
GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
	// и после этого приложение закрывается.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

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

"version 330 core													\n"
"																	\n"
"out vec4 color;													\n"
"																	\n"
"void main()														\n"
"{																	\n"
"	color = vec4(1.0f, 0.5f, 0.2f, 1.0f);							\n"
"}																	\0";

int main()
{
	// Инициализация GLFW
	glfwInit();
	// Настройка GLFW
	// Задаётся минимальная требуемая версия OpenGL.
	// Мажорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// Минорная 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Установка профайла, для которого создаётся контекст.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Выключения возможности изменения размера окна.
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	// Зарегистрировать функцию обратного вызова надо после создания окна и до игрового цикла.
	glfwSetKeyCallback(window, key_callback);

	// После определения вершинных данных требуется передать их в первый этап графического
	// конвейера: в вершинный шейдер. Это делается следующим образом: выделяем памяти на GPU, 
	// куда мы сохраним наши вершинные данные, укажем OpenGL, как он должен интерпретировать 
	// переданные ему данные и передадим GPU количество переданных нами данных. Затем
	// вершинный шейдер обработает такое количество вершин, которое мы ему сообщили.
	// Управление памятью, зарезервированной под вершины, происходит при помощи объекта VBO
	// (vertex buffer objects). Т.к. отправка данных с CPU на GPU происходит медленно, то
	// нужно отправлять как можно больше данных за раз. Однако как только данные окажутся в GPU,
	// вершинный шейдер получит их практически мгновенно.
	GLuint VBO;
	glGenBuffers(1, &VBO);

	// У OpenGL есть большое количество различных типов объектов буферов. Тип VBO - 
	// GL_ARRAY_BUFFER. OpenGL позволяет привязывать множество буферов, если у них разные типы. 
	// Мы можем привязать GL_ARRAY_BUFFER к нашему буферу с помощью glBindBuffer:
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// С этого момента любой вызов, использующий буфер, будет работать с VBO. Теперь мы можем 
	// вызвать glBufferData для копирования вершинных данных в этот буфер.
	// glBufferData - это функция, цель которой - копирование данных пользователя в указанный буфер.
	// Первый её аргумент - это тип буфера, в который мы хотим скопировать данные (VBO в данный 
	// момент привязан к GL_ARRAY_BUFFER). Второй аргумент определяет количество данных, которые
	// мы хотим передать буферу. Третий аргумент - сами данные. Четвёртый аргумент определяет 
	// режим работы видеокарты с данными.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Исходный код шейдера написан, но чтобы этим шейдером мог воспользоваться OpenGL,
	// его надо собрать. В начале мы должны создать объект шейдера. Поскольку доступ
	// к созданным объектам осуществляется через идентификатор, то мы будем хранить 
	// его в переменной с типом GLuint, а создавать через glCreateShader.
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Во время создания шейдера, необходимо указать его тип. В нашем случае - вершинный шейдер.

	// Далее мы привязываем исходный код шейдера к объекту шейдера и компилируем его.
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Функция glShaderSource в качестве первого аргумента принимает шейдер, который требуется собрать.
	// Второй аргумент описывает количество строк. Третий параметр - это исходный код шейдера.

	// Проверка на наличие ошибок при сборке шейдера происходит следующим образом:
	GLint success; // число для определения успешности сборки
	GLchar infoLog[512]; // контейнер для хранения ошибок
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // проверка успешности сборки

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Игровой цикл.
	while (!glfwWindowShouldClose(window))
	{
		// Проверяем события и вызываем функции обратного вызова.
		glfwPollEvents();

		// Ниже будут располагаться команды отрисовки.
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Меняем буферы местами.
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

 