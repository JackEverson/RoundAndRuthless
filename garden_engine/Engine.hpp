#pragma once  

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"

#include "Audio.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "ClickCounter.hpp"
#include "Approacher.hpp"
#include "Scene.hpp"
#include "Button.hpp"
#include "gl_debug.hpp"


class GardenEngine
{

private:
	
	std::unique_ptr<Scene> m_currentScene;

	GLFWwindow* m_window;
	Renderer* m_renderer;
	SimpleSoundManager& soundManager;

	bool m_first_click = false;



public:
	GardenEngine(std::string, bool windowed, int win_width, int win_height);
	~GardenEngine();

	int Start(std::unique_ptr<Scene> scene, float fps);

private:

	void setupGlfwWindow(std::string name, bool windowed, int win_width, int win_height);
	void setupOpenGl();
	void setupImGui();
	void setupAudio();

	void prepImgui();
	void renderImgui(double x, double y);

	float calculateDeltaTime(std::chrono::steady_clock::time_point& lastFrameStart);
};

// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


