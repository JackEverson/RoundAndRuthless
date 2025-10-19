#pragma once

#include "Engine.hpp"
#include "Scene.hpp"

#include <print>

struct Platform {
	glm::vec2 position;
	glm::vec2 size;
};

struct Player {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 size;
	float jump_power;
	float run_speed;
	bool isGrounded;
	bool isAlive;
	Platform* currentPlatform;
};


class MainScene : public Scene
{
private:
	SimpleSoundManager& soundManager;

	Camera m_camera;

	Texture m_rock_texture;
	std::vector<SpriteInstance> m_rocks;

	Texture m_sushi_texture;
	SpriteInstance m_player_sprite;
	Texture m_shark_texture;
	SpriteInstance m_shark_sprite;

	Texture m_death_texture;
	SpriteInstance m_death_sprite;


	Player m_player;

	bool m_clicked = false;
	bool m_restart = false;

	const float m_gravity = -9.81f;
	const float m_level_left = -192.0f / 108.0f;
	const float m_level_right = 192.0f / 108.0f;

	const float m_platform_width = 0.3f;
	const float m_minPlatSpacingY = 0.1f;
	const float m_maxPlatSpacingY = 0.3f;
	const float m_minPlatSpacingX = m_level_left + 0.3;
	const float m_maxPlatSpacingX = m_level_right - 0.3;;

	float m_fail_y = -2.0f;
	float m_fail_speed = 0.2f;

	float m_lastplatform_y = 0.0f;
	std::vector<Platform> m_platforms;


public:
	// scene methods
	MainScene();
	~MainScene();

	void onEnter() override;
	void onExit() override;
	Scene* update(float delta) override;
	void render(GLFWwindow& window, Renderer& renderer) override;
	void handleInput(GLFWwindow& window, float delta) override;

private:
	// game methods
	void updateLocations();
	void updatePlayerPhysics(Player& player, float deltaTime);
	void checkWallCollision(Player& player, float tubeX);

	void generatePlatforms(float& playerY);
	void resolvePlatformCollisions(Player& player);
	bool checkPlatformCollisions(const float& playerTop, const float& playerBottom, const float& playerLeft, const float& playerRight, const Platform& platform);
	
	void handlePlayerInput(Player& player, GLFWwindow& window, float delta);
	void renderImgui(int w, int h);

};
