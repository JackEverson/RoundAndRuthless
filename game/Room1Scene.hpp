#pragma once

#include "Engine.hpp"

#include "GameScene.hpp"
#include "glm/ext/vector_float3.hpp"

class Room1Scene : public GameScene {

public:
  // Textures
  Texture m_ceiling_texture;
  Texture m_wall_texture;
  Texture m_floor_texture;
  Texture m_sushi_texture;
  // TODO: add eye chart, button, scale textures

  // Sushi observer billboard
  SpriteInstance m_sushi_observer;

  // Door animation
  int m_door_index = -1;
  bool m_door_opening = false;
  float m_door_open_progress = 0.0f;

  // Puzzle state
  int m_stage = 0;
  // Stage 0: look at eye chart
  // Stage 1: stand on scale
  // Stage 2: press button
  // Stage 3: door open, room complete

  Room1Scene()
      : m_ceiling_texture("./res/textures/plaster_ceiling.png"),
        m_wall_texture("./res/textures/concrete_wall.png"),
        m_floor_texture("./res/textures/gravel_floor.png"),
        m_sushi_texture("./res/textures/sushi.png") {}

  void onEnter(GLFWwindow &window) override {
    glfwSetInputMode(&window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_controller.Init(window);

    float room_size = 10.0f;
    float room_height = 3.0f;
    float sushi_size = 2.0f;

    float half_room_size = room_size / 2;
    float half_room_height = room_height / 2;
    float half_sushi_size = sushi_size / 2;
    float half_roomsushi_size = half_sushi_size + half_room_size;

    glm::vec3 sushi_position =
        glm::vec3(0.0f, half_sushi_size, -half_room_size - half_sushi_size);

    glm::vec3 light_color = glm::vec3(0.85f, 0.92f, 1.0f) / 3.0f;

    // test room
    AddLight(glm::vec3(0, room_height, 0), half_room_size, light_color);

    AddFloor(glm::vec3(0.0f), glm::vec2(room_size), &m_floor_texture);
    AddCeiling(glm::vec3(0.0f, room_height, 0.0f), glm::vec2(room_size),
               &m_ceiling_texture);

    AddWall(glm::vec3(0.0f, half_room_height, half_room_size),
            glm::vec2(room_size, room_height), &m_wall_texture);
    AddWallRotated(glm::vec3(half_room_size, half_room_height, 0),
                   glm::vec2(room_size, room_height), 90.0f, &m_wall_texture);
    AddWallRotated(glm::vec3(-half_room_size, half_room_height, 0),
                   glm::vec2(room_size, room_height), 90.0f, &m_wall_texture);

    AddWall(glm::vec3(0.0f, half_room_height / 2, -half_room_size),
            glm::vec2(room_size, half_room_height), &m_wall_texture);
    AddGlassRotated(
        glm::vec3(0.0f, 3 * (half_room_height / 2), -half_room_size),
        glm::vec2(room_size, half_room_height), 0.0f, 0.2f, &m_wall_texture);

    // observation room
    AddLight(glm::vec3(sushi_position.x, room_height, sushi_position.z),
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

    // --- Sushi observer ---
    m_sushi_observer.texture = &m_sushi_texture;
    m_sushi_observer.size = glm::vec2(sushi_size, sushi_size);
    m_sushi_observer.color = glm::vec4(1.0f);
    m_sushi_observer.position = sushi_position;

    // --- Triggers ---

    // Stage 0: Look at eye chart
    TriggerVolume eye_chart;
    eye_chart.type = TriggerType::LookAt;
    eye_chart.single_trigger = true;
    // TODO: set position and size to match chart sprite on wall
    eye_chart.on_triggered = [this]() {
      if (m_stage != 0)
        return;
      m_stage = 1;
      m_notification_manager.Push("Good. Visual acuity confirmed.", 3.0f);
    };
    m_triggers.push_back(eye_chart);

    // Stage 1: Stand on scale
    TriggerVolume scale;
    scale.type = TriggerType::Proximity;
    scale.single_trigger = true;
    // TODO: set position and interaction_distance
    scale.on_triggered = [this]() {
      if (m_stage != 1)
        return;
      m_stage = 2;
      m_notification_manager.Push("Weight noted.", 3.0f);
    };
    m_triggers.push_back(scale);

    // Stage 2: Press the button
    TriggerVolume button;
    button.type = TriggerType::Interact;
    button.single_trigger = true;
    // TODO: set position and size to match button sprite on wall
    button.on_triggered = [this]() {
      if (m_stage != 2)
        return;
      m_stage = 3;
      m_door_opening = true;
      m_notification_manager.Push("You may proceed.", 3.0f);
    };
    m_triggers.push_back(button);
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
      float slide_speed = 4.0f;

      glm::vec3 slide_dir = glm::normalize(glm::vec3(door.rotation[0]));
      door.position += slide_dir * slide_speed * delta;
      m_door_open_progress += slide_speed * delta;

      if (m_door_open_progress >= door.size.x) {
        m_surfaces.erase(m_surfaces.begin() + m_door_index);
        m_door_index = -1;
        m_door_opening = false;
      }
    }

    // TODO: return next scene when room is complete
    return nullptr;
  }

  void handleInput(GLFWwindow &window, float delta) override {
    HandleCommonInput(window, delta);
  }

  void render(GLFWwindow &window, Renderer &renderer) override {

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

    renderer.RendBatch(view, projection, campos, 0.05f);

    // HUD: current task instruction
    const char *task_text = "";

    switch (m_stage) {

    case 0:
      task_text = "Please direct your gaze to the eye chart.";
      break;
    case 1:
      task_text = "Please step onto the scale for weighing.";
      break;
    case 2:
      task_text = "Please press the call button when ready to proceed.";
      break;
    case 3:
      task_text = "You may proceed through the door.";
      break;
    }

    ImGui::SetNextWindowPos(ImVec2(w / 2.0f, 20.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##task", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%s", task_text);
    ImGui::End();

    m_notification_manager.Render(w, h);
  }
};
