#pragma once

#include "Audio.hpp"
#include "Engine.hpp"
#include "FPSController.hpp"
#include "Interactable.hpp"
#include "NotificationManager.hpp"
#include "PointLight.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Surface.hpp"
#include "Texture.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

#include <cstdio>
#include <string>
#include <vector>

class TestScene : public Scene {

public:
  SimpleSoundManager &soundManager;
  NotificationManager m_notification_manager;

  Camera m_camera;
  FPSController m_controller;

  Texture m_sushi_texture;
  SpriteInstance m_sushi_sprite;
  SpriteInstance m_sushi_billboard;
  bool m_billboard_sushi = true;

  Texture m_ceiling_texture;
  Texture m_wall_texture;
  Texture m_floor_texture;

  float m_timer = 0.0f;

  std::vector<Interactable> m_interactables;
  std::vector<Surface> m_surfaces;

  bool m_tab_before = false;
  bool m_cursor_captured = true;
  float m_fog_density = 0.15f;
  float m_light_radius = 20.0f;
  float m_ambient = 0.15f;
  glm::vec3 m_light_color = glm::vec3(1.0f);

  TestScene()
      : soundManager(SimpleSoundManager::Instance()),
        m_sushi_texture(Texture("./res/textures/sushi.png")),
        m_ceiling_texture(Texture("./res/textures/plaster_ceiling.png")),
        m_wall_texture(Texture("./res/textures/concrete_wall.png")),
        m_floor_texture(Texture("./res/textures/gravel_floor.png")),
        m_controller(m_camera) {}

  void onEnter(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    soundManager.LoadSound("beep", "./res/sounds/beep.wav");

    m_controller.Init(window);

    m_sushi_sprite.position = glm::vec3(0.0f, 1.0f, 0.0f);
    m_sushi_sprite.size = glm::vec2(2.0f, 2.0f);
    m_sushi_sprite.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_sushi_sprite.texture = &m_sushi_texture;

    m_sushi_billboard.position = glm::vec3(0.0f, 1.0f, 0.0f);
    m_sushi_billboard.size = glm::vec2(2.0f, 2.0f);
    m_sushi_billboard.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_sushi_billboard.texture = &m_sushi_texture;

    static int sushi_touches = 1;

    Interactable sushi_interact;
    sushi_interact.on_interact = [this]() {
      printf("You have poked Sushi %d times\n", sushi_touches++);
      soundManager.PlaySound("beep");
      m_notification_manager.Push("SUSHI: STOP IT!!!");
      m_billboard_sushi = !m_billboard_sushi;
    };

    sushi_interact.position = glm::vec3(0.0f, 1.0f, 0.0f);
    sushi_interact.size = glm::vec3(2.0f, 2.0f, 0.1f);
    sushi_interact.interaction_distance = 2.0f;
    m_interactables.push_back(sushi_interact);

    float room_size = 20.0f;
    float room_height = 5.0f;

    Surface ceiling;
    ceiling.type = SurfaceType::Ceiling;
    ceiling.texture = &m_ceiling_texture;
    ceiling.size = glm::vec2(room_size, room_size);
    ceiling.rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    ceiling.position = glm::vec3(0.0f, room_height, 0.0f);
    m_surfaces.push_back(ceiling);

    Surface wall_front;
    wall_front.type = SurfaceType::Wall;
    wall_front.texture = &m_wall_texture;
    wall_front.size = glm::vec2(room_size, room_height);
    wall_front.position = glm::vec3(0.0f, room_height / 2, -room_size / 2);
    m_surfaces.push_back(wall_front);

    Surface wall_back;
    wall_back.type = SurfaceType::Wall;
    wall_back.texture = &m_wall_texture;
    wall_back.size = glm::vec2(room_size, room_height);
    wall_back.position = glm::vec3(0.0f, room_height / 2, room_size / 2);
    m_surfaces.push_back(wall_back);

    Surface wall_left;
    wall_left.type = SurfaceType::Wall;
    wall_left.texture = &m_wall_texture;
    wall_left.size = glm::vec2(room_size, room_height);
    wall_left.position = glm::vec3(-room_size / 2, room_height / 2, 0.0f);
    wall_left.rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
    m_surfaces.push_back(wall_left);

    Surface wall_right;
    wall_right.type = SurfaceType::Wall;
    wall_right.texture = &m_wall_texture;
    wall_right.size = glm::vec2(room_size, room_height);
    wall_right.position = glm::vec3(room_size / 2, room_height / 2, 0.0f);
    wall_right.rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
    m_surfaces.push_back(wall_right);

    Surface floor;
    floor.type = SurfaceType::Floor;
    floor.texture = &m_floor_texture;
    floor.size = glm::vec2(room_size, room_size);
    floor.rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    m_surfaces.push_back(floor);
  }

