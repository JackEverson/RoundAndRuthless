#pragma once  

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"

#include "Audio.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "ClickCounter.hpp"


class GardenEngine
{

private:
	
	GLFWwindow* m_window;
	Renderer* m_renderer;
	Texture* test_texture;
	Texture* background_texture;

	SimpleSoundManager& soundManager;

	Camera m_camera;


	ClickCounter* m_clickCounter;

	bool m_first_click = false;



public:
	GardenEngine(std::string, int win_width, int win_height);
	~GardenEngine();

	int Start(float fps);

private:

	void setupGlfwWindow(std::string name, int win_width, int win_height);
	void setupOpenGl();
	void setupImGui();
	void setupAudio();
	void setupGameState();

	void processInput();
	void updateGameState();
	void renderScene();
	void renderImgui(double x, double y);
};

// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


