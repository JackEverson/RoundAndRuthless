#include "TestScene.hpp"

void TestScene::checkCollision(Player& player, float groundY, float tubeX) {
	if (player.position.y <= groundY) {
		player.position.y = groundY;
		player.velocity.y = 0.0f;
		player.isGrounded = true;
	}

	if (player.position.x >= tubeX) {
		player.position.x = tubeX;
		player.velocity.x *= -0.1;
	}
	else if (player.position.x <= -tubeX) {
		player.position.x = -tubeX;
		player.velocity.x *= -0.1;
	}
}

void TestScene::updatePlayerPhysics(Player& player, float deltaTime) {

	if (!player.isGrounded) {
		player.velocity.y += m_gravity * deltaTime;
	}

	player.position += player.velocity * deltaTime;
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
			m_player.position.y += 0.01f;
			m_player.velocity.y += m_player.jump_power * adjust;
			m_player.isGrounded = false;
		}
		moved = true;
	}
	if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS) {
		//m_player_sprite += glm::vec3(0.0f, -move, 0.0f);
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

	if (moved) {
		m_player.height = 0.05f;
	}
	else {
		m_player.height = 0.06f;
	}

}

