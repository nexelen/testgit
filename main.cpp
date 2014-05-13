
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <windows.h>
#include <iostream>
#include <vector>
#define BUFFER_OFFSET(offset) ((void*)(offset))

struct Program
{
	static GLuint Load(const char* vert, const char* geom, const char* frag)
	{
		GLuint prog = glCreateProgram();
		if (vert) AttachShader(prog, GL_VERTEX_SHADER, vert);
		if (geom) AttachShader(prog, GL_GEOMETRY_SHADER, geom);
		if (frag) AttachShader(prog, GL_FRAGMENT_SHADER, frag);
		glLinkProgram(prog);
		CheckStatus(prog);
		return prog;
	}

private:
	static void CheckStatus(GLuint obj)
	{
		GLint status = GL_FALSE, len = 10;
		if (glIsShader(obj))   glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
		if (glIsProgram(obj))  glGetProgramiv(obj, GL_LINK_STATUS, &status);
		if (status == GL_TRUE) return;
		if (glIsShader(obj))   glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
		if (glIsProgram(obj))  glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
		std::vector< char > log(len, 'X');
		if (glIsShader(obj))   glGetShaderInfoLog(obj, len, NULL, &log[0]);
		if (glIsProgram(obj))  glGetProgramInfoLog(obj, len, NULL, &log[0]);
		std::cerr << &log[0] << std::endl;
		exit(-1);
	}

	static void AttachShader(GLuint program, GLenum type, const char* src)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		CheckStatus(shader);
		glAttachShader(program, shader);
		glDeleteShader(shader);
	}
};

//#define GLSL(version, shader) "#version " #version "\n" #shader

const char* vert =
{
	"#version 440 core                                                          \n"
	"in vec4 vPosition;															\n"
	"void main()																\n"
	"{																			\n"
		"gl_Position = vPosition;												\n"
	"}                                                                          \n"
};

const char* frag = {
	"#version 440 core                                                          \n"
	"                                                                           \n"
	"out vec4 color;                                                            \n"
	"                                                                           \n"
	"void main(void)                                                            \n"
	"{                                                                          \n"
	"    color = vec4(sin(gl_FragCoord.x * 0.25) * 0.5 + 0.5,                   \n"
	"                 cos(gl_FragCoord.y * 0.25) * 0.5 + 0.5,                   \n"
	"                 sin(gl_FragCoord.x * 0.15) * cos(gl_FragCoord.y * 0.1),  \n"
	"                 1.0);                                                     \n"
	"}                                                                          \n"
};

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

const GLuint NumVertices = 6;

void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);

	GLfloat  vertices[NumVertices][2] = {
		{ -0.90, -0.90 },  // Triangle 1
		{ 0.85, -0.90 },
		{ -0.90, 0.85 },
		{ 0.90, -0.85 },  // Triangle 2
		{ 0.90, 0.90 },
		{ -0.85, 0.90 }
	};

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint program = Program::Load(vert, NULL, frag);
	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);

	glClearColor(0.0, 0.3, 0.3, 1);
}

void Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd )
{
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("SDL2 OpenGL4.4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetSwapInterval(1);
	context = SDL_GL_CreateContext(window);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
		exit(EXIT_FAILURE);
	init();
	std::cout <<"OPENGL VERSION: "<< (const char*)glGetString(GL_VERSION) << std::endl;
	SDL_Event _event;
	bool running = true;
	while (running)
	{
		while (SDL_PollEvent(&_event))
		{
			if (_event.type == SDL_QUIT){ running = false; }
		}
		Render();
		SDL_GL_SwapWindow(window);
	}
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}