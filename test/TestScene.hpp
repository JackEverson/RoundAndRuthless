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
	// scene methods
	TestScene();
	~TestScene();
	void onEnter() override;
	void onExit() override;
	Scene* update(float delta) override;
	void render(GLFWwindow& window, Renderer& renderer) override;
	void handleInput(GLFWwindow& window, float delta) override;

private:
	void updateLocations();

private:
	// game methods
	
	struct Player {
		glm::vec2 position;
		glm::vec2 velocity;
		float jump_power, run_speed;
		float width, height;
		bool isGrounded;
	};
	
	Player m_player;
	float m_gravity = -9.81f;

	void updatePlayerPhysics(Player& player, float deltaTime);
	void checkCollision(Player& player, float groundY, float tubeX);
	void handlePlayerInput(Player& player, GLFWwindow& window, float delta);

};

