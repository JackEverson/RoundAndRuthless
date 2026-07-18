#pragma once

#include "Texture.hpp"
#include "glm/glm.hpp"

enum class SurfaceType { Floor, Wall, Ceiling };

struct Surface {
  SurfaceType type = SurfaceType::Wall;
  glm::vec3 position;
  glm::vec2 size;
  glm::mat4 rotation;
  glm::vec4 color = glm::vec4(1.0f);
  glm::vec2 uv_scale = glm::vec2(1.0f);   // >1 tiles the texture across the surface
  Texture *texture = nullptr;
  bool visible = true;   // false = collision-only (e.g. tall collider over a low fence)

  Surface() : position(0.0f), size(1.0f), rotation(1.0f) {}
};
