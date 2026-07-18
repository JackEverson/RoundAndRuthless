#pragma once

#include "Engine.hpp"
#include "GardenRoom.hpp"
#include "GardenScene.hpp"
#include "SaveSystem.hpp"

#include "imgui.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>


class MenuScene : public GardenRoom {

  SimpleSoundManager &sound_manager;

  Texture m_floor_texture;
  Texture m_kidneybean_texture;
  Texture m_tomato_texture;
  Texture m_human_texture;
  Texture m_sushi_texture;

  struct MenuPlant { SpriteInstance sprite; float phase; };
  std::vector<MenuPlant> m_plants;

  GameSettings m_cfg;                // loaded on enter, saved on change
  bool m_has_save = false;
  bool m_confirm_new = false;
  bool m_start_run = false;
  bool m_show_settings = false;
  double m_time = 0.0;               // drives the crop sway

  const glm::vec4 FLOOR_COLOR = glm::vec4(0.45f, 0.44f, 0.42f, 1.0f);
  const glm::vec4 SOIL_COLOR = glm::vec4(0.58f, 0.28f, 0.20f, 1.0f);

  float GetUiScale(int h) const { return m_cfg.ui_scale * (float)h / 1440.0f; }

public:
  MenuScene()
      : sound_manager(SimpleSoundManager::Instance()),
        m_floor_texture("./res/textures/gravel_floor.png"),
        m_kidneybean_texture("./res/textures/kidneybean_bush.png"),
        m_tomato_texture("./res/textures/tomato_bush.png"),
        m_human_texture("./res/textures/human_hazsuit.png"),
        m_sushi_texture("./res/textures/sushi.png") {}

  void onEnter(GLFWwindow &window) override {
    SaveSystem::LoadSettings(SETTINGS_PATH, m_cfg);
    sound_manager.Initialize();
    sound_manager.SetMasterVolume(m_cfg.volume);
    sound_manager.LoadSound("background_noise", "./res/sounds/ambient-noise.ogg");
    sound_manager.PlayBackgroundMusic("background_noise");
    GardenEngine::SetBorderless(window, m_cfg.borderless);   // apply either way

    m_has_save = std::ifstream(SAVE_PATH).good();

    // the shot: low over the dirt, plants ahead, tenant slightly right
    m_camera.SetCamera(glm::vec3(0.0f, 0.45f, -1.0f));
    m_camera.SetRotation(-90.0f, -12.0f);   // yaw -90 = looking down -z

    AddFloor(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(40.0f), &m_floor_texture,
             FLOOR_COLOR);
    AddFloor(glm::vec3(0.0f, 0.01f, -3.5f), glm::vec2(7.0f, 4.5f),
             &m_floor_texture, SOIL_COLOR);   // the dirt patch

    AddLight(glm::vec3(0.0f, 2.0f, -3.0f), 8.0f,
             glm::vec3(0.9f, 0.8f, 0.65f) / 2.5f);   // warm key light

    // two ragged crop rows
    for (int i = 0; i < 8; i++) {
      MenuPlant p;
      float x = -2.6f + (i % 4) * 1.5f + (rand() % 40 - 20) / 100.0f;
      float z = (i < 4 ? -4.4f : -3.0f) + (rand() % 40 - 20) / 100.0f;
      float s = 0.8f + (rand() % 30) / 100.0f;
      p.sprite.texture = (i % 2 == 0) ? &m_kidneybean_texture : &m_tomato_texture;
      p.sprite.size = glm::vec2(s, s);
      p.sprite.color = glm::vec4(1.0f);
      p.sprite.position = glm::vec3(x, s / 2.0f, z);
      p.phase = (rand() % 628) / 100.0f;
      m_plants.push_back(p);
    }

    EnterSelectionMode(window);   // free cursor, input off — it's a menu
  }

  void onExit(GLFWwindow &window) override {
    SaveSystem::SaveSettings(SETTINGS_PATH, m_cfg);
  }

  Scene *update(GLFWwindow &window, float delta) override {
    m_time += delta;
    if (m_start_run)
      return new GardenScene();   // loads the save, or starts the tutorial
    return nullptr;
  }

  void handleInput(GLFWwindow &window, float delta) override {
    HandleCommonInput(window, delta);   // polls events; input stays disabled
  }

