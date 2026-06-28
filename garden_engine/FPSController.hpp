#pragma once

#include "Camera.hpp"
#include "Surface.hpp"
#include "TriggerVolume.hpp"

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_float4x4.hpp"

#include <algorithm>

class FPSController {
private:
  Camera &m_camera;

  float m_player_radius = 0.3f;
  float m_speed = 5.0f;
  double m_mouse_sensitivity = 0.05;
  double m_lastMouseX = 0.0;
  double m_lastMouseY = 0.0;

  bool m_left_click_before = false;
  bool m_interaction_before = false;
  bool m_interaction_held = false;

  bool m_input_disabled = false;

public:
  int KeyForward = GLFW_KEY_W;
  int KeyBack = GLFW_KEY_S;
  int KeyLeft = GLFW_KEY_A;
  int KeyRight = GLFW_KEY_D;
  int KeyJump = GLFW_KEY_SPACE;
  int KeyCrouch = GLFW_KEY_C;

  bool NoClip = false;

  int KeyInteract = GLFW_KEY_E;
  int MouseClick = GLFW_MOUSE_BUTTON_LEFT;

public:
  FPSController(Camera &camera);
  ~FPSController();
  void Init(GLFWwindow &window);
  void HandleInput(GLFWwindow &window, float delta);
  void ResolveCollisions(const std::vector<Surface> &surfaces);
  void CheckTriggers(GLFWwindow &window, float delta,
                     std::vector<TriggerVolume> &interactables);

  bool InteractionHeld() const { return m_interaction_held; }

  bool InputDisabled() const { return m_input_disabled; }
  void EnableInput() { m_input_disabled = false; }
  void DisableInput() { m_input_disabled = true; }  


private:
  bool RayInBox(const glm::vec3 &sourcePos, const glm::vec3 &sourceDir,
                const glm::vec3 &targetPos, const glm::vec3 &targetSize,
                const glm::mat4 &targetRotation) const;
};

inline FPSController::FPSController(Camera &camera) : m_camera(camera) {}

inline FPSController::~FPSController() {}

inline void FPSController::Init(GLFWwindow &window) {
  double x, y;
  glfwGetCursorPos(&window, &x, &y);
  m_lastMouseX = x;
  m_lastMouseY = y;
}

