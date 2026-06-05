#pragma once

#include "Engine.hpp"

#include "GameScene.hpp"
#include "PointLight.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <vector>

class GardenScene : public GameScene {

public:
  SimpleSoundManager &sound_manager;

  // Textures
  Texture m_ceiling_texture;
  Texture m_wall_texture;
  Texture m_floor_texture;
  Texture m_sushi_texture;
  
  Texture m_gravel_texture;
  Texture m_button_texture;
  Texture m_door_texture;

  // Sprites
  std::vector<SpriteInstance> m_static_sprites;
  SpriteInstance m_sushi_observer;

  SpriteInstance m_plot;

  PointLight m_task_light;

  // Door animation
  int m_door_index = -1;
  bool m_door_opening = false;
  float m_door_open_progress = 0.0f;

  GardenScene()
      : sound_manager(SimpleSoundManager::Instance()),
        m_ceiling_texture("./res/textures/plaster_ceiling.png"),
        m_wall_texture("./res/textures/concrete_wall.png"),
        m_floor_texture("./res/textures/gravel_floor.png"),
        m_sushi_texture("./res/textures/sushi.png"),
        m_gravel_texture("./res/textures/rock.png"),
        m_button_texture("./res/textures/button_red.png"),
        m_door_texture("./res/textures/metal_door.png") {}

