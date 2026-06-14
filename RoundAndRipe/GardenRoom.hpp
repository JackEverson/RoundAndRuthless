#pragma once

#include "GLFW/glfw3.h"
#include "Renderer.hpp"

#include "Audio.hpp"
#include "FPSController.hpp"
#include "NotificationManager.hpp"
#include "PointLight.hpp"
#include "Scene.hpp"
#include "Surface.hpp"
#include "Texture.hpp"
#include "TriggerVolume.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"

#include <string>
#include <vector>

// Game-specific base scene. Inherits from Scene and provides shared state
// and geometry helpers for all RoundAndRuthless room scenes.
class GardenRoom : public Scene {

protected:
  SimpleSoundManager &soundManager;
  NotificationManager m_notification_manager;

  Camera m_camera;
  FPSController m_controller;

  std::vector<PointLight> m_lights;
  std::vector<Surface> m_surfaces;
  std::vector<Surface> m_tsurfaces;
  std::vector<TriggerVolume> m_triggers;

  bool m_cursor_captured = true;
  bool m_tab_before = false;

  GardenRoom()
      : soundManager(SimpleSoundManager::Instance()),
        m_controller(m_camera) {}

  // --- Geometry helpers ---
  // Add a wall aligned to the Z axis (facing +Z or -Z)
  Surface &AddWall(glm::vec3 pos, glm::vec2 size, Texture *texture) {
    Surface s;
    s.type = SurfaceType::Wall;
    s.position = pos;
    s.size = size;
    s.texture = texture;
    m_surfaces.push_back(s);
    return m_surfaces.back();
  }

  // Add a wall with a Y rotation (e.g. -90 degrees for left/right walls)
  Surface &AddWallRotated(glm::vec3 pos, glm::vec2 size, float rotationYDeg,
                          Texture *texture) {
    Surface s;
    s.type = SurfaceType::Wall;
    s.position = pos;
    s.size = size;
    s.texture = texture;
    s.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationYDeg),
                             glm::vec3(0, 1, 0));
    m_surfaces.push_back(s);
    return m_surfaces.back();
  }

  Surface &AddGlassRotated(glm::vec3 pos, glm::vec2 size, float rotationYDeg, float alpha,
                          Texture *texture) {
    Surface s;
    s.type = SurfaceType::Wall;
    s.position = pos;
    s.size = size;
    s.texture = texture;
    s.color.a = alpha;
    s.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationYDeg),
                             glm::vec3(0, 1, 0));
    m_tsurfaces.push_back(s);
    return m_tsurfaces.back();
  }

  // Add a horizontal floor surface
  Surface &AddFloor(glm::vec3 pos, glm::vec2 size, Texture *texture) {
    Surface s;
    s.type = SurfaceType::Floor;
    s.position = pos;
    s.size = size;
    s.texture = texture;
    s.rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    m_surfaces.push_back(s);
    return m_surfaces.back();
  }

  // Add a horizontal ceiling surface
  Surface &AddCeiling(glm::vec3 pos, glm::vec2 size, Texture *texture) {
    Surface s;
    s.type = SurfaceType::Ceiling;
    s.position = pos;
    s.size = size;
    s.texture = texture;
    s.rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    m_surfaces.push_back(s);
    return m_surfaces.back();
  }

  // Add a point light
  PointLight &AddLight(glm::vec3 pos, float radius,
                glm::vec3 color = glm::vec3(1.0f)) {
    PointLight l;
    l.position = pos;
    l.radius = radius;
    l.color = color;
    m_lights.push_back(l);
    return m_lights.back();
  }

  // --- Common input handling (cursor capture toggle) ---
  // Call from handleInput() in each room scene
  void HandleCommonInput(GLFWwindow &window, float delta) {
    glfwPollEvents();

    if (m_cursor_captured) m_controller.HandleInput(window, delta);
    
    m_controller.ResolveCollisions(m_surfaces);
    m_controller.ResolveCollisions(m_tsurfaces);
    m_controller.CheckTriggers(window, delta, m_triggers);

    if (glfwGetKey(&window, GLFW_KEY_MINUS) == GLFW_PRESS)
      glfwSetWindowShouldClose(&window, true);

    bool tab_pressed = glfwGetKey(&window, GLFW_KEY_TAB) == GLFW_PRESS;
    if (!m_tab_before && tab_pressed) {
      m_cursor_captured = !m_cursor_captured;
      glfwSetInputMode(&window, GLFW_CURSOR,
                       m_cursor_captured ? GLFW_CURSOR_DISABLED
                                         : GLFW_CURSOR_NORMAL);
      m_controller.Init(window);
    }
    m_tab_before = tab_pressed;
  }

  // --- Common render setup ---
  // Submits all surfaces to the renderer. Call between BeginBatchDraw and RendBatch.
  void SetupRenderingObjects(Renderer &renderer, int opaque = 30, int transparent = 10 ) {
    for (auto &surface : m_surfaces) {
      SpriteInstance sprite;
      sprite.position = surface.position;
      sprite.size = surface.size;
      sprite.color = surface.color;
      sprite.texture = surface.texture;
      sprite.model_mat = surface.rotation;
      renderer.SubmitSprite(sprite);
    }
    for (auto &surface : m_tsurfaces) {
      SpriteInstance sprite;
      sprite.position = surface.position;
      sprite.size = surface.size;
      sprite.color = surface.color;
      sprite.texture = surface.texture;
      sprite.model_mat = surface.rotation;
      renderer.SubmitTransparentSprite(sprite);
    }
  }
};
