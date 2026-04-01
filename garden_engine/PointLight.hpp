#pragma once
#include <glm/glm.hpp>

struct PointLight {
    glm::vec3 position;
    glm::vec3 color = glm::vec3(1.0f);
    float radius = 5.0f;
};