  void onEnter(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_controller.Init(window);

    sound_manager.Initialize();
    sound_manager.LoadSound("background_noise",
                            "./res/sounds/ambient-noise.ogg");

    sound_manager.PlayBackgroundMusic("background_noise");

    // room variables
    float room_size = 10.0f;
    float room_height = 3.0f;
    float sushi_size = 2.0f;
    float player_height = 1.6f;

    float half_room_size = room_size / 2;
    float half_room_height = room_height / 2;
    float half_sushi_size = sushi_size / 2;
    float half_roomsushi_size = half_sushi_size + half_room_size;


    glm::vec3 sushi_position =
        glm::vec3(0.0f, half_sushi_size, -half_room_size - half_sushi_size);
    m_camera.SetCamera(glm::vec3(-(half_room_size - half_sushi_size), player_height, 0.0f));

    glm::vec3 light_color = glm::vec3(0.85f, 0.92f, 1.0f) / 3.0f;

    glm::vec3 plot_pos = glm::vec3(0, 0.001f, 0);
    glm::vec2 plot_size = glm::vec2(sushi_size, sushi_size);

    glm::vec3 button_pos =
        glm::vec3(half_room_size - 0.001f, player_height, half_room_height);
    glm::vec2 button_size = glm::vec2(half_sushi_size / 2, half_sushi_size / 2);
    glm::mat4 button_rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));

    // test room
    AddLight(glm::vec3(0, room_height, 0), half_room_size, light_color);

    AddFloor(glm::vec3(0.0f), glm::vec2(room_size), &m_floor_texture);
    AddCeiling(glm::vec3(0.0f, room_height, 0.0f), glm::vec2(room_size),
               &m_ceiling_texture);

    AddWall(glm::vec3(0.0f, half_room_height / 2, -half_room_size),
            glm::vec2(room_size, half_room_height), &m_wall_texture);
    AddGlassRotated(
        glm::vec3(0.0f, 3 * (half_room_height / 2), -half_room_size),
        glm::vec2(room_size, half_room_height), 0.0f, 0.2f, &m_wall_texture);
    AddWall(glm::vec3(0.0f, half_room_height, half_room_size),
            glm::vec2(room_size, room_height), &m_wall_texture);
    AddWallRotated(glm::vec3(-half_room_size, half_room_height,
                             -((half_room_size / 2) + (half_sushi_size / 2))),
                   glm::vec2(half_room_size - half_sushi_size, room_height),
                   90.0f, &m_wall_texture);
    AddWallRotated(glm::vec3(-half_room_size, half_room_height,
                             (half_room_size / 2) + (half_sushi_size / 2)),
                   glm::vec2(half_room_size - half_sushi_size, room_height),
                   90.0f, &m_wall_texture);
    AddWallRotated(glm::vec3(-half_room_size, half_room_height, 0.0f),
                   glm::vec2(sushi_size, room_height), 90.0f, &m_door_texture);
    AddWallRotated(glm::vec3(half_room_size, half_room_height,
                             -((half_room_size / 2) + (half_sushi_size / 2))),
                   glm::vec2(half_room_size - half_sushi_size, room_height),
                   90.0f, &m_wall_texture);
    AddWallRotated(glm::vec3(half_room_size, half_room_height,
                             (half_room_size / 2) + (half_sushi_size / 2)),
                   glm::vec2(half_room_size - half_sushi_size, room_height),
                   90.0f, &m_wall_texture);

    m_door_index = m_surfaces.size();
    AddWallRotated(glm::vec3(half_room_size, half_room_height, 0.0f),
                   glm::vec2(sushi_size, room_height), 90.0f, &m_door_texture);

    // observation room
    AddLight(glm::vec3(sushi_position.x, 0, sushi_position.z),
             half_sushi_size, light_color);
    AddWallRotated(
        glm::vec3(half_room_size, half_room_height, -half_roomsushi_size),
        glm::vec2(sushi_size, room_height), 90.0f, &m_wall_texture);
    AddWallRotated(
        glm::vec3(-half_room_size, half_room_height, -half_roomsushi_size),
        glm::vec2(sushi_size, room_height), 90.0f, &m_wall_texture);
    AddWall(glm::vec3(0.0f, half_room_height, -(half_room_size + sushi_size)),
            glm::vec2(room_size, room_height), &m_wall_texture);

    AddFloor(glm::vec3(0.0f, 0.0f, -half_roomsushi_size),
             glm::vec2(room_size, sushi_size), &m_floor_texture);
    AddCeiling(glm::vec3(0.0f, room_height, -half_roomsushi_size),
               glm::vec2(room_size, sushi_size), &m_ceiling_texture);

    // Sushi
    m_sushi_observer.texture = &m_sushi_texture;
    m_sushi_observer.size = glm::vec2(sushi_size, sushi_size);
    m_sushi_observer.color = glm::vec4(1.0f);
    m_sushi_observer.position = sushi_position;


    // Plot
      m_plot.texture = &m_gravel_texture;
      m_plot.size = plot_size;
      m_plot.color = glm::vec4(0.66f, 0.60f, 0.50f, 1.0f);
      m_plot.position = plot_pos;
      m_plot.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

    // --- Triggers ---
    m_task_light.position = plot_pos;
    m_task_light.color = light_color;
    m_task_light.radius = 0.5f;
  }

  void onExit(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  Scene *update(GLFWwindow &window, float delta) override {
    m_notification_manager.Update(delta);

    // Animate door sliding open
    if (m_door_opening && m_door_index >= 0 &&
        m_door_index < (int)m_surfaces.size()) {
      Surface &door = m_surfaces[m_door_index];
      float slide_speed = 1.0f;

      glm::vec3 slide_dir = glm::normalize(glm::vec3(door.rotation[1]));
      door.position += slide_dir * slide_speed * delta;
      m_door_open_progress += slide_speed * delta;

      if (m_door_open_progress >= door.size.y - (door.size.y * 0.1f)) {
        // m_surfaces.erase(m_surfaces.begin() + m_door_index);
        // m_door_index = -1;
        m_door_opening = false;
      }
    }

    return nullptr;
  }

  void handleInput(GLFWwindow &window, float delta) override {
    HandleCommonInput(window, delta);
  }

  void render(GLFWwindow &window, Renderer &renderer) override {

    renderer.Clear(0.05f, 0.05f, 0.05f, 1.0f);
    renderer.BeginBatchDraw(30, 10);

    std::vector<PointLight> lights = m_lights;
    lights.push_back(m_task_light);
    renderer.SetLights(lights, 0.05f);

    SetupRenderingObjects(renderer);

    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    glm::mat4 view = m_camera.GetViewMat();
    glm::mat4 projection = m_camera.GetProjectionMat(w, h);
    glm::vec3 campos = m_camera.GetLocation();

    // Sushi observer billboard
    glm::mat4 billboard = glm::transpose(glm::mat4(glm::mat3(view)));
    m_sushi_observer.model_mat = billboard;
    renderer.SubmitTransparentSprite(m_sushi_observer);

    for (const auto &sprite : m_static_sprites) {
      renderer.SubmitTransparentSprite(sprite);
    }

    renderer.SubmitTransparentSprite(m_plot);

    renderer.RendBatch(view, projection, campos, 0.05f);

    // HUD: current task instruction
    const char *task_text = "Just be a Sushi!";


    ImGui::SetNextWindowPos(
        ImVec2(w * 0.75f, h - 40.0f), ImGuiCond_Always,
        // ImGui::SetNextWindowPos(ImVec2(50.0f, h - 20.0f), ImGuiCond_Always,
        ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##task", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%s", task_text);
    ImGui::SetWindowFontScale(2.0f);
    ImGui::End();

    m_notification_manager.Render(w, h);
  }
};
