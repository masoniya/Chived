#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Window.h"


static void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}


Window::Window()
{
	window = nullptr;
}

void Window::init(int width, int height, const char * name)
{
	//only initialize the window once
	if (window != nullptr) {
		return;
	}

	//create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, name, nullptr, nullptr);

	if (window == nullptr) {
		throw std::runtime_error("Failed to create GLFW window");
	}

	//set opengl context
	glfwMakeContextCurrent(window);

	//load opengl functions (only required once per program)
	static bool gladLoaded = false;
	if (!gladLoaded) {
		gladLoaded = true;
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize GLAD.");
		}
	}

	//set viewport
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 440");
}

void Window::swapBuffers()
{
	glfwSwapBuffers(window);
}

void Window::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void Window::close()
{
	glfwTerminate();
}
