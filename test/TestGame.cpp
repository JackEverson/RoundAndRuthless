#include "TestScene.hpp"


void TestScene::updatePlayerPhysics(Player& player, float deltaTime) {

	checkWallCollision(m_player, 1920 / 1080);

	for (const auto& plat : m_platforms) {
		resolvePlatformCollisions(m_player, plat);
		if (player.isGrounded) break;
	}

	if (!player.isGrounded) checkGroundCollision(m_player, 0.0f);

	if (!player.isGrounded) {
		player.velocity.y += m_gravity * deltaTime;
	}

	player.position += player.velocity * deltaTime;
}


void TestScene::checkGroundCollision(Player& player, float groundY) {
	float player_ground = groundY + player.size.y * 0.5;
	
	if (player.position.y <= player_ground) {
		player.position.y = player_ground;
		player.velocity.y = 0.0f;
		player.isGrounded = true;
	}
}

void TestScene::checkWallCollision(Player& player, float tubeX) {

	if (player.position.x >= tubeX) {
		player.position.x = tubeX;
		player.velocity.x *= -1.0f;
	}
	else if (player.position.x <= -tubeX) {
		player.position.x = -tubeX;
		player.velocity.x *= -1.0f;
	}
}


void TestScene::resolvePlatformCollisions(Player& player, const Platform& platform) {
	
	float player_top = player.position.y + player.size.y * 0.5f;
	float player_bottom = player.position.y - player.size.y * 0.5f;
	float player_left = player.position.x - player.size.x * 0.5f;
	float player_right = player.position.x + player.size.x * 0.5f;
	
	if (checkPlatformCollisions(player_top, player_bottom, player_left, player_right, platform) && player.velocity.y < 0.0f) {
		player.position.y = platform.position.y + platform.size.y * 0.5f + player.size.y * 0.5f;
		player.velocity.y = 0.0f;           
		player.isGrounded = true;         
		//std::println("Landed on platform at y: {}", platform.position.y);
		return;
	}
	player.isGrounded = false;
}

bool TestScene::checkPlatformCollisions(const float& playerTop, const float& playerBottom, const float& playerLeft, const float& playerRight, const Platform& platform) {
	
	float platform_top = platform.position.y + platform.size.y * 0.5f;
	//float platform_bottom = platform.position.y + platform.size.y * -0.5f;
	float platform_left = platform.position.x - platform.size.x * 0.5f;
	float platform_right = platform.position.x + platform.size.x * 0.5f;

	return 
		playerLeft < platform_right &&
		playerRight > platform_left &&
		playerBottom < platform_top + (playerTop - playerBottom) &&
		playerBottom >= platform_top - (playerTop - playerBottom);
}



void TestScene::handlePlayerInput(Player& player, GLFWwindow& window, float delta) {

	bool moved = false;

	float adjust = 1.0f;
	if (glfwGetKey(&window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		adjust = 2.0f;
	}

	if (glfwGetKey(&window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(&window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (m_player.isGrounded)
		{
			m_player.position.y += 0.001f;
			m_player.velocity.y += m_player.jump_power * adjust;
			m_player.isGrounded = false;
		}
		moved = true;
	}
	if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS) {
		if (m_player.isGrounded) {
			m_player.velocity.x *= 5 * delta;
		}
		moved = true;
	}
	if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS) {
		m_player.velocity.x -= m_player.run_speed * delta * adjust;
		moved = true;
	}
	if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS) {
		m_player.velocity.x += m_player.run_speed * delta * adjust;
		moved = true;
	}

	//if (moved) {
	//	m_player.size.y = 0.05f;
	//}
	//else {
	//	m_player.size.y = 0.06f;
	//}

}

