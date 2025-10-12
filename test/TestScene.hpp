#pragma once

#include "Engine.hpp"
#include "Scene.hpp"

#include <print>

struct Player {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 size;
	float jump_power;
	float run_speed;
	bool isGrounded;
};

struct Platform {
	glm::vec2 position;
	glm::vec2 size;
};



class TestScene : public Scene
{
private:
	SimpleSoundManager& soundManager;

	Camera m_camera;

	Texture m_rock_texture;
	std::vector<SpriteInstance> m_rocks;

	Texture m_sushi_texture;
	SpriteInstance m_player_sprite;

	Player m_player;
	float m_gravity = -9.81f;

	std::vector<Platform> m_platforms;


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
	// game methods
	void updateLocations();
	void updatePlayerPhysics(Player& player, float deltaTime);
	void checkGroundCollision(Player& player, float groundY);
	void checkWallCollision(Player& player, float tubeX);

	void resolvePlatformCollisions(Player& player, const Platform& platform);
	bool checkPlatformCollisions(const float& playerTop, const float& playerBottom, const float& playerLeft, const float& playerRight, const Platform& platform);
	
	void handlePlayerInput(Player& player, GLFWwindow& window, float delta);

};
