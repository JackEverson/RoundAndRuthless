#pragma once

#include "Audio.hpp"
#include "Engine.hpp"
#include "GLFW/glfw3.h"
#include "Scene.hpp"
#include "glm/ext/vector_float3.hpp"
#include <print>

class TestScene : public Scene {

public:
  SimpleSoundManager &soundManager;
  Camera m_camera;

  Texture sushi_texture;
  SpriteInstance sushi_sprite;

  float m_timer = 0.0f;

  float m_speed = 5.0f;

  double m_mouse_sensitivity = 0.05;
  bool left_click_before = false;
  double m_lastMouseX;
  double m_lastMouseY;

  TestScene()
      : soundManager(SimpleSoundManager::Instance()),
        sushi_texture(Texture("./res/textures/sushi.png")) {}

  void onEnter(GLFWwindow &window) override {

    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double x, y;
    glfwGetCursorPos(&window, &x, &y);

    m_lastMouseX = x;
    m_lastMouseY = y;

    soundManager.LoadSound("beep", "./res/sounds/beep.wav");

    sushi_sprite.position = glm::vec3(0.0f, 0.0f, -0.0f);
    sushi_sprite.size = glm::vec2(1.0f, 1.0f);
    sushi_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    sushi_sprite.texture = &sushi_texture;
  }

  void onExit(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  };

  Scene *update(GLFWwindow &window, float delta) override {
    m_timer += delta;

    static const float cycle_time = 0.75f;

    if (m_timer >= cycle_time && m_timer < cycle_time * 2) {
      sushi_sprite.size = glm::vec2(1.2f, 0.9f);
    } else if (m_timer > cycle_time * 2) {
      sushi_sprite.size = glm::vec2(1.0f, 1.0f);
      m_timer = 0.0f;
    }

    return nullptr;
  }

  void handleInput(GLFWwindow &window, float delta) override {
    glfwPollEvents();

    bool left_click = false;
    if (!left_click_before &&
        glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      left_click = true;
    }

    auto flat_forward = m_camera.GetForward() * glm::vec3(1, 0, 1);
    auto flat_right = m_camera.GetRight() * glm::vec3(1, 0, 1);

    if (GLFW_PRESS == glfwGetKey(&window, GLFW_KEY_W))
      m_camera.ShiftCamera(flat_forward * m_speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS)
      m_camera.ShiftCamera(-flat_forward * m_speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS)
      m_camera.ShiftCamera(-flat_right * m_speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS)
      m_camera.ShiftCamera(flat_right * m_speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_SPACE) == GLFW_PRESS)
      m_camera.ShiftCamera(glm::vec3(0, 1, 0) * m_speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_C) == GLFW_PRESS)
      m_camera.ShiftCamera(glm::vec3(0, -1, 0) * m_speed * delta);

    // Mouse look
    double mouseX, mouseY;
    glfwGetCursorPos(&window, &mouseX, &mouseY);

    double deltaX = (mouseX - m_lastMouseX) * m_mouse_sensitivity;
    double deltaY = (mouseY - m_lastMouseY) * -m_mouse_sensitivity;

    m_camera.ShiftRotation(deltaX, deltaY);

    // set left click before
    if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
      left_click_before = true;
    else
      left_click_before = false;

    // int w, h;
    // glfwGetWindowSize(&window, &w, &h);

    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(&window, true);

    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;
  }

  void render(GLFWwindow &window, Renderer &renderer) override {

    renderer.Clear(0.2f, 0.2f, 0.2f, 1.0f);

    // glm::vec3 campos = m_camera.GetLocation();

    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    glm::mat4 view = m_camera.GetViewMat();
    glm::mat4 projection = m_camera.GetProjectionMat(w, h);

    renderer.BeginBatchDraw(1);
    renderer.SubmitSprite(sushi_sprite);
    renderer.RendBatch(view, projection);
  }
};
