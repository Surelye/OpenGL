// GLEW нужно подключать до GLFW.
// GLEW
#include <iostream>
#include <cmath>
#include <SOIL/SOIL.h>

// Делаем свой класс шейдера
// Пользуемся директивами ifndef и define, чтобы избежать рекурсивного выполнения директив include

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h> // Подключаем glew для того, чтобы получить все необходимые заголовочные файлы

class Shader
{
	public:
	// Идентификатор программы
	GLuint Program;
	// Конструктор считывает и собирает шейдер
	// Shader(const GLchar * vertexPath, const GLchar * fragmentPath);
	// Использование программы
	// void Use();

	// Считывание файла шейдера. Для считывания используем стандартные потоки C++, помещая
	// результат в строки.
	Shader(const GLchar * vertexPath, const GLchar * fragmentPath)
	{
		// 1. Получаем исходный код шейдера из filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		// Удостоверимся, что ifstream объекты могут выкидывать исключения
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);

		try
		{
			// Открываем файлы
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// Считываем данные в потоки
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// Закрываем файлы
			vShaderFile.close();
			fShaderFile.close();
			// Преобразовываем потоки в массив GLchar
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar * vShaderCode = vertexCode.c_str();
		const GLchar * fShaderCode = fragmentCode.c_str();

		// Теперь нужно скомпилировать и слинковать шейдер.
		// 2. Сборка шейдеров
		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];

		// Вершинный шейдер
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// Если есть ошибки, то вывести их
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// Шейдерная программа 
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);
		
		// Если есть ошибки, то вывести их
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void Use() { glUseProgram(this->Program); }
};

#endif

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

// Формы - это удобный способ обмена данными между шейдером и программой. Но что, если необходимо 
// задать цвет для каждой вершины? Для этого мы должны объявить столько форм, сколько имеется вершин.
// Наиболее удачным решением будет использование атрибутов вершин.
// Необходимо добавить информацию о цветах к данным вершин. Для этого создадим вектор из трёх
// элементов float. Назначим красную, зелёную и синюю компоненты цветы каждой из вершин.

GLfloat vertices[] = {
	// Позиции	       		// Цвета			 	// Текстурные координаты
	0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f, // Верхний правый угол
	0.5f, -0.5f, 0.0f, 		0.0f, 1.0f, 0.0f, 		1.0f, 0.0f, // Нижний правый угол
   -0.5f, -0.5f, 0.0f, 		0.0f, 0.0f, 1.0f, 		0.0f, 0.0f, // Нижний левый угол
   -0.5f,  0.5f, 0.0f, 		1.0f, 1.0f, 0.0f,		0.0f, 1.0f  // Верхний левый угол
};
// Теперь у нас много информации для передачи её вершинному шейдеру. Необходимо отредактировать
// шейдер так, чтобы он получал и вершины и цвета.
// После добавления дополнительных атрибутов необходимо вновь оповестить OpenGL о новом формате данных

GLuint indices[] = {
	0, 1, 3, // Первый треугольник
	1, 2, 3  // Второй треугольник
};
// Дальнейшая инициализация и работа происходит на строке 278