  void render(GLFWwindow &window, Renderer &renderer) override {
    int w, h;
    glfwGetWindowSize(&window, &w, &h);
    if (w == 0 || h == 0) return;
    const float ui = GetUiScale(h);

    renderer.Clear(0.07f, 0.06f, 0.08f, 1.0f);   // fixed dusk
    renderer.BeginBatchDraw(10, 30);
    renderer.SetLights(m_lights, 0.18f * m_cfg.brightness);
    SetupRenderingObjects(renderer);

    glm::vec3 campos = m_camera.GetLocation();
    auto yaw_to_cam = [&](const glm::vec3 &pos) {
      glm::vec3 to = campos - pos;
      return glm::rotate(glm::mat4(1.0f), std::atan2(to.x, to.z),
                         glm::vec3(0, 1, 0));
    };

    for (auto &p : m_plants) {
      float lean = std::sin((float)m_time * 1.5f + p.phase) * 0.06f;
      p.sprite.model_mat =
          yaw_to_cam(p.sprite.position) *
          glm::rotate(glm::mat4(1.0f), lean, glm::vec3(0, 0, 1));
      renderer.SubmitTransparentSprite(p.sprite);
    }

    // the previous tenant, leaned back 45°, Sushi perched on top of him
    SpriteInstance corpse;
    corpse.texture = &m_human_texture;
    corpse.size = glm::vec2(0.8f, 1.6f);
    corpse.color = glm::vec4(1.0f);
    corpse.position = glm::vec3(1.1f, 0.15f, -3.1f);
    corpse.model_mat =
        yaw_to_cam(corpse.position) *
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1) 
      );
    renderer.SubmitTransparentSprite(corpse);

    SpriteInstance sushi;
    sushi.texture = &m_sushi_texture;
    sushi.size = glm::vec2(0.45f);
    sushi.color = glm::vec4(1.0f);
    sushi.position = glm::vec3(1.0f, 0.25f, -2.95f);
    sushi.model_mat = yaw_to_cam(sushi.position);
    renderer.SubmitTransparentSprite(sushi);

    renderer.RendBatch(m_camera.GetViewMat(),
                       m_camera.GetProjectionMat((float)w, (float)h), campos,
                       0.03f);

    // ── the menu itself
    if (!m_show_settings) {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.0f));
      ImGui::SetNextWindowBgAlpha(0.55f);
      ImGui::Begin("##mainmenu", nullptr,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::SetWindowFontScale(ui * 3.5f);
      ImGui::Text("ROUND AND RIPE");
      ImGui::SetWindowFontScale(ui * 1.5f);
      ImGui::Separator();

      if (m_has_save) {
        if (ImGui::Button("Continue")) m_start_run = true;
      } else {
        ImGui::TextDisabled("Continue");
      }

      if (!m_confirm_new) {
        if (ImGui::Button("New Game")) {
          if (m_has_save) m_confirm_new = true;   // never nuke a run silently
          else m_start_run = true;
        }
      } else {
        ImGui::Text("Erase your existing run?");
        if (ImGui::Button("Yes, erase")) {
          std::remove(SAVE_PATH.c_str());
          m_start_run = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) m_confirm_new = false;
      }

      if (ImGui::Button("Settings")) m_show_settings = true;
      if (ImGui::Button("Quit")) glfwSetWindowShouldClose(&window, true);
      ImGui::End();
    } else {
      ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.5f), ImGuiCond_Always,
                              ImVec2(0.5f, 0.5f));
      ImGui::SetNextWindowBgAlpha(0.9f);
      ImGui::Begin("Settings", nullptr,
                   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
      ImGui::SetWindowFontScale(ui);

      if (ImGui::SliderFloat("Volume", &m_cfg.volume, 0.0f, 1.0f))
        sound_manager.SetMasterVolume(m_cfg.volume);
      ImGui::SliderFloat("Brightness", &m_cfg.brightness, 0.3f, 2.0f);
      ImGui::SliderFloat("Mouse Sensitivity", &m_cfg.sensitivity, 0.01f, 0.15f);
      ImGui::Checkbox("Invert Y", &m_cfg.invert_y);
      ImGui::Checkbox("Crosshair", &m_cfg.crosshair);
      ImGui::SliderFloat("UI Scale", &m_cfg.ui_scale, 1.0f, 3.0f);
      bool windowed = !m_cfg.borderless;
      if (ImGui::Checkbox("Windowed", &windowed)) {
        m_cfg.borderless = !windowed;
        GardenEngine::SetBorderless(window, m_cfg.borderless);
      }

      ImGui::Separator();
      if (ImGui::Button("Back")) {
        SaveSystem::SaveSettings(SETTINGS_PATH, m_cfg);   // persist immediately
        m_show_settings = false;
      }
      ImGui::End();
    }
  }
};