  void onExit(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  Scene *update(GLFWwindow &window, float delta) override {

    m_notification_manager.Update(delta);

    m_timer += delta;

    static const float cycle_time = 0.75f;

    if (m_timer >= cycle_time && m_timer < cycle_time * 2) {
      m_sushi_sprite.size = glm::vec2(2.4f, 1.8f);
      m_sushi_sprite.position = glm::vec3(0.0f, 0.9f, 0.0f);

      m_sushi_billboard.size = glm::vec2(2.4f, 1.8f);
      m_sushi_billboard.position = glm::vec3(0.0f, 0.9f, 0.0f);

    } else if (m_timer > cycle_time * 2) {
      m_sushi_sprite.size = glm::vec2(2.0f, 2.0f);
      m_sushi_sprite.position = glm::vec3(0.0f, 1.0f, 0.0f);
      
      m_sushi_billboard.size = glm::vec2(2.0f, 2.0f);
      m_sushi_billboard.position = glm::vec3(0.0f, 1.0f, 0.0f);
      
      m_timer = 0.0f;
    }

    return nullptr;
  }

  void handleInput(GLFWwindow &window, float delta) override {
    glfwPollEvents();

    if (m_cursor_captured)
      m_controller.HandleInput(window, delta);
    m_controller.ResolveCollisions(m_surfaces);
    m_controller.CheckInteraction(window, m_interactables);

    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(&window, true);

    bool tab_pressed = glfwGetKey(&window, GLFW_KEY_TAB) == GLFW_PRESS;

    if (!m_tab_before && tab_pressed) {
      m_cursor_captured = !m_cursor_captured;
      glfwSetInputMode(&window, GLFW_CURSOR,
                       m_cursor_captured ? GLFW_CURSOR_DISABLED
                                         : GLFW_CURSOR_NORMAL);
      m_controller.Init(window);
    }
    m_tab_before = tab_pressed;
  }

  void render(GLFWwindow &window, Renderer &renderer) override {
    renderer.Clear(0.1f, 0.1f, 0.1f, 1.0f);

    PointLight light;
    light.position = glm::vec3(0.0f, 5.0f, 0.0f);
    // light.color = glm::vec3(0.9f, 0.8f, 0.5f);
    light.color = m_light_color;
    light.radius = m_light_radius;

    std::vector<PointLight> lights;
    lights.push_back(light);
    renderer.SetLights(lights, m_ambient);

    renderer.BeginBatchDraw(10);

    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    glm::mat4 view = m_camera.GetViewMat();
    glm::mat4 projection = m_camera.GetProjectionMat(w, h);
    glm::vec3 campos = m_camera.GetLocation();

    for (auto surface : m_surfaces) {
      SpriteInstance sprite;
      sprite.position = surface.position;
      sprite.texture = surface.texture;
      sprite.size = surface.size;
      sprite.model_mat = surface.rotation;

      renderer.SubmitSprite(sprite);
    }

    if (!m_billboard_sushi) {
      renderer.SubmitSprite(m_sushi_sprite);
    } else {
      glm::mat4 billboard = glm::transpose(glm::mat4(glm::mat3(view)));
      m_sushi_billboard.model_mat = billboard;
      renderer.SubmitSprite(m_sushi_billboard);
    }

    renderer.RendBatch(view, projection, campos, m_fog_density);

    m_notification_manager.Render(w, h);

    if (!m_cursor_captured) {
      ImGui::Begin("Effects");
      ImGui::SliderFloat("Fog Density", &m_fog_density, 0.0f, 1.0f);
      ImGui::SliderFloat("Ambient Light", &m_ambient, 0.0f, 1.0f);
      ImGui::SliderFloat("Light Radius", &m_light_radius, 0.0f, 30.0f);
      ImGui::ColorEdit3("Light Color", &m_light_color.x);
      ImGui::End();
    }
  }
};
