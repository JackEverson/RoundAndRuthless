#include "TestScene.hpp"

#include <glm/gtc/matrix_transform.hpp>



TestScene::TestScene() :
	soundManager(SimpleSoundManager::Instance()),
	m_sushi_texture(Texture("./res/textures/sushi.png")),
	m_rock_texture(Texture("./res/textures/rock.png"))
{
	m_player.position = glm::vec2(0.0f, 0.0f);
	m_player.velocity = glm::vec2(0.0f, 0.0f);
	m_player.jump_power = 4.5f;
	m_player.run_speed = 2.0f;
	m_player.size = glm::vec2(0.06f, 0.06f);
	
	updateLocations();

	m_player_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_player_sprite.texture = &m_sushi_texture;

	int ground_rock_count = 7;
	for (int i = 0; i < ground_rock_count; i++) {
		SpriteInstance rock;
		rock.size = glm::vec2(0.3f, 0.2f);
		rock.position = glm::vec3((float)(i - ground_rock_count / 2) * 0.3f, rock.size.y * -0.5f, 0.0f);
		rock.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		rock.texture = &m_rock_texture;
		m_rocks.push_back(rock);
	}
	
	int plat_count = 50;
	m_platforms.reserve(plat_count);
	for (int i = 0; i < plat_count; i++) {
		Platform plat;
		//plat.position = glm::vec2((float)(i - 1) * 0.5f, 0.3f);
		plat.position = glm::vec2(-1 + (i % 5) * 0.5f, 0.3f * i);
		plat.size = glm::vec2(0.2f, 0.05f);
		m_platforms.push_back(plat);
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

	renderer.BeginBatchDraw(m_platforms.size());
	for (const auto& p : m_platforms) {
		SpriteInstance plat_sprite;
		plat_sprite.position = glm::vec3(p.position, 0.0f);
		plat_sprite.size = p.size;
		plat_sprite.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		plat_sprite.texture = &m_rock_texture;
		renderer.SubmitSprite(plat_sprite);
	}
	renderer.RendBatch(view, projection);

	renderer.BeginBatchDraw(1);
	renderer.SubmitSprite(m_player_sprite);

	glm::mat4 model = glm::mat4(1.0f);
	float angle = -m_player.position.x * glm::pi<float>() * 2;
	glm::vec3 axis(0.0f, 0.0f, 1.0f);
	glm::mat4 rotation = glm::rotate(model, angle, axis);
	
	renderer.RendBatch(rotation, view, projection);

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
	m_player_sprite.size = m_player.size;
	//m_camera.SetCamera(m_player_sprite.position.x, m_player_sprite.position.y, m_player_sprite.position.z - 1);
	m_camera.SetCamera(0.0f, m_player_sprite.position.y, m_player_sprite.position.z - 1);

}