// Для того, чтобы привязать текстуру к треугольнику, мы должны сообщить каждой вершине треугольника
// , какой части текстуры принадлежит эта вершина, поэтому каждая вершина должна иметь
// текстурные координаты, ассоциированные с частью текстуры. Текстурные координаты находятся 
// в промежутке между 0 и 1 по x и по y. Текстурные координаты начинаются в (0, 0) в нижнем левом
// углу текстуры и заканчиваются в (1, 1) в верхнем правом углу. Для прямоугольника текстурные
// координаты будут выглядеть так:
// GLfloat texCoords[] = { Будем хранить текстурные координаты в массиве вершинных данных
//	1.0f, 1.0f, // верхний правый угол
//	1.0f, 0.0f, // нижний правый 
//	0.0f, 0.0f, // нижний левый
//	0.0f, 1.0f // верхний левый
// };
// Получение цвета текстуры с помощью текстурных координат называется отбором (sampling).
// Сэмплинг текстуры может быть выполнен различными методами, поэтому необходимо сообщить 
// OpenGL, как он должен проводить сэмплинг. 
// TEXTURE WRAPPING
// Текстурные координаты находятся в промежутке между (0, 0) и (1, 1). Но возможна ситуация, когда 
// текстурные координаты выходят за промежуток. По умолчанию OpenGL будет повторять изображение, 
// но возможно и другое поведение:
// GL_REPEAT - стандартное поведение для текстур - повторение текстуры
// GL_MIRRORED_REPEAT - похоже на GL_REPEAT, только в этом режиме изображение отражается
// GL_CLAMP_TP_EDGE - привязывает координаты между 0 и 1. В результате выходящие за пределы координаты
// будут привязаны к границе текстуры.
// GL_CLAMP_TO_BORDER - координаты, выходящие за пределы диапазона, будут давать установленный
// пользователем цвет границы
// Каждую из вышеприведённых опций можно установить на осях (s, t, r (в случае 3D текстур)), которые
// эквивалентны x, y и z с помощью функций **glTexParameter***:
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
// Первый аргумент определяет цель, к которой привязана наша текстура, т.к. работаем с 2D, то 
// значение будет GL_TEXTURE_2D. Второе значение требуется для того, чтобы сообщить OpenGL, какой 
// параметр необходимо установить - опция WRAP для осей S или T. В последнем аргументе передаётся
// выбранный метод wrapping. Если был бы выбран метод GL_CLAMP_TO_BORDER, то нам бы ещё пришлось
// указать цвет границ. Это делается при помощи fv-альтернативы glTexParameter с передачей в неё
// GL_TEXTURE_BORDER_COLOR в качестве опции и массива из чисел с плавающей точкой в качестве 
// цветового значения:
// float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

// ФИЛЬТРОВАНИЕ ТЕКСТУР
// Текстурные координаты не зависят от разрешения, но при этом могут принимать любые значения с 
// плавающей точкой, поэтому OpenGL требуется понять, какой пиксель текстуры (также называемый 
// текселем) ему требуется наложить. В OpenGL есть опция для фильтрования текстур. Наиболее важные
// из них - это GL_NEAREST и GL_LINEAR.
// GL_NEAREST (также называемый фильтр ближайшего соседа) - стандартный метод фильтрования в OpenGL.
// Пока он установлен OpenGL будет выбирать пиксель, который находится ближе всего к текстурной координате.

// GL_LINEAR (также называемый (би)линейной фильтрацией): принимает интерполированное значение от
// ближайших к текстурной координате текселей. Чем ближе тексель к текстурной координате, тем больше
// множитель цвета этого текселя.

// У OpenGL есть сложности с получением правильного цвета для фрагмента с текстурой высокого 
// разрешения, когда приходится учитывать большое количество пикселей текстуры. Такое поведение будет
// генерировать артефакты на маленьких объектах и чрезмерно вызывать чрезмерную трату памяти,
// связанную с использованием текстур высокого разрешения на маленьких объектах.
// Для решения этой проблемы OpenGL использует технологию, называемую мипмапами, которая 
// предусматривает набор изображений-текстур, в котором каждая последующая текстура вдвое меньше
// предыдущей. После определённого расстояния OpenGL будет использовать другую мипмап-текстуру,
// которая лучше подходит для текущего расстояния. Чем дальше от наблюдателя находится объект, тем 
// меньше будет использоваться текстура. Каждое изображение требует создание своего набора
// мипмап-текстур, для этого OpenGL умеет генерировать их с помощью вызова glGenerateMipmaps. 
// Также как возможно использование фильтрации на текстурах, можно использовать фильтрацию на
// различных уровнях мипмапов с помощью NEAREST и LINEAR фильтрации для переключения между уровнями.
// Также как и с фильтрацией текстур мы можем установить метод фильтрации с помощью функции
// glTexParameteri: 
// glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
// glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// ЗАГРУЗКА И СОЗДАНИЕ ТЕКСТУР
// Перед тем как начать использовать текстуры их необходимо загрузить в приложение. Для загрузки
// изображений будем пользоваться готовой библиотекой SOIL.

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

