#pragma once

#include "GardenScene.hpp" 
#include "Event.hpp"

#include "imgui.h"
#include <GLFW/glfw3.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace RoundAndRipeEvents {

struct VoiceLine {
  std::string text;
  std::string voice = "meow_talk";
  VoiceLine(const char* t) : text(t) {}                    // plain strings still work
  VoiceLine(std::string t, std::string v) : text(std::move(t)), voice(std::move(v)) {}
};


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

inline std::vector<VoiceLine> TierUpLines(int tier) {
  switch (tier) {
  case 1: return {"Tier 1. The Gods of Round acknowledge your existence.",
                  "Barely."};
  case 2: return {"Tier 2! Your liver is safe for another day.",
                  "New seeds are available. Spend wisely, inmate."};
  default: return {"Another tier. How industrious."};
  }
}

class DialogueEvent : public Event {
  std::vector<VoiceLine> m_lines;
  int m_line = 0;
  bool m_advance_held = false; // edge-detect so one tap = one line

public:
  DialogueEvent(GardenScene &scene, std::vector<VoiceLine> lines)
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
    ImGui::TextWrapped("%s", m_lines[m_line].text.c_str());
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

// ── NotificationEvent: timed VoiceLines as notifications; no input, non-modal ─
class NotificationEvent : public Event {
  std::vector<VoiceLine> m_lines;
  float m_line_time;
  float m_timer = 0.0f;
  size_t m_line = 0;

public:
  NotificationEvent(GardenScene &scene, std::vector<VoiceLine> lines,
                    float line_time = 6.0f)
      : Event(scene), m_lines(std::move(lines)), m_line_time(line_time) {}

  void Update(float dt) override {
    m_timer -= dt;
    if (m_timer > 0.0f) return;
    if (m_line >= m_lines.size()) { m_complete = true; return; }
    m_scene.PushNotification(m_lines[m_line].text, m_line_time);
    m_scene.PlaySound(m_lines[m_line].voice);
    m_line++;
    m_timer = m_line_time;
  }
};

// ── TutorialEvent: reactive, polls world state and walks its own steps. ─
// Stays alive (never completes early) so Update() runs every frame until done.
class TutorialEvent : public Event {
  static constexpr float LINE_TIME = 6.0f;

  float m_timer = 0.0f;
  size_t m_line = 0;

  enum class Step { 
    Intro, 
    WaitForLookDown, 
    GreetSushi,
    WaitForNoRocks,
    WaitForHoedSpot,
    WaitForPlayerToHaveBiomass,
    WaitForPlayerToHaveSeed,
    WaitForPlantedTile,
    WaitForWateredTile,
    WaitForRipeTile,
    Done 
  };
  Step m_step = Step::Intro;


public:
  using Event::Event;

  // void OnStart() override {}

