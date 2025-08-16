#pragma once  

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"

#include "Camera.hpp"
#include "ClickCounter.hpp"
#include "bugs/GrubWrangler.hpp"


class GardenEngine
{

private:
	
	GLFWwindow* m_window;
	ImGuiIO* m_imgui_io;

	Camera m_camera;

	ClickCounter* m_clickCounter;
	GrubWrangler* m_grubs;


	//void processInput(GLFWwindow* window);
	void RunImGuiDemo();


public:
	GardenEngine(std::string, int win_width, int win_height);
	~GardenEngine();

	int Start(float fps);

private:

	void setupGlfwWindow(std::string name, int win_width, int win_height);
	void setupOpenGl();
	void setupImGui();
	void setupGameState();

	void processInput();
	void updateGameState();
	void renderScene();
	void renderImgui();
};

// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


