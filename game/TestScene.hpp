#pragma once

// #include "glm/ext/vector_float3.hpp"
#include <print>
#include <vector>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

#include "Audio.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Surface.hpp"
#include "Texture.hpp"
#include "FPSController.hpp"

class TestScene : public Scene
{

public:
  SimpleSoundManager &soundManager;
  Camera m_camera;
  FPSController m_controller;

  Texture m_sushi_texture;
  SpriteInstance m_sushi_sprite;

  Texture m_floor_texture;
  Texture m_wall_texture;

  float m_timer = 0.0f;

  std::vector<Surface> m_surfaces;

  TestScene()
      : soundManager(SimpleSoundManager::Instance()),
        m_sushi_texture(Texture("./res/textures/sushi.png")),
        m_floor_texture(Texture("./res/textures/gravel_floor.png")),
        m_wall_texture(Texture("./res/textures/concrete_wall.png")),
        m_controller(m_camera)
  {
  }

  void onEnter(GLFWwindow &window) override
  {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    soundManager.LoadSound("beep", "./res/sounds/beep.wav");

    m_controller.Init(window);

    m_sushi_sprite.position = glm::vec3(0.0f, 1.0f, 0.0f);
    m_sushi_sprite.size = glm::vec2(2.0f, 2.0f);
    m_sushi_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_sushi_sprite.texture = &m_sushi_texture;

    Surface floor;
    floor.type = SurfaceType::Floor;
    floor.texture = &m_floor_texture;
    floor.size = glm::vec2(10.0f, 10.0f);
    floor.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    m_surfaces.push_back(floor);

    Surface wall_front;
    wall_front.type = SurfaceType::Wall;
    wall_front.texture = &m_wall_texture;
    wall_front.size = glm::vec2(10.0f, 3.0f);
    wall_front.position = glm::vec3(0.0f, 1.5f, -5.0f);
    m_surfaces.push_back(wall_front);

    Surface wall_back;
    wall_back.type = SurfaceType::Wall;
    wall_back.texture = &m_wall_texture;
    wall_back.size = glm::vec2(10.0f, 3.0f);
    wall_back.position = glm::vec3(0.0f, 1.5f, 5.0f);
    m_surfaces.push_back(wall_back);

    Surface wall_left;
    wall_left.type = SurfaceType::Wall;
    wall_left.texture = &m_wall_texture;
    wall_left.size = glm::vec2(10.0f, 3.0f);
    wall_left.position = glm::vec3(-5.0f, 1.5f, 0.0f);
    wall_left.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
    m_surfaces.push_back(wall_left);

    Surface wall_right;
    wall_right.type = SurfaceType::Wall;
    wall_right.texture = &m_wall_texture;
    wall_right.size = glm::vec2(10.0f, 3.0f);
    wall_right.position = glm::vec3(5.0f, 1.5f, 0.0f);
    wall_right.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
    m_surfaces.push_back(wall_right);
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

    m_controller.HandleInput(window, delta);
    m_controller.ResolveCollisions(m_surfaces);

    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(&window, true);
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
