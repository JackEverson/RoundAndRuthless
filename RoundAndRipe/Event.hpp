#pragma once

#include "Renderer.hpp"   
#include <GLFW/glfw3.h>   

class GardenScene;        // forward declaration — full definition lives in GardenScene.hpp

class Event {
protected:
  GardenScene& m_scene;
  bool m_complete = false;
  
public:
  Event(GardenScene& s) : m_scene(s) {}
  virtual ~Event() = default;
  virtual void OnStart() {}                    // empty defaults:
  virtual void Update(float dt) {}             // override only what you need
  virtual void Render(Renderer& r, const int& w, const int& h) {}
  virtual void HandleInput(GLFWwindow& w, float dt) {}
  virtual void OnComplete() {}
  bool IsComplete() const { return m_complete; }

};