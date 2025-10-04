#include "TestScene.hpp"



TestScene::TestScene() :
	soundManager(SimpleSoundManager::Instance()),
	m_sushi_texture(Texture("./res/textures/sushi.png")),
	m_rock_texture(Texture("./res/textures/rock.png"))
{
	m_player.position = glm::vec2(0.0f, 0.0f);
	m_player.velocity = glm::vec2(0.0f, 0.0f);
	m_player.jump_power = 4.5f;
	m_player.run_speed = 2.0f;
	m_player.width = 0.06f;
	m_player.height = 0.05f;
	
	updateLocations();

	m_player_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
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

Scene* TestScene::update(float delta)
{
	checkCollision(m_player, 0.0f, 0.5f);
	updatePlayerPhysics(m_player, delta);
	updateLocations();
	
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

void TestScene::handleInput(GLFWwindow& window, float delta)
{
	GLCall(glfwPollEvents());

	handlePlayerInput(m_player, window, delta);



	if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(&window, true);
}


void TestScene::updateLocations() {

	m_player_sprite.position = glm::vec3(m_player.position, 0);
	m_player_sprite.size = glm::vec2(m_player.width, m_player.height);
	m_camera.SetCamera(m_player_sprite.position.x, m_player_sprite.position.y, m_player_sprite.position.z - 1);



}