// Обобщённая структура шейдера
// #version version_number

// in type in_variable_name; -- набор входных значений
// in type in_variable_name;

// out type out_variable_name; -- выходная переменная

// uniform type uniform_name; -- глобальная переменная 

// void main() -- функция main является начальной точкой шейдера
// {
//   // Что-то делаем, вычисляем, экспериментируем, и т.п.
//   ...
//   // Присваиваем результат работы шейдера выходной переменной
//   out_variable_name = weird_stuff_we_processed;
// }

// Изменим шейдеры так, чтобы вершинный шейдер предоставлял цвет для фрагментного шейдера:
const char * vertexShaderSource =

"#version 330 core													\n"
"																	\n"
"layout (location = 0) in vec3 position;							\n" // Устанавливаем позицию переменной с координатами в 0
"layout (location = 1) in vec3 color;								\n" // А позицию переменной с цветом в 1
"																	\n"
"out vec3 ourColor;													\n" // Передаём цвет во фрагментный шейдер
"																	\n"
"void main()														\n"
"{																	\n"
"	gl_Position = vec4(position, 1.0);								\n" // Напрямую передаём vec3 в vec4
"	ourColor = color;												\n" // Устанавливаем значение цвета, полученное от вершинных данных
"}																	\0";

const char * coloredVerticesFragmentShaderSource = 

"#version 330 core													\n"
"in vec3 ourColor;													\n"
"out vec4 color;													\n" 
"																	\n"
"void main()														\n"
"{																	\n"
"	color = vec4(ourColor, 1.0f);									\n"
"}																	\0";

const char * fragmentShaderSource =

"#version 330 core													\n"
"in vec4 vertexColor;												\n" // Входная переменная из вершинного шейдера (имеет то же название и тот же тип)
"																	\n"
"out vec4 color;													\n"
"																	\n"
"void main()														\n"
"{																	\n"
"	color = vertexColor;											\n"
"}																	\0";
// В этом примере мы объявили выходной вектор из 4-х элементов vertexColor в вершинном шейдере и 
// идентичный вектор с названием vertexColor, но только как входной во фрагментном шейдере. В 
// результате выходной vertexColor из вершинного шейдера и входной vertexColor из фрагментного 
// шейдера были соединены.
// Но также есть способы передачи информации шейдеру из приложения - Uniforms.

// Uniforms (формы) - это ещё один способ передачи информации от приложения, работающего на CPU, к
// шейдеру, работающему на GPU. Формы являются глобальными. Глобальная для GLSL переменная означает 
// следующее: глобальная переменная будет уникальной для каждой шейдерной программы, и доступ к ней
// есть у каждого шейдера на любом этапе в этой программе, а также значение формы сохраняется до 
// тех пор, пока оно не будет сброшено или обновлено. Для объявления формы в GLSL используется 
// спецификатор переменной uniform. После объявления формы её можно использовать в шейдере. 
// Установить цвет треугольника с использованием формы можно следующим образом: 

const char * uniformFragmentShaderSource = 

"#version 330 core													\n"
"out vec4 color;													\n" 
"																	\n"
"uniform vec4 ourColor;												\n" // Значение этой переменной устанавливается в коде OpenGL
"																	\n"
"void main()														\n"
"{																	\n"
"	color = ourColor;												\n"
"}																	\0";
// Мы объявили переменную формы ourColor в фрагментном шейдере и используем её для установки 
// выходного значения фрагментного шейдера. Т.к. форма является глобальной переменной, то её 
// объявление можно производить в любом шейдере, а это значит, что не нужно передавать что-то
// из вершинного шейдера во фрагментный, т.е. нет необходимости объявлять форму в вершинном шейдере,
// т.к. она там не используется. Дальнейшее заполнение формы данными и её использование происходит
// на строке 373 и дальше. 

