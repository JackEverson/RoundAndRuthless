#pragma once

// RoundAndRipeEvents.hpp — concrete Event subclasses + the per-day schedule.
// Included ONLY in GardenScene.cpp (never in a header) so it can pull in the
// full GardenScene without forming a cycle.
#include "GardenScene.hpp" // full definition — concrete events call scene verbs
#include "Event.hpp"

#include "imgui.h"
#include <GLFW/glfw3.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class ActionEvent : public Event {
  std::function<void(GardenScene &)> m_fn;

public:
  ActionEvent(GardenScene &scene, std::function<void(GardenScene &)> fn)
      : Event(scene), m_fn(std::move(fn)) {}

  void OnStart() override {
    if (m_fn)
      m_fn(m_scene);
    m_complete = true; // instant — done the moment it starts
  }
};

class DialogueEvent : public Event {
  std::vector<std::string> m_lines;
  int m_line = 0;
  bool m_advance_held = false; // edge-detect so one tap = one line

public:
  DialogueEvent(GardenScene &scene, std::vector<std::string> lines)
      : Event(scene), m_lines(std::move(lines)) {}

  void Render(Renderer &) override {
    if (m_line >= (int)m_lines.size())
      return;

    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.8f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.85f);
    ImGui::Begin("##dialogue", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetWindowFontScale(2.0f);
    ImGui::TextWrapped("%s", m_lines[m_line].c_str());
    ImGui::Text("[E] continue");
    ImGui::End();
  }

  void HandleInput(GLFWwindow &window, float /*delta*/) override {
    bool pressed = glfwGetKey(&window, GLFW_KEY_E) == GLFW_PRESS;
    if (pressed && !m_advance_held) { 
      m_line++;
      if (m_line >= (int)m_lines.size())
        m_complete = true;
    }
    m_advance_held = pressed;
  }
};


// ── TutorialEvent: reactive, polls world state and walks its own steps. ─
// Stays alive (never completes early) so Update() runs every frame until done.
class TutorialEvent : public Event {
  enum class Step { WaitForPlant, WaitForLookDown, Done };
  Step m_step = Step::WaitForPlant;

public:
  using Event::Event;

  void OnStart() override {
    m_scene.PushNotification("Equip the seed packet and plant a seed.");
  }

  void Update(float /*delta*/) override {
    switch (m_step) {
    case Step::WaitForPlant:
      if (m_scene.HasPlantedTile()) {
        m_scene.PushNotification("Good. Now look down — say hi to Sushi.");
        m_step = Step::WaitForLookDown;
      }
      break;
    case Step::WaitForLookDown:
      if (m_scene.IsLookingDown()) {
        m_scene.PushNotification("There I am. Now get to work.");
        m_step = Step::Done;
        m_complete = true; // finished → swept from m_events next sweep
      }
      break;
    case Step::Done:
      break;
    }
  }
};

namespace RoundAndRipeEvents {

inline std::vector<std::unique_ptr<Event>> GetDaysEvents(GardenScene &scene,
                                                         int day) {
  std::vector<std::unique_ptr<Event>> events;

  switch (day) {
  case 0:
    events.push_back(std::make_unique<TutorialEvent>(scene));
    break;
  case 1:
    events.push_back(std::make_unique<DialogueEvent>(
    scene, std::vector<std::string>{
                "Wakey wakey, inmate.",
                "That ache in your side? That's me. Say hi.",
                "100 days to make quota, or I keep the liver. Dig."}));
                break;
  case 2:
  case 3:
  case 4:
  case 5:
    events.push_back(std::make_unique<DialogueEvent>(
    scene, std::vector<std::string>{
                "Wakey wakey.", 
                "Your liver is mine now.", 
                "Get to work."}));
                break;

  // Example of an instant effect on a scheduled day (needs a WaterAll verb):
  // case 7:
  //   events.push_back(std::make_unique<ActionEvent>(scene,
  //       [](GardenScene &s) { s.PushNotification("It rained overnight."); }));
  //   break;

  default:
    break;
  }
  return events;
}

} // namespace RoundAndRipeEvents
