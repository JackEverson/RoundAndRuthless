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
  Texture *texture = nullptr;

  Surface() : position(0.0f), size(1.0f), rotation(1.0f) {}
};
