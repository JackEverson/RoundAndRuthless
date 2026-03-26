#pragma once

#include "Renderer.hpp"

class Scene {
public:
  virtual ~Scene() = default;

  // Called once when scene becomes active
  virtual void onEnter(GLFWwindow &window) {}

  // Called once when scene is about to change
  virtual void onExit(GLFWwindow &window) {}

  // Return next scene (nullptr = stay on current scene)
  virtual Scene *update(GLFWwindow &window, float delta) = 0;

  // Render the scene
  virtual void render(GLFWwindow &window, Renderer &renderer) = 0;

  // Handle input
  virtual void handleInput(GLFWwindow &window, float delta) = 0;
};
