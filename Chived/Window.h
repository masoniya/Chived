#pragma once

#include <stdexcept>

#include <glad/glad.h>
#include <glfw/glfw3.h>



class Window
{
public:
	Window();

	void init(int width, int height, const char* name);
	void swapBuffers();
	void setClearColor(float r, float g, float b, float a);
	void close();

	GLFWwindow* getWindow() { return window; }

private:
	GLFWwindow *window;

};
