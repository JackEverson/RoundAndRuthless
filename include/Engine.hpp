#pragma once  

#include <glad/glad.h>
#include <GLFW/glfw3.h>



class GardenEngine
{
public:
	GardenEngine(std::string, int win_width, int win_height);
	~GardenEngine();
	
	int Start();


private:
	
	GLFWwindow* m_window;

	void processInput();
	//void processInput(GLFWwindow* window);

};

// callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height);




