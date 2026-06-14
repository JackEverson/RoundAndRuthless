#pragma once

#include "Engine.hpp"
#include "Field.hpp"
#include "GardenRoom.hpp"

#include "GLFW/glfw3.h"
#include "Plants.hpp"
#include "PointLight.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Tile.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

#include <cstddef>
#include <string>
#include <vector>


class GardenScene : public GardenRoom {

private:
  SimpleSoundManager &sound_manager;

  // Textures
  Texture m_ceiling_texture;
  Texture m_wall_texture;
  Texture m_floor_texture;
  Texture m_sushi_texture;

  Texture m_soil_texture;
  Texture m_rock_texture;
  Texture m_till_texture;
  Texture m_button_texture;
  Texture m_door_texture;
  Texture m_radish_texture;

  // Sprites
  std::vector<SpriteInstance> m_static_sprites;
  SpriteInstance m_sushi_observer;

  // menu
  enum class MenuMode { None, Plant, Tend };
  MenuMode m_menu_mode = MenuMode::None;
  Tile *m_menu_tile = nullptr;
  float m_font_size = 2.0f;

  // field
  std::vector<PlantDef> m_plant_defs;
  PointLight m_task_light;
  Field m_field;

  // progression
  int m_day = 1;
  int m_biomass = 0;
  bool m_sleep_held = false;

  // player
  int m_max_energy = 100;
  int m_energy = m_max_energy;
  enum class Tool { Shovel, Hoe, WateringCan, SeedPacket, None };
  Tool m_tool = Tool::None;

  const int WATER_COST = 1;
  const int CLEAR_COST = 1;
  const int TILL_COST = 1;
  const float FLOOR_TILE_SIZE = 10.0f;

  // Door animation
  int m_door_index = -1;
  bool m_door_opening = false;
  float m_door_open_progress = 0.0f;

public:
  GardenScene()
      : sound_manager(SimpleSoundManager::Instance()),
        m_ceiling_texture("./res/textures/plaster_ceiling.png"),
        m_wall_texture("./res/textures/concrete_wall.png"),
        m_floor_texture("./res/textures/gravel_floor.png"),
        m_sushi_texture("./res/textures/sushi.png"),
        m_soil_texture("./res/textures/gravel_floor.png"),
        m_rock_texture("./res/textures/rock.png"),
        m_till_texture("./res/textures/hole.png"),
        m_button_texture("./res/textures/button_red.png"),
        m_door_texture("./res/textures/metal_door.png"),
        m_radish_texture("./res/textures/radish.png"),
        m_field(glm::vec3(0.0f, 0, 0.0f), 3, 3, 1.0f, &m_soil_texture,
                &m_rock_texture, &m_till_texture) {}

  void onEnter(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_controller.Init(window);

    sound_manager.Initialize();
    sound_manager.LoadSound("background_noise",
                            "./res/sounds/ambient-noise.ogg");

    sound_manager.PlayBackgroundMusic("background_noise");

    // room variables
    float player_height = 1.6f;

    float sushi_size = 2.0f;
    float half_sushi_size = sushi_size / 2;

    glm::vec3 sushi_position =
        glm::vec3(0.0f, half_sushi_size, -FLOOR_TILE_SIZE);
    m_camera.SetCamera(glm::vec3(0.0f, player_height, 0.0f));

    // lights
    glm::vec3 light_color = glm::vec3(0.85f, 0.92f, 1.0f) / 3.0f;
    AddLight(glm::vec3(sushi_position.x, 0, sushi_position.z), half_sushi_size,
             light_color);
    AddLight(glm::vec3(0), FLOOR_TILE_SIZE / 2, light_color);

    m_task_light.color = glm::vec3(1.0, 0.8, 0.8);
    m_task_light.position = glm::vec3(sushi_position.x, 1, sushi_position.z);
    m_task_light.radius = half_sushi_size;

    AddFloor(glm::vec3(0.0f), glm::vec2(FLOOR_TILE_SIZE), &m_floor_texture);

    // Sushi
    m_sushi_observer.texture = &m_sushi_texture;
    m_sushi_observer.size = glm::vec2(sushi_size, sushi_size);
    m_sushi_observer.color = glm::vec4(1.0f);
    m_sushi_observer.position = sushi_position;

    // plots
    m_plant_defs.push_back(Radish(&m_radish_texture));
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

    if (m_controller.InteractionHeld()) {
      glm::vec3 campos = m_camera.GetLocation();
      glm::vec3 forward = m_camera.GetForward();
      auto tile = m_field.TileAtRay(campos, forward);

      if (tile)
        UseToolOn(*tile);
    }

    if (m_menu_tile) {
      if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        CloseMenu();
      } else if (m_menu_mode == MenuMode::Plant) {
        for (size_t n = 0; n < m_plant_defs.size(); n++)
          if (glfwGetKey(&window, GLFW_KEY_1 + n) == GLFW_PRESS) {
            m_menu_tile->Plant(&m_plant_defs[n]);
            CloseMenu();
            break;
          } else if (m_menu_mode == MenuMode::Tend) {
            if (glfwGetKey(&window, GLFW_KEY_1 + n) == GLFW_PRESS) {
              m_menu_tile->PullUp();
              CloseMenu();
              break;
            }
          }
      }
    } else {
      if (glfwGetKey(&window, GLFW_KEY_1) == GLFW_PRESS)
        m_tool = Tool::Shovel;
      if (glfwGetKey(&window, GLFW_KEY_2) == GLFW_PRESS)
        m_tool = Tool::Hoe;
      if (glfwGetKey(&window, GLFW_KEY_3) == GLFW_PRESS)
        m_tool = Tool::WateringCan;
      if (glfwGetKey(&window, GLFW_KEY_4) == GLFW_PRESS)
        m_tool = Tool::SeedPacket;
      if (glfwGetKey(&window, GLFW_KEY_5) == GLFW_PRESS)
        m_tool = Tool::None;
    }

