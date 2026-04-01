#include "imgui_internal.h"
#include <string>
#include <vector>

struct Notification {
  std::string message;
  float duration;
  float remaining;
};

class NotificationManager {
private:
  std::vector<Notification> m_notifications;

public:
  NotificationManager();
  ~NotificationManager();

  void Push(const std::string &message, float duration = 5.0f);
  void Update(float delta);
  void Render(int w, int h);
};

inline NotificationManager::NotificationManager() {}

inline NotificationManager::~NotificationManager() {}

inline void NotificationManager::Push(const std::string &message, float duration) {
  Notification note;
  note.duration = duration;
  note.remaining = duration;
  note.message = message;

  m_notifications.push_back(note);
}

inline void NotificationManager::Update(float delta) {

  for (auto& note : m_notifications) {
    note.remaining -= delta;
  }

  std::erase_if(m_notifications, [](const Notification& n){
    return n.remaining <= 0.0f;
  });
}

inline void NotificationManager::Render(int w, int h){
    if (m_notifications.empty()) return;
    
    ImGui::SetNextWindowPos(ImVec2(w * 0.5f, h * 0.85f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::Begin("##notifications", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings);

    for (auto& n : m_notifications) {
        float alpha = n.remaining / n.duration;
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, alpha));
        ImGui::TextUnformatted(n.message.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::End();
}
