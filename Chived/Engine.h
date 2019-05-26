#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "ShaderProgram.h"
#include "Window.h"


const int WIDTH = 800;
const int HEIGHT = 600;

const char* const vertexShaderPath = "shaders/mainShader.vert";
const char* const fragmentShaderPath = "shaders/mainShader.frag";

typedef enum { create, extract } Mode;

enum class Algo;

class Engine
{
public:
	Engine();

	void start();

	//Global variables to control with gui
	static bool modeChosen;
	static bool filesDropped;
	static bool algoChosen;
	static bool workComplete;
	static bool invalidArchive;

	static Mode mode;

	static const char ** paths;
	static std::vector<std::string> pathStrings;
	static int pathCount;
	static Algo compression;
	static char archiveName[256];
	static char archiveDirectory[256];
	static char extractName[256];
	static char extractDirectory[256];

private:
	float vertices[6];

	uint32_t vbo;
	uint32_t vao;

	ShaderProgram program;

	Window *window;

	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;

	void init();
	void mainLoop();
	void cleanup();
	
	void initWindow();
	void initShaderProgram();
	void createVertexObjects();

	void renderFrame();
};
