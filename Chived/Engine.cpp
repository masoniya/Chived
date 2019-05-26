#include <cmath>
#include <sstream>
#include <filesystem>
#include <cstring>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Engine.h"
#include "Compressor.h"
#include "Archiver.h"


namespace fs = std::filesystem;


static void drop_callback(GLFWwindow * window, int count, const char ** paths);

bool Engine::modeChosen = false;
bool Engine::filesDropped = false;
bool Engine::algoChosen = false;
bool Engine::workComplete = false;
bool Engine::invalidArchive = false;

Mode Engine::mode = create;

const char** Engine::paths;
std::vector<std::string> Engine::pathStrings;
int Engine::pathCount = 0;
Algo Engine::compression = Algo::store;

char Engine::archiveName[256] = "archive.car";
char Engine::archiveDirectory[256] = "C:\\Users\\ASUS\\AppData\\Local\\Temp\\chived";
char Engine::extractName[256] = "C:\\Users\\ASUS\\AppData\\Local\\Temp\\chived\\archive.car";
char Engine::extractDirectory[256] = "C:\\Users\\ASUS\\AppData\\Local\\Temp\\chived";


Engine::Engine() :
	vertices {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.0f,  0.5f,
	}
{
}


void Engine::init()
{
	window = new Window;

	initWindow();
	initShaderProgram();
	createVertexObjects();

	glfwSetDropCallback(window->getWindow(), drop_callback);

	show_demo_window = true;
	show_another_window = true;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void Engine::mainLoop()
{
	while (!glfwWindowShouldClose(window->getWindow())) {

		renderFrame();

		glfwWaitEventsTimeout(0.014666667);
	}
}

void Engine::cleanup()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	window->close();
}

