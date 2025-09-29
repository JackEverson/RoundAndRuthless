#pragma once

#include <random>
#include <print>

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
	Texture door_texture;
	Texture rock_texture;
	Texture key_texture;

	Texture delivered_text;

	Button button_rock;
	Button button_key;
	Button button_lock;

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
	
private:
	void DrawHallway(glm::mat4 view, glm::mat4 projection, Renderer& renderer);



};


static bool rock_clicked = false;
static bool key = false;
static bool escaped = false;
static bool death = false;

static void OnRockClick() {
	std::println("rock clicked");
	rock_clicked = true;
}

static void OnKeyClick(){
	if (rock_clicked) {
		key = true;
		std::println("key clicked");
	}
}

static void OnLockClick() {
	if (key) {
		escaped = true;
		std::println("lock clicked");
	}
}