inline void FPSController::HandleInput(GLFWwindow &window, float delta) {
  if (InputDisabled()) return;
  
  float speed = m_speed;
  if (glfwGetKey(&window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
      glfwGetKey(&window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
    speed *= 5;
  }
  auto flat_forward = m_camera.GetForward() * glm::vec3(1, 0, 1);
  auto flat_right = m_camera.GetRight() * glm::vec3(1, 0, 1);

  if (glfwGetKey(&window, KeyForward) == GLFW_PRESS)
    m_camera.ShiftCamera(flat_forward * speed * delta);
  if (glfwGetKey(&window, KeyBack) == GLFW_PRESS)
    m_camera.ShiftCamera(-flat_forward * speed * delta);
  if (glfwGetKey(&window, KeyLeft) == GLFW_PRESS)
    m_camera.ShiftCamera(-flat_right * speed * delta);
  if (glfwGetKey(&window, KeyRight) == GLFW_PRESS)
    m_camera.ShiftCamera(flat_right * speed * delta);
  // if (glfwGetKey(&window, KeyJump) == GLFW_PRESS)
  //   m_camera.ShiftCamera(glm::vec3(0, 1, 0) * speed * delta);
  // if (glfwGetKey(&window, KeyCrouch) == GLFW_PRESS)
  //   m_camera.ShiftCamera(glm::vec3(0, -1, 0) * speed * delta);

  // bool left_click = false;
  // if (!m_left_click_before &&
  //     glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
  // { left_click = true; }

  // if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
  // m_left_click_before = true; else m_left_click_before = false;

  double mouseX, mouseY;
  glfwGetCursorPos(&window, &mouseX, &mouseY);

  double deltaX = (mouseX - m_lastMouseX) * m_mouse_sensitivity;
  double deltaY = (mouseY - m_lastMouseY) * -m_mouse_sensitivity;

  m_camera.ShiftRotation(deltaX, deltaY);

  m_lastMouseX = mouseX;
  m_lastMouseY = mouseY;
}

inline void
FPSController::ResolveCollisions(const std::vector<Surface> &surfaces) {
  if (NoClip)
    return;

  glm::vec3 campos = m_camera.GetLocation();

  for (const auto &surface : surfaces) {
    // if (surface.type != SurfaceType::Wall)
    //     continue;

    glm::vec3 normal = glm::normalize(glm::vec3(surface.rotation[2]));
    glm::vec3 to_player = campos - surface.position;
    float dist = glm::dot(to_player, normal);

    if (dist <= -m_player_radius || dist >= m_player_radius)
      continue;

    glm::mat4 invRot = glm::inverse(surface.rotation);
    glm::vec3 localPos = glm::vec3(invRot * glm::vec4(to_player, 0.0f));
    float halfW = surface.size.x * 0.5f;
    float halfH = surface.size.y * 0.5f;

    if (localPos.x < -halfW || localPos.x > halfW)
      continue;
    if (localPos.y < -halfH || localPos.y > halfH)
      continue;

    float penetration = m_player_radius - glm::abs(dist);
    campos += glm::sign(dist) * normal * penetration;
  }

  m_camera.SetCamera(campos);
}

inline void
FPSController::CheckTriggers(GLFWwindow &window, float delta,
                             std::vector<TriggerVolume> &interactables) {
  if (m_input_disabled) return;

  m_interaction_held = glfwGetKey(&window, KeyInteract) == GLFW_PRESS ||
                          glfwGetMouseButton(&window, MouseClick) == GLFW_PRESS;

  bool interaction_started = false;
  if (!m_interaction_before && m_interaction_held) {
    interaction_started = true;
  }

  glm::vec3 origin = m_camera.GetLocation();
  glm::vec3 dir = m_camera.GetForward();

  for (auto &trigger : interactables) {
    // distance cull
    if (glm::length(trigger.position - origin) > trigger.interaction_distance &&
        trigger.interaction_distance > 0) {
      trigger.Reset();
      continue;
    }

    switch (trigger.type) {

    case TriggerType::Interact:

      // ray cull
      if (!RayInBox(origin, dir, trigger.position, trigger.size,
                    trigger.rotation)) {
        trigger.Reset();
        continue;
      }

      if (!m_interaction_held) {
        trigger.Reset();
        continue;
      }
      if (interaction_started || trigger.trigger_timer != 0) {
        trigger.Fuse(delta);
      } else {
        trigger.Reset();
        continue;
      }
      break;

    case TriggerType::LookAt:
      if (!RayInBox(origin, dir, trigger.position, trigger.size,
                    trigger.rotation)) {
        trigger.Reset();
        continue;
      }
      trigger.Fuse(delta);
      break;

    case TriggerType::Proximity:
      trigger.Fuse(delta);
      break;
    }
  }

  m_interaction_before = m_interaction_held;
}

inline bool FPSController::RayInBox(const glm::vec3 &sourcePos,
                                    const glm::vec3 &sourceDir,
                                    const glm::vec3 &targetPos,
                                    const glm::vec3 &targetSize,
                                    const glm::mat4 &targetRotation) const {

  glm::mat4 inv_rot = glm::inverse(targetRotation);
  glm::vec3 local_origin =
      glm::vec3(inv_rot * glm::vec4(sourcePos - targetPos, 0.0f));
  glm::vec3 local_dir = glm::vec3(inv_rot * glm::vec4(sourceDir, 0.0f));

  glm::vec3 half = targetSize / 2.0f;

  float tmin_x = (-half.x - local_origin.x) / local_dir.x;
  float tmax_x = (half.x - local_origin.x) / local_dir.x;
  if (tmin_x > tmax_x)
    std::swap(tmin_x, tmax_x);

  float tmin_y = (-half.y - local_origin.y) / local_dir.y;
  float tmax_y = (half.y - local_origin.y) / local_dir.y;
  if (tmin_y > tmax_y)
    std::swap(tmin_y, tmax_y);

  float tmin_z = (-half.z - local_origin.z) / local_dir.z;
  float tmax_z = (half.z - local_origin.z) / local_dir.z;
  if (tmin_z > tmax_z)
    std::swap(tmin_z, tmax_z);

  float tmin = std::max({tmin_x, tmin_y, tmin_z});
  float tmax = std::min({tmax_x, tmax_y, tmax_z});

  if (tmin <= tmax && tmax > 0.0f) {
    return true;
  } else {
    return false;
  }
}