#include "TestScene.hpp"

#include <glm/gtc/matrix_transform.hpp>



TestScene::TestScene() :
	soundManager(SimpleSoundManager::Instance()),
	m_sushi_texture(Texture("./res/textures/sushi.png")),
	m_shark_texture(Texture("./res/textures/shark.png")),
	m_rock_texture(Texture("./res/textures/rock.png"))
{
	m_player.position = glm::vec2(0.0f, 0.3f);
	m_player.velocity = glm::vec2(0.0f, 0.0f);
	m_player.jump_power = 4.5f;
	m_player.run_speed = 2.0f;
	m_player.size = glm::vec2(0.06f, 0.06f);
	
	updateLocations();

	m_player_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_player_sprite.texture = &m_sushi_texture;

	m_shark_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_shark_sprite.texture = &m_shark_texture;

	Platform ground;
	ground.position = glm::vec2(0.0f, -0.1f);
	ground.size = glm::vec2(10.0f, 0.2f);
	m_platforms.push_back(ground);
	m_player.currentPlatform = &ground;

	generatePlatforms(m_player.position.y);
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
	
	generatePlatforms(m_player.position.y);

	m_fail_y += delta * m_fail_speed;

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




	// walls
	renderer.BeginBatchDraw(2);
	SpriteInstance wall_left;
	wall_left.position = glm::vec3(-192/108 - 0.08f, 0.0f, 0.0f);
	wall_left.size = glm::vec2(0.1f, 10.0f);
	wall_left.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	wall_left.texture = &m_rock_texture;
	SpriteInstance wall_right;
	wall_right.position = glm::vec3(192 / 108 + 0.08f, 0.0f, 0.0f);
	wall_right.size = glm::vec2(0.1f, 10.0f);
	wall_right.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	wall_right.texture = &m_rock_texture;
	renderer.SubmitSprite(wall_left);
	renderer.SubmitSprite(wall_right);
	renderer.RendBatch(view, projection);

	// platforms
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


	// player
	renderer.BeginBatchDraw(1);
	renderer.SubmitSprite(m_player_sprite);
	glm::mat4 model = glm::mat4(1.0f);
	float angle = -m_player.position.x * 5;
	glm::vec3 axis(0.0f, 0.0f, 1.0f);
	glm::mat4 rotation = glm::rotate(model, angle, axis);
	renderer.RendBatch(rotation, view, projection);

	// shark
	renderer.BeginBatchDraw(1);
	renderer.SubmitSprite(m_shark_sprite);
	model = glm::mat4(1.0f);
	rotation = glm::rotate(model, glm::half_pi<float>(), axis);
	renderer.RendBatch(rotation, view, projection);


	////debug fail line
	//renderer.BeginBatchDraw(1);
	//SpriteInstance fail_line;
	//fail_line.position = glm::vec3(0.0f, m_fail_y, -0.01f);
	//fail_line.size = glm::vec2(10.0f, 0.01f);
	//fail_line.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//fail_line.texture = &m_rock_texture;
	//renderer.SubmitSprite(fail_line);
	//renderer.RendBatch(view, projection);



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

	m_player_sprite.position = glm::vec3(m_player.position, -0.001f);
	m_player_sprite.size = m_player.size;
	//m_camera.SetCamera(m_player_sprite.position.x, m_player_sprite.position.y, m_player_sprite.position.z - 1);
	m_camera.SetCamera(0.0f, m_player_sprite.position.y, m_player_sprite.position.z - 1);

	m_shark_sprite.size = glm::vec2(1920.0f / 1080.0f, 1.0f);
	m_shark_sprite.position = glm::vec3(0.0f, m_fail_y - m_shark_sprite.size.x * 0.5, 0.0f);

}
