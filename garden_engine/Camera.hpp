#pragma once

#include "glm/common.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

class Camera
{

public:
  Camera() :
  m_position(0.0f, 1.6f, 2.0f),
  m_yaw(glm::radians(-90.0))
  {
    CalcLookAt();
  };
  ~Camera() {};

  glm::vec3 GetLocation() const { return m_position; }

  glm::mat4 GetViewMat() const
  {
    glm::mat4 view = glm::lookAt(m_position, m_position + m_forward,
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    return view;
  }

  glm::mat4 GetProjectionMat(float screen_width, float screen_height) const
  {
    glm::mat4 proj = glm::perspective(glm::radians(m_fov), screen_width / screen_height,
                                      m_clip_near, m_clip_far);
    return proj;
  }

  void ShiftCamera(glm::vec3 delta) { m_position += delta; }

  void SetCamera(glm::vec3 loc) { m_position = loc; }

  void SetRotation(float yawDegrees, float pitchDegrees)
  {
    m_yaw = glm::radians(yawDegrees);
    float new_pitch = glm::clamp(pitchDegrees, -89.0f, 89.0f);
    m_pitch = glm::radians(new_pitch);
    CalcLookAt();
  }

  void ShiftRotation(float yawDeltaDegrees, float pitchDeltaDegree)
  {
    m_yaw += glm::radians(yawDeltaDegrees);

    float new_pitch =
        glm::clamp(glm::degrees(m_pitch) + pitchDeltaDegree, -89.0f, 89.0f);

    m_pitch = glm::radians(new_pitch);
    CalcLookAt();
  }

  glm::vec3 GetForward() const { return m_forward; }
  glm::vec3 GetRight() const { return m_right; }

private:
  void CalcLookAt()
  {
    m_forward = glm::vec3(cos(m_yaw) * cos(m_pitch), sin(m_pitch),
                          sin(m_yaw) * cos(m_pitch));
    m_right = glm::cross(m_forward, glm::vec3(0, 1, 0));
  }

  glm::vec3 m_position;
  float m_yaw = 0;   // radians
  float m_pitch = 0; // radians
  glm::vec3 m_forward;
  glm::vec3 m_right;

  float m_fov = 60.0f;
  float m_clip_near = 0.1f;
  float m_clip_far = 10000.0f;
};
