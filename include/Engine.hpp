#pragma once  

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.hpp"
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

	GrubWrangler* m_grubs;


	void processInput();
	//void processInput(GLFWwindow* window);

};

// callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height);




