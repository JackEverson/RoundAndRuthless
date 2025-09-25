#pragma once

#include "Scene.hpp"
#include "gl_debug.hpp"
#include "Audio.hpp"
#include "GameLoopScene.hpp"

#include "Button.hpp"

#include "Renderer.hpp"

static bool start_game = false;
static bool shutdown = false;

class IntroScene : public Scene {

public:

    bool left_click_before = false;

    Camera m_camera;
    SimpleSoundManager& soundManager;

    Button button_quit;
    Button button_start;

    Texture background_texture;
    Texture button_texture_red;
    Texture button_texture_green;
    Texture sushi_texture;
    bool m_first_click = false;

    static void QuitFunc() {
        std::println("QUIT: Intro Scene Quit Button Click");
        shutdown = true;
    }

    static void StartFunc() {
        std::println("START: Intro Scene Start Button Click");
        start_game = true;
    }

    IntroScene() :
        soundManager(SimpleSoundManager::Instance()),
        background_texture(Texture("./res/textures/background.png")),
        button_texture_red(Texture("./res/textures/exit.png")),
        button_texture_green(Texture("./res/textures/start.png")),
        sushi_texture(Texture("./res/textures/sushi.png")),
        button_quit(glm::vec3(0.0f, -0.1f, 0.0f), glm::vec2(0.2f, 0.1f), QuitFunc),
        button_start(glm::vec3(0.0f, 0.1f, 0.0f), glm::vec2(0.2f, 0.1f), StartFunc)
    {
        
    }

    void onEnter() override {

        soundManager.LoadSound("beep", "./res/sounds/beep.wav");
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


        renderer.Clear(0.2f, 0.2f, 0.2f, 1.0f);
        //renderer.DrawBackground(background_texture);
        
        glm::vec3 campos = m_camera.GetLocation();

        int w, h;
        glfwGetWindowSize(&window, &w, &h);
        //float aspect;
        //if (w <= 0 || h <= 0) aspect = 1.0f;
        //else aspect = w / h;
        glm::mat4 view = m_camera.GetViewMat();
        glm::mat4 projection = m_camera.GetProjectionMat(w, h);

        //renderer.DrawBackground(background_texture);

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
        SpriteInstance quit_sprite;
        quit_sprite.position = button_quit.m_worldPosition;
        quit_sprite.size = button_quit.m_size;
        quit_sprite.rotation = 0.0f;
        quit_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        quit_sprite.texture = &button_texture_red;
        renderer.SubmitSprite(quit_sprite);
        renderer.RendBatch(view, projection);


        renderer.BeginBatchDraw(1);
        SpriteInstance start_button;
        start_button.position = button_start.m_worldPosition;
        start_button.size = button_start.m_size;
        start_button.rotation = 0.0f;
        start_button.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        start_button.texture = &button_texture_green;
        renderer.SubmitSprite(start_button);
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

        if (button_quit.IsMouseOverButton(view, proj, glm::vec2(mouseX, mouseY), w, h)) {
            soundManager.PlaySound("beep");
            button_quit.click();
        }

        if (button_start.IsMouseOverButton(view, proj, glm::vec2(mouseX, mouseY), w, h)) {
            soundManager.PlaySound("beep");
            button_start.click();
        }

        if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS || shutdown)
            glfwSetWindowShouldClose(&window, true);

    }
};
