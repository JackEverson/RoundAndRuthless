#pragma once

// #include "glm/ext/vector_float3.hpp"
#include <print>
#include <vector>

#include "Audio.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Surface.hpp"
#include "Texture.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

class TestScene : public Scene
{

public:
  SimpleSoundManager &soundManager;
  Camera m_camera;

  Texture m_sushi_texture;
  SpriteInstance m_sushi_sprite;

  Texture m_floor_texture;

  float m_timer = 0.0f;

  float m_speed = 5.0f;

  double m_mouse_sensitivity = 0.05;
  bool m_left_click_before = false;
  double m_lastMouseX;
  double m_lastMouseY;

  std::vector<Surface> m_surfaces;

  TestScene()
      : soundManager(SimpleSoundManager::Instance()),
        m_sushi_texture(Texture("./res/textures/sushi.png")),
        m_floor_texture(Texture("./res/textures/gravel_floor.png")) {}

  void onEnter(GLFWwindow &window) override
  {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double x, y;
    glfwGetCursorPos(&window, &x, &y);

    m_lastMouseX = x;
    m_lastMouseY = y;

    soundManager.LoadSound("beep", "./res/sounds/beep.wav");

    m_sushi_sprite.position = glm::vec3(0.0f, 1.0f, 0.0f);
    m_sushi_sprite.size = glm::vec2(2.0f, 2.0f);
    m_sushi_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_sushi_sprite.texture = &m_sushi_texture;

    Surface floor;
    floor.type = SurfaceType::Floor;
    floor.texture = &m_floor_texture;
    floor.size = glm::vec2(10.0f, 10.0f);
    floor.rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

    m_surfaces.push_back(floor);
  }

  void onExit(GLFWwindow &window) override
  {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  Scene *update(GLFWwindow &window, float delta) override
  {
    m_timer += delta;

    static const float cycle_time = 0.75f;

    if (m_timer >= cycle_time && m_timer < cycle_time * 2)
    {
      m_sushi_sprite.size = glm::vec2(2.4f, 1.8f);
      m_sushi_sprite.position = glm::vec3(0.0f, 0.9f, 0.0f);
    }
    else if (m_timer > cycle_time * 2)
    {
      m_sushi_sprite.size = glm::vec2(2.0f, 2.0f);
      m_sushi_sprite.position = glm::vec3(0.0f, 1.0f, 0.0f);
      m_timer = 0.0f;
    }

    return nullptr;
  }

  void handleInput(GLFWwindow &window, float delta) override
  {
    glfwPollEvents();


    bool left_click = false;
    if (!m_left_click_before &&
        glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
      left_click = true;
    }

    if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) m_left_click_before = true;
    else m_left_click_before = false;

    if (left_click)
    {
      soundManager.PlaySound("beep");
    }


    float speed = m_speed;
    if (glfwGetKey(&window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(&window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS){
      speed *= 5;
    }

    auto flat_forward = m_camera.GetForward() * glm::vec3(1, 0, 1);
    auto flat_right = m_camera.GetRight() * glm::vec3(1, 0, 1);

    if (GLFW_PRESS == glfwGetKey(&window, GLFW_KEY_W))
      m_camera.ShiftCamera(flat_forward * speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_S) == GLFW_PRESS)
      m_camera.ShiftCamera(-flat_forward * speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_A) == GLFW_PRESS)
      m_camera.ShiftCamera(-flat_right * speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS)
      m_camera.ShiftCamera(flat_right * speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_SPACE) == GLFW_PRESS)
      m_camera.ShiftCamera(glm::vec3(0, 1, 0) * speed * delta);
    if (glfwGetKey(&window, GLFW_KEY_C) == GLFW_PRESS)
      m_camera.ShiftCamera(glm::vec3(0, -1, 0) * speed * delta);

    // Mouse look
    double mouseX, mouseY;
    glfwGetCursorPos(&window, &mouseX, &mouseY);

    double deltaX = (mouseX - m_lastMouseX) * m_mouse_sensitivity;
    double deltaY = (mouseY - m_lastMouseY) * -m_mouse_sensitivity;

    m_camera.ShiftRotation(deltaX, deltaY);

    // int w, h;
    // glfwGetWindowSize(&window, &w, &h);

    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(&window, true);

    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;
  }

  void render(GLFWwindow &window, Renderer &renderer) override
  {
    renderer.Clear(0.2f, 0.2f, 0.2f, 1.0f);
    renderer.BeginBatchDraw(10);

    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    glm::mat4 view = m_camera.GetViewMat();
    glm::mat4 projection = m_camera.GetProjectionMat(w, h);
    glm::vec3 campos = m_camera.GetLocation();

    for (auto surface : m_surfaces)
    {
      SpriteInstance sprite;
      sprite.position = surface.position;
      sprite.texture = surface.texture;
      sprite.size = surface.size;
      sprite.model_mat = surface.rotation;

      renderer.SubmitSprite(sprite);
    }

    renderer.SubmitSprite(m_sushi_sprite);

    renderer.RendBatch(view, projection, campos, 0.3f);
  }
};
