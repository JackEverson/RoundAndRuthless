#pragma once
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <memory>

#include "Audio.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"

class GardenEngine {

private:
  std::unique_ptr<Scene> m_currentScene;

  GLFWwindow *m_window;
  Renderer *m_renderer;
  SimpleSoundManager &soundManager;

  bool m_first_click = false;

public:
  GardenEngine(std::string, bool windowed, int win_width, int win_height, const std::string& png_icon = "");
  ~GardenEngine();

  int Start(std::unique_ptr<Scene> scene, float fps);

  // Windowed fullscreen (borderless) toggle. Static so scenes — which only
  // hold the GLFWwindow& — can call it; remembers the windowed rect to restore.
  static void SetBorderless(GLFWwindow &window, bool borderless);

private:
  void setupGlfwWindow(std::string name, bool windowed, int win_width,
                       int win_height, const std::string& png_icon = "");
  void setupOpenGl();
  void setupImGui();
  void setupAudio();

  void prepImgui();
  void renderImgui();
  void checkImgui();

  float
  calculateDeltaTime(std::chrono::steady_clock::time_point &lastFrameStart);
};

// callbacks
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