  void Update(float delta) override {
    switch (m_step) {
    case Step::Intro:
      m_scene.PushNotification("Hey. You. Down here!!!", LINE_TIME);
      m_scene.PlaySound("meow_talk");
      m_step = Step::WaitForLookDown;
      m_scene.SetTaskText("Look down");
      break;
    case Step::WaitForLookDown:
      if (m_scene.IsLookingDown()) {
        m_scene.ClearTaskText();
        m_step = Step::GreetSushi;
      }
      break;
    case Step::GreetSushi:
      if (PlayLines({
        VoiceLine{"HI!", "meow_talk"},
        "Welcome to Yield 3. An agricultural prison world. Most people just call this Planet 'The Garden'.",
        "You are now a prisoner here. The crime you committed? doesn't matter......",
        VoiceLine{"ALL THAT MATTERS IS THAT YOU BLASPHEMED AGAINST THE FOUR GODS OF ROUND!", "meow_angry"},
        "I'm a Monitor Sushi and your designated parole officer....",
        "Following protocol, I have therefore been surgically attached to your liver.",
        VoiceLine{"I now offer you a choice: MAKE QUOTA OR I KEEP THE LIVER.", "meow_angry"},
        "Lets get you started. This field has been left a mess... pull out your shovel and clear out these rocks",
        },delta)) {
        m_scene.SetTaskText("Select shovel [5] and clear the rocks from the field [LMB] or [E]");
        m_step = Step::WaitForNoRocks;
      }
      break;
    case Step::WaitForNoRocks:
      if (m_scene.HasNoRocks()) {
        if (PlayLines({"Good. Nice and clear. Now hoe a spot for planting."}, delta)) {
          m_scene.SetTaskText("Select hoe [2] and hoe a spot for planting [LMB] or [E]");
          m_step = Step::WaitForHoedSpot;
        }
      }
      break;
    case Step::WaitForHoedSpot:
      if (m_scene.HasHoedTile()) {
        if (PlayLines({
          "Excellent. Now you need some seeds. Convert some of your biomass into seeds at the seed maker.",
          VoiceLine{"Oh..... you don't have biomass yet? Apples fall around the field every now and then. Go find one!", "meow_sad"},
          }, delta)) {
            m_scene.SetTaskText("Find the apple and collect it [LMB] or [E]");
            m_step = Step::WaitForPlayerToHaveBiomass;
        }
      }
      break;
    case Step::WaitForPlayerToHaveBiomass:
      if (m_scene.PlayerHasBiomass()) {
        m_scene.PlaySound("meow_talk");
        m_scene.PushNotification("Now you have biomass! Go to the seed maker to convert it into seeds.", LINE_TIME);
        m_scene.SetTaskText("Interact with the seed maker using [LMB] or [E]");
        m_step = Step::WaitForPlayerToHaveSeed;
      }
      break;
    case Step::WaitForPlayerToHaveSeed:
      if (m_scene.PlayerHasSeed()) {
        m_scene.PlaySound("meow_talk");
        m_scene.PushNotification("You have seeds! pull out your seed packet and plant them in the hoed spot.", LINE_TIME);
        m_scene.SetTaskText("Select seed packet [4] and Select seed with [RMB] Use [LMB] or [E] to plant");
        m_step = Step::WaitForPlantedTile;
      }
      break;
    case Step::WaitForPlantedTile:
      if (m_scene.HasPlantedTile()) {
            m_scene.PlaySound("meow_angry");
            m_scene.PushNotification("Don't just stare at it. Water it!", LINE_TIME);
            m_scene.SetTaskText("Select watering can [3] and water the plants [LMB] or [E]");
        m_step = Step::WaitForWateredTile;
      }
      break;
    case Step::WaitForWateredTile:
      if (m_scene.HasWateredTile()) {
        m_scene.PlaySound("meow_sad");
        m_scene.PushNotification("Now we wait......", LINE_TIME);
        m_scene.SetTaskText("Wait for the plants to grow and ripen. Make sure it stays watered.");
        m_step = Step::WaitForRipeTile;
      }
      break;
    case Step::WaitForRipeTile:
      if (m_scene.HasRipeTile()) {
        m_scene.PlaySound("meow_talk");
        m_scene.PushNotification("The plants are ripe, Harvest Time! Make sure to use your hands and not your shovel or you will destroy the plants.", LINE_TIME);
        m_scene.SetTaskText("Select hands [1] and harvest the ripe plants [LMB] or [E]");
        m_step = Step::Done;
      }
      break;
    case Step::Done:
    if (m_scene.HarvestCount() >= 1) {
      if (PlayLines({
            "Good. Once you have harvested enough biomass, you can upgrade what tier of plants you can get at the Chest.",
            "The last tier represents your quota, hit that and you can go home.",
            "Have fun! If you need me I'll be nuzzling further into your liver.",
            }, delta)) {
              m_complete = true;
              m_scene.ClearTaskText();
            }
      }
      break;
    }
  }

   bool PlayLines(const std::vector<VoiceLine>& lines, float dt, float line_time = LINE_TIME) {
      m_timer -= dt;
        if (m_timer <= 0.0f) {
          if (m_line >= lines.size()) { m_line = 0; m_timer = 0; return true; }
          m_scene.PushNotification(lines[m_line].text, line_time);
          m_scene.PlaySound(lines[m_line].voice);
          m_line++;
          m_timer = line_time;
        }
        return false;
  }
};


inline std::unique_ptr<Event> GetRandomBackgroundEvent(GardenScene &scene) {
  int event_number = rand() % 4;

  switch (event_number) {
    case 0: // a distant, unexplained detonation. Not your business, inmate.
      return std::make_unique<ActionEvent>(scene, [](GardenScene &s) {
        s.PlaySound("boom", 0.8f);
      });

    case 1: // Sushi small talk
      return std::make_unique<ActionEvent>(scene, [](GardenScene &s) {
        s.PlaySound("meow_talk");
        s.PushNotification("Your liver is very warm. Cozy.", 4.0f);
      });

    case 2: // Sushi passive-aggression
      return std::make_unique<ActionEvent>(scene, [](GardenScene &s) {
        s.PlaySound("meow_sad");
        s.PushNotification("The quota isn't going to fill itself.", 4.0f);
      });

    case 3: // Sushi hears the booms too. Probably fine.
      return std::make_unique<ActionEvent>(scene, [](GardenScene &s) {
        s.PlaySound("meow_talk");
        s.PushNotification("Did you hear that? ......No? Good.", 4.0f);
      });

    default:
      return nullptr;
  }
}


} // namespace RoundAndRipeEvents
