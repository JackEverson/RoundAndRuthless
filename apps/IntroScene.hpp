#pragma once

#include "Scene.hpp"
#include "gl_debug.hpp"
#include "Audio.hpp"

#include "GameLoopScene.hpp"


class IntroScene : public Scene {

public:
    
    bool start_game = false;

    Camera m_camera;
    Approacher m_approacher;
    SimpleSoundManager& soundManager;

    Texture background_texture;
    bool m_first_click = false;

    IntroScene() :
    soundManager(SimpleSoundManager::Instance()),
        m_approacher(Approacher(30.0f, 1.0f)),
        background_texture(Texture("./res/textures/background.png"))
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

        GLCall(glfwSwapBuffers(&window));
    }


    
    void handleInput(GLFWwindow& window) {


        GLCall(glfwPollEvents());

        if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(&window, true);

        if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            start_game = true;
        }



    }
};
