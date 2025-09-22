#pragma once

#include "Scene.hpp"
#include "gl_debug.hpp"
#include "Audio.hpp"
#include "GameLoopScene.hpp"

#include "Button.hpp"

#include "Renderer.hpp"

class IntroScene : public Scene {

public:
    
    bool start_game = false;
    bool left_click_before = false;

    Camera m_camera;
    Approacher m_approacher;
    SimpleSoundManager& soundManager;


    Button button;

    Texture background_texture;
    bool m_first_click = false;

    static void sayHello() {
        static int counter = 0;
        std::println("Button says hello {}", counter++);
    }


    IntroScene() :
        soundManager(SimpleSoundManager::Instance()),
        m_approacher(Approacher(30.0f, 1.0f)),
        background_texture(Texture("./res/textures/background.png")),
        button(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), Texture("./res/textures/background.png"), sayHello)
    {
        
    }

    void onEnter() override {

        //soundManager.LoadSound("click", "./res/sounds/click.wav");
        //soundManager.LoadSound("cave", "./res/sounds/cave.ogg");

        //soundManager.PlayBackgroundMusic("cave", 1.0f); 
    }
    
    void onExit() override {

    }

    Scene* update() override {

        if (start_game) {
            return new GameLoopScene;
        }

        return nullptr;
    }

    void render(GLFWwindow& window, Renderer& renderer) override {


        renderer.Clear(0.1f, 0.1f, 0.1f, 1.0f);
        //SpriteInstance background;
        //background.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        //background.texture = background_texture;
        //m_renderer->DrawBackground(background);

        
        glm::vec3 campos = m_camera.GetLocation();


        int w, h;
        glfwGetWindowSize(&window, &w, &h);
        float aspect = w / h;
        glm::mat4 view = m_camera.GetViewMat();
        glm::mat4 projection = m_camera.GetProjectionMat(w, h);

        renderer.DrawBackground(background_texture);


        renderer.BeginBatchDraw(4);
        SpriteInstance button_sprite;
        button_sprite.position = button.m_worldPosition;
        button_sprite.size = button.m_size;
        button_sprite.rotation = 0.0f;
        button_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        button_sprite.texture = &button.texture;
        renderer.SubmitSprite(button_sprite);
        renderer.RendBatch(view, projection);



        GLCall(glfwSwapBuffers(&window));
    }


    
    void handleInput(GLFWwindow& window) {
        GLCall(glfwPollEvents());

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

        if (button.IsMouseOverButton(view, proj, glm::vec2(mouseX, mouseY), w, h)) {
            button.click();
        }

        //if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        //    //start_game = true;
        //    button.click();
        //}

        if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(&window, true);

    }
};
