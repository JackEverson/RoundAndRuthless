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
	bool approach = false;
	float locy = 0.0f;
	float locx = 0.0f;
	float locz = 0.0f;
	float red = 0.0f;

	void processInput();
	//void processInput(GLFWwindow* window);

};

// callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height);




