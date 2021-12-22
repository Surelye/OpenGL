// GLEW нужно подключать до GLFW.
// GLEW
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

// Массив вершин в в нормализованном виде:
// GLfloat vertices[] = {
// 	-0.5f, -0.5f, 0.0f,
// 	 0.5f, -0.5f, 0.0f,
// 	 0.0f,  0.5f, 0.0f
// };

// Допустим, что мы хотим отрисовать прямоугольник. Мы можем сформировать прямоугольник при 
// помощи двух треугольников. Но в таком случае произойдёт наложение вершин, что является 
// нерациональным использованием ресурсов. Такую ситуацию можно обойти, используя EBO 
// (ELEMENT BUFFER OBJECTS) или иначе IBO (INDEX BUFFER OBJECT). Эта структура хранит
// индексы, которые OpenGL будет использовать для решения того, какую вершину отрисовывать. Это
// называется отрисовка по индексам. Отрисуем только уникальные вершины и индексы для их 
// отрисовки как треугольников.

GLfloat vertices[] = {
	0.5f,  0.5f, 0.0f, // Верхний правый угол
	0.5f, -0.5f, 0.0f, // Нижний правый угол
   -0.5f, -0.5f, 0.0f, // Нижний левый угол
   -0.5f,  0.5f, 0.0f  // Верхний левый угол
};

GLuint indices[] = {
	0, 1, 3, // Первый треугольник
	1, 2, 3  // Второй треугольник
};
// Дальнейшая инициализация и работа происходит на строке 278

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

"#version 330 core													\n"
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

	// Процесс сборки фрагментного шейдера аналогичен сборке вершинного
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Создание шейдерной программы (объект, являющийся результатом комбинации 
	// нескольких шейдеров)
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	// Присоединение собранных шейдеров к программе и их связка с помощью функции glLinkProgram
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Также как и со сборкой шейдера, можно получить успешность связывания и сообщение об ошибке
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Каждый вызов шейдера и отрисовочных функций будет использовать наш объект программы
	// и, соответственно, наши шейдеры. Также после связывания наши шейдеры нам больше не 
	// понадобятся, поэтому их можно удалить:
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Вершинный шейдер позволяет указать любые данные в каждый атрибут вершины, но это 
	// не значит, что нам придётся указывать, какой элемент данных относится к какому атрибуту.
	// Это означает, что мы должны сообщить, как OpenGL должен интерпретировать вершинные
	// данные перед отрисовкой. Формат нашего вершинного буфера следующий:
	// 1) Информация о позиции хранится в 32-битном значении с плавающей точкой
	// 2) Каждая позиция формируется из 3-х значений
	// 3) Не существует никакого разделителя между наборами из 3-х значений. Такой буфер
	// называется плотно упакованным.
	// 4) Первое значение в переданных данных - это начало буфера
	// Зная эти особенности, мы можем сообщить OpenGL, как он должен интерпретировать
	// вершинные данные. Это делается с помощью функции glVertexAttribPointer:
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	// Первый аргумент описывает, какой аргумент шейдера мы хотим настроить. Мы хотим
	// специфицировать значение аргумента position, позиция которого была указана следующим 
	// образом: layout (location = 0)
	// Следующий аргумент описывает размер аргумента в шейдере. Поскольку мы использовали 
	// vec3, то мы указываем 3.
	// Третий аргумент описывает используемый тип данных. Мы указываем GL_FLOAT, поскольку
	// vec в шейдере использует числа с плавающей точкой.
	// Четвёртый аргумент указывает необходимость нормализовать входные данные. Если мы укажем
	// GL_TRUE, то все данные будут расположены между 0 (-1 для знаковых значений) и 1.
	// В нашем случае нормализация не требуется, поэтому выставляем GL_FALSE.
	// Пятый аргумент называется шагом и описывает расстояние между наборами данных.
	// Мы также можем указать шаг равный 0, и тогда OpenGL высчитает шаг (работает только с 
	// плотно упакованными наборами данных).
	// Последний параметр имеет тип GLvoid*, и поэтому требует приведения типов. Это
	// смещение начала данных в буфере. В нашем случае буфер не имеет смещения.
	glEnableVertexAttribArray(0); // - Передача вершинному атрибуту позицию аргумента

	// Отрисовка объекта будет выглядеть вот так:
	// 0. Копируем массив с вершинами в буфер OpenGL
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Затем установим указатели на вершинные атрибуты
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	// 2. Используем нашу шейдерную программу
	// glUseProgram(shaderProgram);
	// 3. Теперь отрисовываем объект
	// someOpenGLFunctionThatDrawsOutTriangle();
	// Этот процесс должен повторяться при каждой отрисовке объекта. Но существует
	// иной способ хранения всех состояний - vertex array object.
	// Объект вершинного массива может быть также привязан как и VBO, и после этого
	// все последующие вызовы вершинных атрибутов будут храниться в VAO. Преимущество
	// этого метода в том, что нам требуется настроить атрибуты лишь единожды, а все 
	// последующие разы будет использована конфигурация VAO. Также такой метод
	// упрощает смену вершинных данных и конфигураций атрибутов простым привязыванием различных VAO.
	// Core OpenGL требует, чтобы мы использовали VAO для того, чтобы OpenGL знал,
	// как работать с нашими входными вершинами. Если мы не укажем VAO, OpenGL может
	// отказаться отрисовывать что-либо.
	// VAO хранит следующие вызовы: 
	// 1. Вызовы glEnableVertexAttribArray или glDisableVertexAttribArray
	// 2. Конфигурацию атрибутов, выполненную через glVertexAttribPointer
	// 3. VBO, ассоциированные с вершинными атрибутами с помощью glVertexAttribPointer

	// Процесс генерации VAO:
	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	GLuint EBO; // Создание EBO
	glGenBuffers(1, &EBO); // Генерация буфера, аналогично VBO

	// Для того, чтобы использовать VAO необходимо привязать VAO с помощью glBindVertexArray.
	// Теперь мы должны настроить/привязать требуемые VBO и указатели на атрибуты, а
	// в конце отвязать VAO для последующего использования. И теперь, каждый раз, когда 
	// мы хотим отрисовать объект, мы привязываем VAO с требуемыми нам настройками перед 
	// отрисовкой объекта. Это должно выглядеть следующим образом:
	// 1. Привязываем VAO
	glBindVertexArray(VAO);
	// 2. Копируем нам массив вершин в буфер для OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. Копируем индексы в буфер для OpenGL
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 3. Устанавливаем указатели на вершинные атрибуты
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 4. Отвязываем VAO
	glBindVertexArray(0);
	// Затем код продолжается в игровом цикле.	

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Отрисовка контуров фигур без заполнения области

	// Игровой цикл.
	while (!glfwWindowShouldClose(window))
	{
		// Проверяем события и вызываем функции обратного вызова.
		glfwPollEvents();

		// Ниже будут располагаться команды отрисовки.
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Вырисовка треугольника
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glDrawElements берёт индексы из текущего привязанного к GL_ELEMENT_ARRAY_BUFFER EBO
		// Это означает, что мы должны каждый раз привязывать различные EBO. Но VAO умеет 
		// хранить и EBO. 
		glBindVertexArray(0);

		// Меняем буферы местами.
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}

 