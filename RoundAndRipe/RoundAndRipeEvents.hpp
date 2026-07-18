#pragma once

#include "GardenScene.hpp" 
#include "Event.hpp"

#include "imgui.h"
#include <GLFW/glfw3.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace RoundAndRipeEvents {

static constexpr float LINE_TIME = 4.5f;

struct VoiceLine {
  std::string text;
  std::string voice = "meow_talk";
  VoiceLine(const char* t) : text(t) {}                    // plain strings still work
  VoiceLine(std::string t, std::string v) : text(std::move(t)), voice(std::move(v)) {}
};

class LineEvent : public Event {
protected:
  float m_line_timer = 0.0f;
  size_t m_line = 0;

  using Event::Event;   // inherit the (GardenScene&) constructor

  bool PlayLines(const std::vector<VoiceLine> &lines, float dt,
                 float line_time = LINE_TIME) {
    m_line_timer -= dt;
    if (m_line_timer <= 0.0f) {
      if (m_line >= lines.size()) { m_line = 0; m_line_timer = 0; return true; }
      if (lines[m_line].text.empty()) { m_line++; return false; }
      float t = std::max(line_time,
                         1.5f + 0.075f * (float)lines[m_line].text.size());
      m_scene.PushNotification(lines[m_line].text, t);
      m_scene.PlaySound(lines[m_line].voice);
      m_line++;
      m_line_timer = t;
    }
    return false;
  }
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
  case 1: return {"Tier 1. You get access to new seeds and sprinklers.",
                  "Start automating that watering! I'll be here... in your liver"};
  case 2: return {"Tier 2. Kidney beans and harvesting things!"};
  case 3: return {"Tier 3. You unlocked staring Melon......",
                  VoiceLine("I don't like them.... they feel.... judgey...", "meow_sad")};
  case 4: return {"Tier 4......",
                  "You have gained access to the LIVER BOUQUET!",
                  VoiceLine("You are truly blessed by the 4 GODS OF ROUND", "meow_angry")};
  
  default: return {"Another tier. How industrious."};
  }
}

inline std::unique_ptr<Event> GetLookDownQuip(GardenScene &scene) {
  static const std::vector<VoiceLine> quips = {
      {"Hi.", "meow_talk"},
      {"Don't mind me.", "meow_talk"},
      {"Yes, that ache in your side is me. Keep working.", "meow_talk"},
      {"What? Never seen a parole officer in a liver before?", "meow_angry"},
      {"The Gods of Round see you too.", "meow_sad"},
  };
  const VoiceLine &q = quips[rand() % quips.size()];
  return std::make_unique<ActionEvent>(scene, [q](GardenScene &s) {
    s.PlaySound(q.voice);
    s.PushNotification(q.text, 4.0f);
  });
}

class DialogueEvent : public Event {
  std::vector<VoiceLine> m_lines;
  int m_line = 0;
  bool m_advance_held = false; // edge-detect so one tap = one line

public:
  DialogueEvent(GardenScene &scene, std::vector<VoiceLine> lines)
      : Event(scene), m_lines(std::move(lines)) {}

  void Render(Renderer &, const int &, const int& ) override {
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
    ImGui::SetWindowFontScale(m_scene.GetUiScale(io.DisplaySize.y));
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
    if (!m_lines[m_line].text.empty()){ m_scene.PushNotification(m_lines[m_line].text, m_line_time); }
    m_scene.PlaySound(m_lines[m_line].voice);
    m_line++;
    m_timer = m_line_time;
  }
};

// ── TutorialEvent: reactive, polls world state and walks its own steps. ─
// Stays alive (never completes early) so Update() runs every frame until done.
class TutorialEvent : public LineEvent {

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
  using LineEvent::LineEvent;

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
        m_scene.SetTaskText("Select shovel [6] and clear the rocks [LMB] or [E]");
        m_step = Step::WaitForNoRocks;
      }
      break;
    case Step::WaitForNoRocks:
      if (m_scene.HasNoRocks()) {
        m_scene.PlaySound("meow_talk");
        m_scene.PushNotification("Good. Nice and clear. Now hoe a spot for planting.", LINE_TIME);
        m_scene.SetTaskText("Select hoe [2] and hoe a spot for planting [LMB] or [E]");
        m_step = Step::WaitForHoedSpot;
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
};

