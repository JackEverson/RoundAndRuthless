#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

struct Interactable {
    glm::vec3 position;
    glm::vec3 size;
    glm::mat4 rotation = glm::mat4(1.0f);
    float interaction_distance = 3.0f;
    std::function<void()> on_interact;
};