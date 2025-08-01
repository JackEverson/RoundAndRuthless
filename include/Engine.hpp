#pragma once  

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"


#include "Camera.hpp"
#include "ClickCounter.hpp"
#include "bugs/GrubWrangler.hpp"




class GardenEngine
{
public:
	GardenEngine(std::string, int win_width, int win_height);
	~GardenEngine();
	
	int Start(float fps);
	

private:
	
	GLFWwindow* m_window;

	Camera m_camera;
	ClickCounter m_clickCounter;

	GrubWrangler* m_grubs;


	void processInput();
	//void processInput(GLFWwindow* window);
	void RunImGuiDemo(const ImGuiIO& io, bool& boolle);
};

// callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height);