class EndEvent : public LineEvent {
  enum class EndPhase { None, Farewell, FadeRed, FadeBlack, Hold };
  EndPhase m_end_phase = EndPhase::None;
  float m_end_timer = 0.0f;

  using LineEvent::LineEvent;

  void OnStart() override{
    m_scene.SetOutcome(GardenScene::Outcome::Won);         
  }

  void Update(float delta) override {
    switch (m_end_phase) {
    case EndPhase::None:
    m_scene.EaseLookDown(delta);
    if (PlayLines({
        VoiceLine{"You have reached your final tier. The FOUR GODS OF ROUND acknowledge your existence", "meow_talk"},
        VoiceLine{"Barely.", "meow_sad"},
        VoiceLine{"Well I guess now you can go home and I keep the liver", "meow_talk"},
        VoiceLine{"Oh did I say you go home OR I keep your ", "meow_angry"},
        VoiceLine{"I always seem to mix them up", "meow_talk"},
        VoiceLine{"And by the way, this is your home now", "meow_talk"},
        },delta, 4.0f)) {
        m_scene.SetTaskText("Relinquish your Liver");
        m_end_phase = EndPhase::Farewell;
      }
      break;

    case EndPhase::Farewell:
        m_scene.PlaySound("eating");                     
        m_end_phase = EndPhase::FadeRed;
        m_end_timer = 0.0f;
      break;

    case EndPhase::FadeRed:                      // ~2s: red rises
      m_end_timer += delta;
      if (m_end_timer >= 2.0f) { m_end_phase = EndPhase::FadeBlack; m_end_timer = 0.0f; }
      break;

    case EndPhase::FadeBlack:                    // ~1.5s: red drains to black
      m_end_timer += delta;
      if (m_end_timer >= 1.5f) { m_end_phase = EndPhase::Hold; m_end_timer = 0.0f; }
      break;

    case EndPhase::Hold:                         // beat of black, then out
      m_end_timer += delta;
      if (m_end_timer >= 1.5f) m_scene.QUIT();
      break;
    }
  }

  void Render(Renderer& r, const int &w, const int &h) override {
     
    if (m_end_phase == EndPhase::FadeRed) {
      int a = (int)(255 * std::min(m_end_timer / 2.0f, 1.0f));
      ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0,0), ImVec2((float)w,(float)h),
                                                    IM_COL32(160, 10, 10, a));
    } else if (m_end_phase == EndPhase::FadeBlack || m_end_phase == EndPhase::Hold) {
      float t = (m_end_phase == EndPhase::Hold) ? 1.0f : std::min(m_end_timer / 1.5f, 1.0f);
      ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(0,0), ImVec2((float)w,(float)h),
          IM_COL32((int)(160 * (1.0f - t)), (int)(10 * (1.0f - t)), (int)(10 * (1.0f - t)), 255));
    }
  }
};


inline std::unique_ptr<Event> GetRandomBackgroundEvent(GardenScene &scene) {
  
  int event_number = rand() % 5;

  switch (event_number) {
    case 0: 
    case 1:
      return std::make_unique<NotificationEvent>(scene, std::vector<VoiceLine>{
          VoiceLine{"", "boom"},                                        
          VoiceLine{"Did you hear that? ......No? Good.", "meow_talk"},
      }, 5.0f);

    case 2: // Sushi small talk
      return std::make_unique<ActionEvent>(scene, [](GardenScene &s) {
        s.PlaySound("meow_talk");
        s.PushNotification("Your liver is very warm. Cozy.", 4.0f);
      });

    case 3: // Sushi passive-aggression
      return std::make_unique<ActionEvent>(scene, [](GardenScene &s) {
        s.PlaySound("meow_sad");
        s.PushNotification("The quota isn't going to fill itself.", 4.0f);
      });

    case 4: 
      return std::make_unique<ActionEvent>(scene, [](GardenScene &s) {
        s.PlaySound("meow_angry");
        s.PushNotification("Are you done yet?", 4.0f);
      });

    default:
      return nullptr;
  }
}


} // namespace RoundAndRipeEvents
