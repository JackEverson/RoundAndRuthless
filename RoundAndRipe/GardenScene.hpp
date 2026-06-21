#pragma once

#include "Field.hpp"
#include "GardenRoom.hpp"
#include "Plants.hpp"
#include "PointLight.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Tile.hpp"

#include "GLFW/glfw3.h"
#include "TriggerVolume.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

#include <cstddef>
#include <string>
#include <vector>

class GardenScene : public GardenRoom {

private:
  SimpleSoundManager &sound_manager;

  // Textures
  Texture m_wall_texture;
  Texture m_floor_texture;
  
  Texture m_sushi_texture;
  Texture m_house_texture;

  Texture m_soil_texture;
  Texture m_rock_texture;
  Texture m_till_texture;
  Texture m_seeded_texture;

  Texture m_radish_texture;
  Texture m_carrot_texture;


  // enums and struct 
  enum class MenuMode { None, Tend };
  enum class Tool { Shovel, Hoe, WateringCan, SeedPacket, None };
  enum class SleepPhase { Awake, GoingDark, Waking };
  
  struct Seed {
      PlantDef def;     
      int count = 0;    
  };

  // Sprites
  std::vector<SpriteInstance> m_static_sprites;
  SpriteInstance m_sushi_observer;
  SpriteInstance m_house;

  // menu
  MenuMode m_menu_mode = MenuMode::None;
  Tile *m_menu_tile = nullptr;
  float m_font_size = 2.0f;

  // field
  PointLight m_highlight;
  bool m_show_highlight = false;
  Field m_field;

  // progression
  int m_day = 1;
  int m_biomass = 0;
  bool m_sleep_held = false;

  // player
  int m_max_energy = 100;
  int m_energy = m_max_energy;
  Tool m_tool = Tool::None;
  int m_selected_seed = -1;
  std::vector<Seed> m_seeds;


  // animation
  SleepPhase m_sleep = SleepPhase::Awake;
  float m_fade = 0.0f;                 // 0 = clear, 1 = black

  // const
  const int WATER_COST = 1;
  const int CLEAR_COST = 1;
  const int TILL_COST = 1;
  const float FLOOR_TILE_SIZE = 100.0f;

  const float HOUSE_SIZE = 4.0f;
  const glm::vec3 HOUSE_POS = glm::vec3(-5.0f, HOUSE_SIZE/ 2.0, -10.0f);

  const float SUSHI_SIZE = 2.0f;
  const float HALF_SUSHI_SIZE = SUSHI_SIZE / 2;

  const float PLAYER_HEIGHT = 1.6f;
  const float FADE_SPEED = 2.0f;

  

public:
  GardenScene()
      : sound_manager(SimpleSoundManager::Instance()),
        m_wall_texture("./res/textures/concrete_wall.png"),
        m_floor_texture("./res/textures/gravel_floor.png"),
        m_sushi_texture("./res/textures/sushi.png"),
        m_house_texture("./res/textures/house.png"),
        m_soil_texture("./res/textures/gravel_floor.png"),
        m_rock_texture("./res/textures/rock.png"),
        m_till_texture("./res/textures/hole.png"),
        m_seeded_texture("./res/textures/covered_hole.png"),
        m_radish_texture("./res/textures/radish.png"),
        m_carrot_texture("./res/textures/carrot.png"),
        m_field(glm::vec3(-5.0f, 0.0f, -5.0f), 10, 10, 1.0f, &m_soil_texture,
                &m_rock_texture, &m_till_texture, &m_seeded_texture) {}