    bool s = glfwGetKey(&window, GLFW_KEY_K) == GLFW_PRESS;
    if (s && !m_sleep_held)
      Sleep();
    m_sleep_held = s;
  }

  void render(GLFWwindow &window, Renderer &renderer) override {
    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    if (w == 0 || h == 0)
      return;

    renderer.Clear(0.05f, 0.05f, 0.05f, 1.0f);
    renderer.BeginBatchDraw(30, 10);

    std::vector<PointLight> lights = m_lights;
    lights.push_back(m_task_light);

    renderer.SetLights(lights, 0.05f);

    SetupRenderingObjects(renderer);

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

    m_field.Render(renderer, campos);

    renderer.RendBatch(view, projection, campos, 0.05f);

    // HUD: current task instruction
    const char *task_text = "Just cooperate and hand over the liver!";

    ImGui::SetNextWindowPos(ImVec2(w * 0.75f, h - 40.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##task", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%s", task_text);
    ImGui::SetWindowFontScale(m_font_size);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(w * 0.15f, h - 125.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##hud", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Tool: %s\nDay: %d\nBiomass: %d g\nEnergy: %d/%d",
                GetToolName(m_tool), m_day, m_biomass, m_energy, m_max_energy);
    ImGui::SetWindowFontScale(m_font_size);
    ImGui::End();

    // rendering planting menu
    if (m_menu_mode != MenuMode::None) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.5f));
      ImGui::Begin("##plant", nullptr,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                       ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SetWindowFontScale(m_font_size);

      if (m_menu_mode == MenuMode::Plant) {
        ImGui::Text("Plant what?");
        for (int n = 0; n < (int)m_plant_defs.size(); n++)
          ImGui::Text("[%d] %s", n + 1, m_plant_defs[n].name.c_str());
      } else if (m_menu_mode == MenuMode::Tend) {
        ImGui::Text("[1] Pull up?");
      }
      ImGui::Text("[ESCAPE] Cancel");
      ImGui::End();
    }

    m_notification_manager.Render(w, h);
  }

  void Sleep() {
    m_day++;
    m_energy = m_max_energy;
    m_field.Advance();
  }

  bool Spend(int cost) {
    if (m_energy < cost) {
      m_notification_manager.Push("Too tired", 1.5f);
      return false;
    }
    m_energy -= cost;
    return true;
  }

  void UseToolOn(Tile &t) {
    switch (m_tool) {
    case Tool::Shovel:
      if (t.IsGrowing()) {
        m_menu_tile = &t;
        m_menu_mode = MenuMode::Tend;
      } else if (t.IsRefuse() && Spend(CLEAR_COST))
        t.Clear();
      break;

    case Tool::Hoe:
      if (t.IsEmpty() && Spend(TILL_COST))
        t.Till();
      break;
    case Tool::WateringCan:
      if (t.IsGrowing() && !t.IsWatered() && Spend(WATER_COST))
        t.Water();
      break;
    case Tool::None:
      if (t.IsRipe()) {
        m_biomass += t.Harvest();
      }
      break;
    case Tool::SeedPacket:
      if (t.IsTilled()) {
        m_menu_tile = &t;
        m_menu_mode = MenuMode::Plant;
      }
      break;
    }
  }

  void CloseMenu() {
    m_menu_tile = nullptr;
    m_menu_mode = MenuMode::None;
  }

  const char *GetToolName(Tool t) {
    switch (t) {
    case Tool::Shovel:
      return "Shovel";
    case Tool::Hoe:
      return "Hoe";
    case Tool::WateringCan:
      return "Watering Can";
    case Tool::SeedPacket:
      return "Seed Packet";
    case Tool::None:
      return "Hands";
    }
    return "?";
  }
};
