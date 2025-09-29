#pragma once


#include "Scene.hpp"

static bool QuitGame = false;


class ExitScene : public Scene {

public:

	Camera m_camera;
	SimpleSoundManager& soundManager;

	Button button_exit;

    Texture thankyou_texture;
    Texture exit_texture;
	Texture sushi_texture;

    bool left_click_before = false;


	static void ExitFunc() {
		QuitGame = true;
	}


	ExitScene() :
		soundManager(SimpleSoundManager::Instance()),
        thankyou_texture(Texture("./res/textures/thankyou.png")),
        exit_texture(Texture("./res/textures/exit.png")),
        sushi_texture(Texture("./res/textures/sushi.png")),
		button_exit(glm::vec3(0.4f, -0.3f, -0.2f), glm::vec2(0.4f, 0.2f), ExitFunc)
	{

	}

	void onEnter() override {

		//soundManager.LoadSound("beep", "./res/sounds/beep.wav");
		//soundManager.LoadSound("cave", "./res/sounds/cave.ogg");

		//soundManager.PlayBackgroundMusic("cave", 1.0f); 

    }

	void onExit() override {

	}

	Scene* update() override {
		

		return nullptr;
	}

    void render(GLFWwindow& window, Renderer& renderer) override {

        renderer.Clear(0.2f, 0.2f, 0.2f, 1.0f);

        glm::vec3 campos = m_camera.GetLocation();

        int w, h;
        glfwGetWindowSize(&window, &w, &h);
        glm::mat4 view = m_camera.GetViewMat();
        glm::mat4 projection = m_camera.GetProjectionMat(w, h);


        renderer.BeginBatchDraw(1);
        SpriteInstance sushi_sprite;
        sushi_sprite.position = glm::vec3(-0.7f, 0.0f, -0.2f);
        sushi_sprite.size = glm::vec2(1.2f, 0.8f);
        sushi_sprite.rotation = 0.0f;
        sushi_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        sushi_sprite.texture = &sushi_texture;
        renderer.SubmitSprite(sushi_sprite);
        renderer.RendBatch(view, projection);

        renderer.BeginBatchDraw(1);
        SpriteInstance thanks;
        thanks.position = glm::vec3(0.3f, 0.0f, -0.2f);
        thanks.size = glm::vec2(0.6f, 0.3f);
        thanks.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        thanks.texture = &thankyou_texture;
        renderer.SubmitSprite(thanks);
        renderer.RendBatch(view, projection);

        renderer.BeginBatchDraw(1);
        SpriteInstance exit;
        exit.position = button_exit.m_worldPosition;
        exit.size = button_exit.m_size;
        exit.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        exit.texture = &exit_texture;
        renderer.SubmitSprite(exit);
        renderer.RendBatch(view, projection);

        GLCall(glfwSwapBuffers(&window));
    }


    void handleInput(GLFWwindow& window) {
        GLCall(glfwPollEvents());

        if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS || QuitGame)
            glfwSetWindowShouldClose(&window, true);


        bool left_click = false;
        if (!left_click_before && glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            left_click = true;
        }

        // set left click before
        if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) left_click_before = true;
        else left_click_before = false;

        if (!left_click) {
            return;
        }

        int w, h;
        glfwGetWindowSize(&window, &w, &h);

        double mouseX, mouseY;
        glfwGetCursorPos(&window, &mouseX, &mouseY);

        auto view = m_camera.GetViewMat();
        auto proj = m_camera.GetProjectionMat(w, h);

        if (button_exit.IsMouseOverButton(view, proj, glm::vec2(mouseX, mouseY), w, h)) {
            soundManager.PlaySound("beep");
            button_exit.click();
        }


    }

};