  void onEnter(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_controller.Init(window);

    sound_manager.Initialize();
    sound_manager.LoadSound("background_noise",
                            "./res/sounds/ambient-noise.ogg");

    sound_manager.PlayBackgroundMusic("background_noise");

    glm::vec3 sushi_position =
        glm::vec3(0.0f, HALF_SUSHI_SIZE, -10.0f);
    m_camera.SetCamera(glm::vec3(0.0f, PLAYER_HEIGHT, 0.0f));

    // lights
    glm::vec3 light_color = glm::vec3(0.85f, 0.92f, 1.0f) / 3.0f;
    AddLight(glm::vec3(sushi_position.x, 0, sushi_position.z), HALF_SUSHI_SIZE,
             light_color);
    AddLight(glm::vec3(0), FLOOR_TILE_SIZE / 2, light_color);

    m_highlight.color = glm::vec3(0.5);
    m_highlight.position = glm::vec3(sushi_position.x, 1, sushi_position.z);
    m_highlight.radius = 0.25f;

    AddFloor(glm::vec3(0.0f), glm::vec2(FLOOR_TILE_SIZE), &m_floor_texture);
    AddWall(glm::vec3(0.0f, 0.0f, FLOOR_TILE_SIZE / 2), glm::vec2(FLOOR_TILE_SIZE, 4.0f), &m_wall_texture);
    AddWall(glm::vec3(0.0f, 0.0f, -FLOOR_TILE_SIZE / 2), glm::vec2(FLOOR_TILE_SIZE, 4.0f), &m_wall_texture);
    AddWallRotated(glm::vec3(FLOOR_TILE_SIZE / 2, 0.0f, 0.0f), glm::vec2(FLOOR_TILE_SIZE, 4.0f), 90.0f, &m_wall_texture);
    AddWallRotated(glm::vec3(-FLOOR_TILE_SIZE / 2, 0.0f, 0.0f), glm::vec2(FLOOR_TILE_SIZE, 4.0f), 90.0f, &m_wall_texture);

    // Sushi
    m_sushi_observer.texture = &m_sushi_texture;
    m_sushi_observer.size = glm::vec2(SUSHI_SIZE * 1.2f, SUSHI_SIZE);
    m_sushi_observer.color = glm::vec4(1.0f);
    m_sushi_observer.position = sushi_position;

    // house 
    m_house.texture = &m_house_texture;
    m_house.size = glm::vec2(HOUSE_SIZE, HOUSE_SIZE);
    m_house.color = glm::vec4(1.0f);
    m_house.position = HOUSE_POS;
    m_static_sprites.push_back(m_house);

    TriggerVolume house_trigger;
    house_trigger.position = HOUSE_POS;
    house_trigger.size = glm::vec3(HOUSE_SIZE, HOUSE_SIZE, 0.10f);
    house_trigger.time_to_trigger = 2.0f;
    house_trigger.type = TriggerType::Interact;
    house_trigger.on_triggered = [this]() { StartSleep(); };
    m_triggers.push_back(house_trigger);

    // seeds
    m_seeds.push_back({Radish(&m_radish_texture), 10});
    m_seeds.push_back({Carrot(&m_carrot_texture), 10});

  }

