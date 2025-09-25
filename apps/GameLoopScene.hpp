#pragma once

#include <random>

#include "Scene.hpp"


class GameLoopScene : public Scene {

public:

	ClickCounter m_clickCounter;
	Camera m_camera;
	Approacher m_approacher;
	SimpleSoundManager& soundManager;

	Texture background_texture;
	Texture approacher_texture;
	Texture floor_texture;
	Texture wall_texture;
	Texture ceiling_texture;
	Texture delivered_text;

	bool m_first_click = false;
	bool m_death = false;
	bool m_jump_scared = false;


	GameLoopScene();
	~GameLoopScene();
	
	void onEnter() override;
	void onExit() override;

	Scene* update() override;
	void render(GLFWwindow& window, Renderer& renderer) override;
	void handleInput(GLFWwindow& window) override;
	
	
};