const GLuint WIDTH = 800, HEIGHT = 600;

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
		
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
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

	// Узнать максимальное количество входных переменных-вершин, передаваемых в шейдер
	GLint nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << '\n';

	// Шейдеры программируются на C-подобном языке GLSL. Он адаптирован для использования в графике
	// и предоставляет функции для работы с векторами и матрицами. Как и любой другой язык 
	// программирования, GLSL предоставляет перечень примитивных типов данных. Также GLSL 
	// предоставляет два типа-контейнера: vector и matrix.
	// vector в GLSL - это контейнер, содержащий от 1 до 4 значений любого примитивного типа.
	// Объявление выглядит следующим образом: ''/b/i/u/dvecn (например, vec4 или ivec4). 
	// vecn - это стандартный вектор, содержащий в себе n float значений.
	// При обращении из вектора к данным через точку можно получить не только одно значение, но и
	// целый вектор, используя синтаксис, который называется swizzling:
	//
	// vec2 someVec;
	// vec4 anotherVec = someVec.xyxx;
	// vec3 differentVec = anotherVec.zyw;
	// vec4 otherVec = someVec.xxxx + anotherVec.wzyx;
	//
	// Поскольку шейдеры представляют собой небольшие программы, в GLSL есть in и out переменные,
	// позволяющие получить данные для обработки и передать результаты вызывающей стороне.
	// Так, например, вершинный шейдер принимает входные значения напрямую из вершинных данных.
	// Для того, чтобы указать OpenGL, как организованы аргументы, мы используем метаданные позиции,
	// для того, чтобы мы могли настраивать атрибуты на CPU (ранее использованный 'layout (location = 0))'.
	// Исключением является то, что фрагментный шейдер должен иметь на выходе vec4, т.е. представить
	// в виде результата цвет в формате RGBA. Если этого не будет сделано, то объект будет отрисован
	// чёрным или белым.
	// Таким образом, если стоит задача передачи информации от одного шейдера к другому, то необходимо
	// определить в передающем шейдере out переменную такого типа, как и у in переменной в 
	// принимающем шейдере. Если типа и имена переменных будут одинаковы с обеих сторон, то OpenGL
	// соединит эти переменные вместе, что даст нам возможность обмена информации между шейдерами.

	// После определения вершинных данных требуется передать их в первый этап графического
	// конвейера: в вершинный шейдер. Это делается следующим образом: выделяем памяти на GPU, 
	// куда мы сохраним наши вершинные данные, укажем OpenGL, как он должен интерпретировать 
	// переданные ему данные и передадим GPU количество переданных нами данных. Затем
	// вершинный шейдер обработает такое количество вершин, которое мы ему сообщили.
	// Управление памятью, зарезервированной под вершины, происходит пр	и помощи объекта VBO
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
	glShaderSource(fragmentShader, 1, &coloredVerticesFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::UNIFORM_SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
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
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
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
	// glEnableVertexAttribArray(0); // - Передача вершинному атрибуту позицию аргумента

	// ЗАТЕМ ИДЁТ АТРИБУТ С ЦВЕТОМ
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	// glEnableVertexAttribArray(1);
	// Мы используем вершинный атрибут с позицией 1. Цвет состоит из трёх float значений, и нам не
	// нужна нормализация. Т.к. мы используем два атрибута шейдеров, то надо пересчитать шаг.
	// Для доступа к следующему атрибуту шейдера нужно переместиться на 6 элементов вправо, на 3
	// для вектора вершин и на 3 для вектора цвета.

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
	// 2. Копируем наш массив вершин в буфер для OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. Копируем индексы в буфер для OpenGL
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 3. Устанавливаем указатели на вершинные атрибуты
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Установка указателей на атрибуты цвета
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// После добавления дополнительных атрибутов (текстурные координаты) необходимо оповестить OpenGL
	// о новом формате данных (также скорректировав значение шага прошлых двух атрибутов):
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// Сначала необходимо создать все указатели на новые элементы в массиве, а только затем 
	// отвязать VAO.

	// 4. Отвязываем VAO
	glBindVertexArray(0);
	// Затем код продолжается в игровом цикле.	

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Отрисовка контуров фигур без заполнения области

	// На данный момент форма не содержит в себе полезных данных, т.к. мы их туда не поместили.
	// Для того, чтобы заполнить форму данными, нужно узнать индекс нужного нам атрибута формы в 
	// шейдере. Получив значение индекса атрибута можно вместить туда необходимые данные. Для 
	// демонстрации работы этой функции будет менять цвет от времени (реализация в игровом цикле).

	Shader ourShader("/home/surelye/Desktop/repos/OpenGL/vertex_shader.vs", 
					 "/home/surelye/Desktop/repos/OpenGL/fragment_shader.frag");

	// Также как и на любой другой объект в OpenGL, на текстуры ссылаются идентификаторы. 
	GLuint containerTexture, faceTexture;
	glGenTextures(1, &containerTexture);
	// Функция glGenTextures принимает в качестве первого аргумента количество текстур для генерации
	// , а в качестве второго аргумента - массив GLuint, в котором будут храниться идентификаторы
	// этих текстур. Также как любой другой объект мы привяжем его для того, чтобы функции, 
	// использующие текстуры, знали, какую текстуру использовать.

	// ТЕКСТУРНЫЙ БЛОК
	// Почему же sampler2D переменная является uniform, если мы ей так и не присвоили никакое значение
	// с помощью glUniform? С помощью glUniform1i мы можем присвоить значение местоположения текстурному
	// сэмплеру для возможности использования нескольких текстур в одном фрагментном шейдере. 
	// Местоположение текстуры чаще называется текстурным блоком. Текстурный блок по умолчанию - 0, 
	// который означает активный текстурный блок. 
	// Основная цель текстурных блоков - это обеспечение возможности использования более чем 1 текстуры
	// в шейдере. Передавая текстурные блоки сэмплеру, мы можем привязывать несколько текстур за один 
	// раз до тех пор, пока мы активируем соотносящиеся текстурные блоки. Также как и glBindTexure, 
	// мы можем активировать текстуры с помощью glActivateTexture, передавая туда текстурный блок,
	// который мы хотим использовать.
	glActiveTexture(GL_TEXTURE0); // Активируем текстурный блок перед привязкой текстуры
	// OpenGL поддерживает как минимум 16 текстурных блоков, которые можно получить через 
	// GL_TEXTURE0 - GL_TEXTURE15. Они объявлены по порядку, поэтому обращаться к ним можно 
	// следующим образом: GL_TEXTURE8 = GL_TEXTURE0 + 8. Для принятия другого сэмплера требуется
	// изменить фрагментный шейдер.

	// Для того, чтобы использовать вторую текстуру, надо добавить привязку текстур к 
	// соответствующим текстурным блокам и указанием того, к каком сэмплеру относится каждый текстурный блок.
	glBindTexture(GL_TEXTURE_2D, containerTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Для загрузки изображения через SOIL будем использовать функцию SOIL_load_image:
	int picWidth, picHeight;
	unsigned char * image = SOIL_load_image("/home/surelye/Desktop/repos/OpenGL/pics/container.jpg", &picWidth, &picHeight, 0, SOIL_LOAD_RGB);
	// Первый аргумент - это местоположение файла, второй и третий - это размеры изображения, они 
	// понадобятся для генерации текстуры. Четвёртый аргумент - это количество каналов изображения.
	// Последний аргумент сообщает SOIL, как ему загружать изображение: нам нужна только RGB информация.
	// Результат будет храниться в массиве байтов.	

	// После привязки текстуры мы можем начать генерировать данные текстуры, используя предварительно
	// загруженное изображение. Текстуры генерируются с помощью glTexImage2D:
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, picWidth, picHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Первый аргумент описывает текстурную цель. Установив значение GL_TEXTURE_2D мы сообщили
	// функции, что наша текстура привязана к этой цели (чтобы другие цели GL_TEXTURE_1D и 
	// GL_TEXTURE_3D не будут задействованы).
	// Второй аргумент описывает уровень мипмапа, для которого мы хотим сгенерировать текстуру, в 
	// случае, если мы хотим самостоятельно сгенерировать мипмапы. Поскольку мипмапы будет генерировать
	// сам OpenGL, то передаём 0.
	// Третий аргумент сообщает OpenGL, в каком формате мы хотим хранить текстуру (наше изображение
	// имеет только RGB значения).
	// Четвёртый и пятый аргументы задают ширину и высоту результирующей текстуры. 
	// Шестой аргумент всегда должен быть 0 (аргумент устарел) (?).
	// Седьмой и восьмой аргументы описывают формат и тип данных исходного изображения. Мы загружали
	// RGB значения и хранили их в байтах (char), так что мы передаём эти значения.
	// Последний аргумент - это сами данные изображения.
	// После вызова glTexImage2D текущая привязанная текстура будет иметь привязанное к ней 
	// изображение. Текстура будет иметь только базовое изображение, и если мы захотим использовать
	// мипмапы, то нам придётся таким же образом задавать изображение инкрементируя значения уровня
	// мипмапов. Или возможен вызов glGenerateMipmap после генерации текстуры. Эта функция 
	// автоматически сгенерирует все требуемые мипмапы для текущей привязанной текстуры. 

	// После окончания генерации текстуры и мипмапов освободим участок памяти, выделенный под 
	// загруженное изображение и отвяжем объект текстуры.
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &faceTexture);
	glBindTexture(GL_TEXTURE_2D, faceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("/home/surelye/Desktop/repos/OpenGL/pics/awesomeface.png", &picWidth, &picHeight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, picWidth, picHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Далее идёт модификация вершинного и фрагментного шейдера, а затем: 
	// Осталось только привязать текстуру перед вызовом glDrawElements в игровом цикле, и она 
	// автоматически будет передана сэмплеру фрагментного шейдера.

	// Игровой цикл.
	while (!glfwWindowShouldClose(window))
	{
		// Проверяем события и вызываем функции обратного вызова.
		glfwPollEvents();

		// Ниже будут располагаться команды отрисовки.
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Активируем шейдерную программу
		// glUseProgram(shaderProgram);
		ourShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTexture);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, faceTexture);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);
		// glUniform1i используется для того, чтобы установить позицию текстурного блока в uniform
		// sampler. Устанавливая их через glUniform1i мы будем уверены, что uniform sampler 
		// соотносится с правильным текстурным блоком. 

		// Обновляем цвет формы
		// GLfloat timeValue = glfwGetTime();
		// GLfloat greenValue = (tan(timeValue) / 2) + 0.5;
		// GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		// glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		// Т.к. OpenGL написан на C, в котором нет перегрузки функций, для каждого типа данных
		// определены свои функции, определяемые постфиксом.

		// Рисуем фигуру 
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glDrawElements берёт индексы из текуuniformFragmentShaderSourceщего привязанного к GL_ELEMENT_ARRAY_BUFFER EBO
		// Это означает, что мы должны каждый раз привязывать различные EBO. Но VAO умеет 
		// хранить и EBO. 
		glBindVertexArray(0);

		// Меняем буферы местами.
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	
	glfwTerminate();
	return (EXIT_SUCCESS);
}

 