  void onExit(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  Scene *update(GLFWwindow &window, float delta) override {
    m_notification_manager.Update(delta);

    // // sushi observer follows the player
    // glm::vec3 campos = m_camera.GetLocation();
    // glm::vec3 forward = m_camera.GetForward();
    // glm::vec3 f = glm::normalize(forward);
    // glm::vec3 p = campos + (forward * 0.01f);
    // p.y -= (PLAYER_HEIGHT / 2);
    // m_sushi_observer.position = p;


    // animate sleep
    if (m_sleep == SleepPhase::GoingDark){
      m_fade += FADE_SPEED * delta;
      if (m_fade >= 1.0f) { 
        m_fade = 1.0f; AdvanceDay(); m_sleep = SleepPhase::Waking; 
      }
    } else if (m_sleep == SleepPhase::Waking) {
        m_fade -= FADE_SPEED * delta;
        if (m_fade <= 0.0f) { m_fade = 0.0f; m_sleep = SleepPhase::Awake; }
      }

    return nullptr;
  }

  void handleInput(GLFWwindow &window, float delta) override {
    HandleCommonInput(window, delta);

    glm::vec3 campos = m_camera.GetLocation();
    glm::vec3 forward = m_camera.GetForward();
    auto tile = m_field.TileAtRay(campos, forward);

    if (tile) {
      m_highlight.position = tile->Position();
      m_show_highlight = true;
    } else {
      m_show_highlight = false;
    }

    if (m_controller.InteractionHeld()) {
      if (tile)
        UseToolOn(*tile);
    }

    if (m_menu_tile) {
      if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        CloseMenu();
      } 
      else if (m_menu_mode == MenuMode::Tend){
        if (glfwGetKey(&window, GLFW_KEY_Y) == GLFW_PRESS) {
              m_menu_tile->PullUp();
              CloseMenu();
        }
        else if (glfwGetKey(&window, GLFW_KEY_N) == GLFW_PRESS){
          CloseMenu();
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
      DebugAdvanceDay();
    m_sleep_held = s;

    // temp scroll wheel seed selection
    float wheel = ImGui::GetIO().MouseWheel;   // + up / - down, this frame
    if (wheel > 0) CycleSeed(+1);
    else if (wheel < 0) CycleSeed(-1);

  }

  void render(GLFWwindow &window, Renderer &renderer) override {
    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    if (w == 0 || h == 0)
      return;

    renderer.Clear(0.05f, 0.05f, 0.05f, 1.0f);
    renderer.BeginBatchDraw(30, 10);

    std::vector<PointLight> lights = m_lights;
    m_field.CollectLights(lights);

    if (m_show_highlight)
      lights.push_back(m_highlight);

    renderer.SetLights(lights, 0.15f);

    SetupRenderingObjects(renderer);

    glm::vec3 campos = m_camera.GetLocation();
    glm::mat4 view = m_camera.GetViewMat();
    glm::mat4 projection = m_camera.GetProjectionMat(w, h);

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


    std::string seed_text; 
    
    if (m_selected_seed == -1){
      seed_text = "none";
    }else{
      seed_text = m_seeds[m_selected_seed].def.name + " x" + std::to_string(m_seeds[m_selected_seed].count);
    }

    ImGui::SetNextWindowPos(ImVec2(w * 0.15f, h - 150.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##hud", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Tool: %s\nSeed: %s\nDay: %d\nBiomass: %d g\nEnergy: %d/%d",
                GetToolName(m_tool), seed_text.c_str(), m_day, m_biomass, m_energy, m_max_energy);
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

      if (m_menu_mode == MenuMode::Tend) {
        ImGui::Text("[Y] Pull up?");
        ImGui::Text("[N] Cancel");
      }
      ImGui::End();
    }

    // sleep screen, TODO: probably should replace this with my own rects
    if (m_fade > 0.0f) {
        ImU32 col = IM_COL32(0, 0, 0, (int)(m_fade * 255));
        ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2((float)w, (float)h), col);
    }


    m_notification_manager.Render(w, h);
  }

  void DebugAdvanceDay() {
    m_day++;
    m_energy = m_max_energy;
    m_field.Advance();
  }

  void StartSleep() { if (m_sleep == SleepPhase::Awake) m_sleep = SleepPhase::GoingDark; }

  void AdvanceDay(){
    m_day++;
    m_energy = m_max_energy;
    m_field.Advance();
    m_camera.SetCamera(m_sushi_observer.position);
  }

  bool Spend(int cost) {
    if (m_energy < cost) {
      m_notification_manager.Push("Too tired", 1.5f);
      return false;
    }
    m_energy -= cost;
    return true;
  }

  void CycleSeed(int dir) {
      int n = (int)m_seeds.size();
      int start = (m_selected_seed < 0) ? 0 : m_selected_seed;
      for (int i = 1; i <= n; i++) {
          int idx = ((start + dir * i) % n + n) % n;   // wrap, handles negatives
          if (m_seeds[idx].count > 0) { m_selected_seed = idx; return; }
      }
      m_selected_seed = -1;   // nothing in stock
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
      if (!t.IsWatered() && Spend(WATER_COST))
        t.Water();
      break;
    case Tool::None:
      if (t.IsRipe()) {
        m_biomass += t.Harvest();
      }
      break;
    case Tool::SeedPacket:
        if (t.IsTilled() && m_selected_seed >= 0 && m_seeds[m_selected_seed].count > 0) {
            t.Plant(&m_seeds[m_selected_seed].def);
            if (--m_seeds[m_selected_seed].count == 0) m_selected_seed = -1; 
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
