
#include "TestScene.hpp"



TestScene::TestScene() :
	soundManager(SimpleSoundManager::Instance()),
	m_sushi_texture(Texture("./res/textures/sushi.png")),
	m_rock_texture(Texture("./res/textures/rock.png"))
{
	m_player_sprite.position = glm::vec3(0, 0, 0);
	m_player_sprite.size = glm::vec2(0.12f, 0.1f);
	m_player_sprite.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	m_player_sprite.texture = &m_sushi_texture;


	for (int i = 0; i < 5; i++) {
		SpriteInstance rock;
		rock.position = glm::vec3((float)(i - 2) * 0.3f, 0.0f, -1.0f);
		rock.size = glm::vec2(0.2f, 0.2f);
		rock.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		rock.texture = &m_rock_texture;
		m_rocks.push_back(rock);
	}

}



TestScene::~TestScene()
{

}

void TestScene::onEnter()
{
}

void TestScene::onExit()
{
}

Scene* TestScene::update()
{
	return nullptr;
}

void TestScene::render(GLFWwindow& window, Renderer& renderer)
{
	renderer.Clear(0.2f, 0.2f, 0.2f, 1.0f);

	glm::vec3 campos = m_camera.GetLocation();

	int w, h;
	glfwGetWindowSize(&window, &w, &h);
	glm::mat4 view = m_camera.GetViewMat();
	glm::mat4 projection = m_camera.GetProjectionMat(w, h);

	renderer.BeginBatchDraw(m_rocks.size());
	for (const auto& r : m_rocks) {
		renderer.SubmitSprite(r);
	}
	renderer.RendBatch(view, projection);

	renderer.BeginBatchDraw(1);
	renderer.SubmitSprite(m_player_sprite);
	renderer.RendBatch(view, projection);


	GLCall(glfwSwapBuffers(&window));
}

void TestScene::handleInput(GLFWwindow& window)
{
	GLCall(glfwPollEvents());

	bool moved = false;

	float sensitivity = 1.0f;
	if (glfwGetKey(&window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		sensitivity = 2.0f;
	}
	float move = 0.02f * sensitivity;
	if (glfwGetKey(&window, GLFW_KEY_W) == GLFW_PRESS) {
		m_player_sprite.position += glm::vec3(0.0f, move, 0.0f);
		moved = true;
	}
	if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS) {
		m_player_sprite.position += glm::vec3(0.0f, -move, 0.0f);
		moved = true;
	}
	if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS) {
		m_player_sprite.position += glm::vec3(-move, 0.0f, 0.0f);
		moved = true;
	}
	if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS) {
		m_player_sprite.position += glm::vec3(move, 0.0f, 0.0f);
		moved = true;
	}

	m_camera.SetCamera(m_player_sprite.position.x, m_player_sprite.position.y, m_player_sprite.position.z - 1);


	static float offset = 0.0f;
	static float speed = 0.001f;

	if (moved) {
		//float applied_offset = offset;
		
		//m_player_sprite.position.y += applied_offset;
		//size += speed;

		//if (size > 0.12f + 0.12f * 0.12f) {
		//	speed = -0.0001f;
		//}
		//if (size < 0.2 - 0.2f * 0.12f) {
		//	speed = 0.0001f;
		//}

	}
	else {
		m_player_sprite.size.y = 0.12f;
	}



	//if (glfwGetKey(&window, GLFW_KEY_SPACE) == GLFW_PRESS) {
	//	m_camera.ShiftCamera(0.0f, 0.0f, -move);
	//}
	//if (glfwGetKey(&window, GLFW_KEY_C) == GLFW_PRESS) {
	//	m_camera.ShiftCamera(0.0f, 0.0f, move);
	//}




	if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(&window, true);
}
