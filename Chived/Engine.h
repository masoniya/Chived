#pragma once

#include <string>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "ShaderProgram.h"
#include "Window.h"



const int WIDTH = 1366;
const int HEIGHT = 768;

const char* const vertexShaderPath = "shaders/mainShader.vert";
const char* const fragmentShaderPath = "shaders/mainShader.frag";

class Engine
{
public:
	Engine();

	void start();

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