void Engine::initWindow()
{
	window->init(WIDTH, HEIGHT, "Engine");
	window->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Engine::initShaderProgram()
{
	program.compileShaders(vertexShaderPath, fragmentShaderPath);
}

void Engine::createVertexObjects()
{
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Engine::renderFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();



	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	/*if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);*/

	{
		ImGui::Begin("Chived Archiver");

		//Mode select screen
		if (!Engine::modeChosen) {
			ImGui::Text("Select Mode");

			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				Engine::modeChosen = true;
				Engine::mode = create;
				//std::cout << "Create Mode Chosen" << std::endl;
			}

			ImGui::SameLine();

			if (ImGui::Button("Extract")) {
				Engine::modeChosen = true;
				Engine::mode = extract;
				//std::cout << "Extract Mode Chosen" << std::endl;
			}
		}

		//Drop files screen
		if (Engine::modeChosen && !Engine::filesDropped) {
			//waiting for drop starts here

			if (Engine::mode == create) {
				ImGui::Text("Drop Files To Create Archive");
			}

			else if (Engine::mode == extract) {
				ImGui::Text("Drop Archive To Extract");
			}

			if (ImGui::Button("Cancel")) {
				Engine::modeChosen = false;
				//std::cout << "Canceled... Returning" << std::endl;
			}

			//waiting for drop ends here
		}

		//Select Algorithm Screen
		if (Engine::mode == create && Engine::filesDropped && !Engine::algoChosen) {
			
			ImGui::Text("Select Compression Algorithm");

			const char * items[5] = {"Store", "RLE", "Huffman", "Adaptive Huffman", "Shannon-Fano"};
			static int item_current = 0;
			ImGui::Combo("Algorithm", &item_current, items, 5);

			if (ImGui::Button("Select")) {
				switch (item_current)
				{
				case 0 : Engine::compression = Algo::store; break;
				case 1 : Engine::compression = Algo::rle; break;
				case 2 : Engine::compression = Algo::huff; break;
				case 3 : Engine::compression = Algo::adahuff; break;
				case 4 : Engine::compression = Algo::shan_fano; break;
				default : break;
				}

				Engine::algoChosen = true;
				//std::cout << "Successfully Chose Algorithm" << std::endl;
			}
			
			if (ImGui::Button("Cancel")) {
				Engine::modeChosen = false;
				Engine::filesDropped = false;
				//std::cout << "Canceled... Returning" << std::endl;
			}
		}

		//Select Compression Targets Screen
		if (Engine::mode == create && Engine::algoChosen && !Engine::workComplete) {
			ImGui::Text("Select Targets");
			ImGui::InputText("Archive Name", Engine::archiveName, 256);
			ImGui::InputText("Target Directory", Engine::archiveDirectory, 256);

			if (ImGui::Button("Start")) {
				Engine::paths = new const char*[Engine::pathCount];
				for (int i = 0; i < Engine::pathStrings.size(); i++) {
					Engine::paths[i] = Engine::pathStrings.at(i).c_str();
				}

				for (int i = 0; i < Engine::pathCount; i++) {
					std::cout << paths[i] << std::endl;
				}

				Archiver::createArchive(Engine::pathCount, Engine::paths, Engine::archiveDirectory, Engine::archiveName, Engine::compression);
				Engine::workComplete = true;
				//std::cout << "Work complete" << std::endl;
			}

			if (ImGui::Button("Cancel")) {
				Engine::modeChosen = false;
				Engine::filesDropped = false;
				Engine::algoChosen = false;
				//std::cout << "Canceled... Returning" << std::endl;
			}
		}

		//Select Extraction Targets Screen
		if (Engine::mode == extract && Engine::filesDropped && !Engine::workComplete) {
			if (fs::is_regular_file(Engine::extractName)) {
				ImGui::Text("Select Targets");

				ImGui::Text("Extract Target : ");
				ImGui::SameLine();
				ImGui::Text(Engine::extractName);

				ImGui::InputText("Extract Directory", Engine::extractDirectory, 256);

				if (ImGui::Button("Start")) {
					Archiver::extractArchive(Engine::extractName, Engine::extractDirectory);
					Engine::workComplete = true;
					//std::cout << "Work complete" << std::endl;
				}

				if (ImGui::Button("Cancel")) {
					Engine::modeChosen = false;
					Engine::filesDropped = false;
					Engine::algoChosen = false;
					//std::cout << "Canceled... Returning" << std::endl;
				}
			}
			else {
				Engine::invalidArchive = true;
			}
		}

		//Invalid Archive Screen
		if (Engine::invalidArchive) {
			ImGui::Text("Invalid Archive Name");

			if (ImGui::Button("Return")) {
				Engine::modeChosen = false;
				Engine::filesDropped = false;
				Engine::algoChosen = false;
				Engine::workComplete = false;
				Engine::invalidArchive = false;
			}
		}

		//Return Screen
		if (Engine::workComplete) {
			if (Engine::mode == create) {
				ImGui::Text("Successfully Created Archive");
			}
			else if (Engine::mode == extract) {
				ImGui::Text("Successfully Extracted Archive");
			}

			if (ImGui::Button("Return")) {
				Engine::modeChosen = false;
				Engine::filesDropped = false;
				Engine::algoChosen = false;
				Engine::workComplete = false;
			}
		}


		ImGui::End();
	}

	ImGui::Render();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

	program.useProgram();

	glBindVertexArray(vao);	

	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glBindVertexArray(0);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	window->swapBuffers();
}

void Engine::start()
{
	init();
	mainLoop();
	cleanup();
}


void drop_callback(GLFWwindow * window, int count, const char ** paths)
{
	if (Engine::modeChosen && !Engine::filesDropped) {
		Engine::pathCount = count;

		for (int i = 0; i < count; i++) {
			Engine::pathStrings.push_back(paths[i]);
		}
		
		Engine::filesDropped = true;

		if (Engine::mode == extract) {
			strcpy_s(Engine::extractName, paths[0]);
		}
		//std::cout << "Files Dropped Successfully" << std::endl;
	}
}
