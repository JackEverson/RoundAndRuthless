#pragma once

#include "Engine.hpp"
#include "Scene.hpp"

class TestScene : public Scene
{
private:
	SimpleSoundManager& soundManager;

	Camera m_camera;

	Texture m_rock_texture;
	std::vector<SpriteInstance> m_rocks;

	Texture m_sushi_texture;
	SpriteInstance m_player_sprite;

public:
	TestScene();
	~TestScene();
	void onEnter() override;
	void onExit() override;
	Scene* update() override;
	void render(GLFWwindow& window, Renderer& renderer) override;
	void handleInput(GLFWwindow& window) override;


private:

};

