#pragma once

#include "Scene.hpp"
#include "gl_debug.hpp"
#include "Audio.hpp"

class GameLoopScene : public Scene {

public:
    
    ClickCounter m_clickCounter;
    Camera m_camera;
    Approacher m_approacher;
    SimpleSoundManager& soundManager;

    Texture background_texture;
    Texture approacher_texture;
    Texture floor_texture;
    Texture wall_texture;
    Texture ceiling_texture;
    bool m_first_click = false;

    GameLoopScene() :
    soundManager(SimpleSoundManager::Instance()),
        m_approacher(Approacher(30.0f, 1.0f)),
        approacher_texture(Texture("./res/textures/sushi.png")),
        background_texture(Texture("./res/textures/background.png")),
        floor_texture(Texture("./res/textures/gravel_floor.png")),
        wall_texture(Texture("./res/textures/concrete_wall.png")),
        ceiling_texture(Texture("./res/textures/plaster_ceiling.png"))
    {

    }

    void onEnter() override {

        soundManager.LoadSound("click", "./res/sounds/click.wav");
        soundManager.LoadSound("cave", "./res/sounds/cave.ogg");

        soundManager.PlayBackgroundMusic("cave", 1.0f); 
    }
    
    void onExit() override {

    }

    Scene* update() override {

        m_approacher.Step();

        return nullptr;
    }

    void render(GLFWwindow& window, Renderer& renderer) override {


        renderer.Clear(0.1f, 0.1f, 0.1f, 1.0f);

        float clicks = (float)m_clickCounter.GetClicks();
        
        glm::vec3 campos = m_camera.GetLocation();


        int w, h;
        glfwGetWindowSize(&window, &w, &h);
        float aspect = w / h;
        glm::mat4 view = m_camera.GetViewMat();
        glm::mat4 projection = m_camera.GetProjectionMat(w, h);

        renderer.DrawHallway(view, projection, floor_texture, wall_texture, ceiling_texture);

        float jiggle_size_x = 0.01f;
        float jiggle_size_y = 0.001f;
        float jiggle_speed = 20.0f;

        float jiggle_x = glm::cos(m_approacher.m_distanceAway * jiggle_speed) * -jiggle_size_x;
        float jiggle_y = glm::sin(m_approacher.m_distanceAway * jiggle_speed) * jiggle_size_y;

        float darkness = 1.0f - 0.8f * (m_approacher.m_distanceAway / 30.0f);

        renderer.BeginBatchDraw(4);
        SpriteInstance static_sprite;
        static_sprite.position = glm::vec3(jiggle_x, jiggle_y, -m_approacher.m_distanceAway);
        static_sprite.size = glm::vec2(0.6f, 0.4f);
        static_sprite.rotation = 90.0f;
        static_sprite.color = glm::vec4(darkness, darkness, darkness, 1.0f);
        static_sprite.texture = &approacher_texture;
        renderer.SubmitSprite(static_sprite);

        //SpriteInstance clicker_sprite;
        //clicker_sprite.position = glm::vec3(0.0f, 0.0f, -m_approacher->m_distanceAway);
        //if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        //    clicker_sprite.color = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
        //}
        //else {
        //    static_sprite.color = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
        //}
        //clicker_sprite.size = glm::vec2(0.6f, 0.4f);
        //clicker_sprite.rotation = 0.0f;
        //clicker_sprite.texture = approacher_texture;
        //m_renderer->SubmitSprite(clicker_sprite);

        renderer.RendBatch(view, projection);

        GLCall(glfwSwapBuffers(&window));
    }


    
    void handleInput(GLFWwindow& window) {


        GLCall(glfwPollEvents());

        if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(&window, true);

        if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(&window, &xpos, &ypos);

            int w, h;
            glfwGetWindowSize(&window, &w, &h);
            float aspect_ratio = (float)w / (float)h;

            glm::vec3 cam_loc = m_camera.GetLocation();

            double nxpos = (xpos / w + cam_loc.x);
            double nypos = (1 - (ypos / h)) + cam_loc.y;

            if (m_first_click == true) {
                m_first_click = false;
                m_clickCounter.click();
                soundManager.PlaySound("click");
            }
        }
        else {
            m_first_click = true;
        }
        if (glfwGetKey(&window, GLFW_KEY_E)) {
            m_clickCounter.click();
            soundManager.PlaySound("click");
        }

        float sensitivity = 1.0f;
        if (glfwGetKey(&window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            sensitivity = 1.5f;
        }
        float move = 0.01f * sensitivity;

        if (glfwGetKey(&window, GLFW_KEY_W) == GLFW_PRESS) {
            // locy += 0.001f;
            m_camera.ShiftCamera(0.0f, move, 0.0f);
        }
        if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS) {
            // locy -= 0.001f;
            m_camera.ShiftCamera(0.0f, -move, 0.0f);
        }
        if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS) {
            // locx -= 0.001f;
            m_camera.ShiftCamera(-move, 0.0f, 0.0f);
        }
        if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS) {
            // locx += 0.001f;
            m_camera.ShiftCamera(move, 0.0f, 0.0f);
        }
        if (glfwGetKey(&window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            m_camera.ShiftCamera(0.0f, 0.0f, -move);
        }
        if (glfwGetKey(&window, GLFW_KEY_C) == GLFW_PRESS) {
            m_camera.ShiftCamera(0.0f, 0.0f, move);
        }

    }
};
