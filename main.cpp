
#include <SDL.h>
#include <gl\glew.h>
//#include <windows.h>
#include <iostream>
#include "GLShader.h"
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <time.h>

int screenWidth = 800;
int screenHeight = 600;

class Tree {
private:
	std::vector<GLfloat> xPositions;
	std::vector<GLfloat> yPositions;
	std::vector<GLfloat> angles;
	GLfloat treeRed;
	GLfloat treeGreen;
public:

	std::vector<GLfloat> sizes;
	std::vector<glm::vec4> vertex;
	std::vector<unsigned int> index;
	std::vector<glm::vec3> color;

	Tree() {
		generateFractalTree();
		genVertTree(2500);
		genIndexTree();
	}

	void genVertTree(int lines) {
		if (lines > xPositions.size())
			return;

		for (size_t i = 0; i < lines; i++) {
			//glColor3f(1.0f / (sizes.at(i) * treeRed), 1.0f / (sizes.at(i) * treeGreen), 0.0f);
			glm::vec3 colorp = glm::vec3(1.0f / (sizes.at(i) * treeRed), 1.0f / (sizes.at(i) * treeGreen), 0.0f);
			color.push_back(colorp);
			color.push_back(colorp);
			//glLineWidth(sizes.at(i) / 40.0f + 1.0f);
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(xPositions.at(i), yPositions.at(i), 0.0f));
			model=glm::rotate(model, angles.at(i), glm::vec3(0.0f, 0.0f, 1.0f));
			vertex.push_back(model*glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			vertex.push_back(model*glm::vec4(0.0f, -sizes.at(i), 0.0f, 1.0f));
		}
	}

	void genIndexTree()
	{
		unsigned int ind = 0;
		for (std::vector<glm::vec4>::iterator i = vertex.begin(); i != vertex.end(); ++i)
		{			
			//index.push_back(0);
			index.push_back(ind++);
			//std::cout << "Print index: " << ind << std::endl;
		}
	}

	void generateFractalTree() {
		xPositions.clear();
		yPositions.clear();
		sizes.clear();
		angles.clear();

		srand(time(0));
		int randVarRed = rand() % 3;
		if (randVarRed == 0)
			treeRed = 0.35f;
		else if (randVarRed == 1)
			treeRed = 0.25f;
		else if (randVarRed == 2)
			treeRed = 0.15f;

		int randVarGreen = rand() % 2;
		if (randVarGreen == 0)
			treeGreen = 0.15f;
		else if (randVarGreen == 1)
			treeGreen = 0.30f;
		generateFractalTree(screenWidth / 2.0f, screenHeight - 20.0f, (rand() % screenHeight / 4) + 40.0f, 0.0f);
	}

	void generateFractalTree(GLfloat xPos, GLfloat yPos, GLfloat size, GLfloat degrees) {
		if (size < 5.0f || yPos - size < 10.0f)
			return;
		
		static const int maxShrink = 900; // per thousand
		static const int minShrink = 1.0f;
		static const int maxAngle = 45;
		static const int maxBranches = 8;
		static const int minBranches = 0;
		GLfloat newPosX = xPos + size * sin(degrees / 180.0f * M_PI);
		GLfloat newPosY = yPos - size * cos(degrees / 180.0f * M_PI);
		xPositions.push_back(xPos);
		yPositions.push_back(yPos);
		sizes.push_back(size);
		angles.push_back(degrees);

		int branches = rand() % (maxBranches - minBranches) + 1 + minBranches;
		for (int i = 0; i < branches; i++) {
			int x = rand() % 2 == 1 ? -1 : 1;
			generateFractalTree(newPosX, newPosY, size * (rand() % maxShrink + minShrink) / 1000.0f, degrees + x * rand() % maxAngle);
		}
	}

	int getTotalLines() {
		return xPositions.size();
	}
} tree;

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint indexBuff;
GLuint colorBuff;

const GLuint NumVertices = 6;

void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(200, 200, 0));
	//model = glm::scale(model, glm::vec3(100, 100, 0));
	glm::mat4 projection = glm::ortho(0.f, float(screenWidth), float(screenHeight), 0.f);
	glm::mat4 mvp = projection*model;

	std::vector<glm::vec2> vert;
	vert.push_back(glm::vec2(400.f, 600.f));
	vert.push_back(glm::vec2(400.f, 300.f));		

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, tree.vertex.size()*sizeof(tree.vertex), tree.vertex.data(), GL_STATIC_DRAW);
	/*glGenBuffers(1, &indexBuff);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tree.index.size()*sizeof(tree.index), tree.index.data(), GL_STATIC_DRAW);*/

	GLuint program = LoadShader("vert.glsl", "frag.glsl");
	glUseProgram(program);

	GLuint matr = glGetUniformLocation(program, "MVP");
	glUniformMatrix4fv(matr, 1, GL_FALSE, &mvp[0][0]);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	glGenBuffers(1, &colorBuff);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuff);
	glBufferData(GL_ARRAY_BUFFER, tree.color.size()*sizeof(tree.color), tree.color.data(), GL_STATIC_DRAW);
	GLuint vColor = glGetAttribLocation(program, "color");
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
	//glLineWidth(2.0f);
	glClearColor(0.3f, 0.5f, 0.8f, 0.0f);
}

void Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	//glDrawElements(GL_LINES, tree.index.size(), GL_UNSIGNED_INT, 0);
	for (unsigned int i = 0; i < tree.vertex.size(); i =i +2)
	{
		glLineWidth(tree.sizes.at(i/2) / 40.0f + 1.0f);
		glDrawArrays(GL_LINES, i, 2);
	}
}

void clear(void)
{
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, Buffers);
	glDeleteBuffers(1, &colorBuff);
	glBindVertexArray(0);
	glDeleteVertexArrays(NumVAOs, VAOs);
}

//int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd )
int wmain(int argc, char* argv[])
{
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("SDL2 OpenGL4.4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetSwapInterval(1);
	context = SDL_GL_CreateContext(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);
	init();
	std::cout <<"OPENGL VERSION: "<< (const char*)glGetString(GL_VERSION) << std::endl;
	std::cout <<"GLSL VERSION : " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)<< std::endl;	
	SDL_Event _event;
	bool running = true;
	while (running)
	{
		while (SDL_PollEvent(&_event))
		{
			if (_event.type == SDL_QUIT || _event.key.keysym.sym == SDLK_ESCAPE){ running = false; }
		}
		Render();
		SDL_GL_SwapWindow(window);
	}
	SDL_GL_DeleteContext(context);
	clear();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}