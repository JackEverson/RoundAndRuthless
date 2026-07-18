#pragma once
#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <functional>
#include <string>

enum class TriggerType { Interact, Proximity, LookAt };

struct TriggerVolume {
    TriggerType type = TriggerType::Proximity;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 size = glm::vec3(1.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    float interaction_distance = 3.0f;
    std::string label; // shown by the scene when aimed at ("" = no note)
    std::function<void()> on_triggered;
    
    float time_to_trigger = 0.0f;
    float trigger_timer = 0.0f;
    bool triggered = false;
    bool single_trigger = false;

    void Fuse(float delta){
        trigger_timer += delta;
        if (trigger_timer > time_to_trigger) Fire();
    }

    void Fire(){
        if (!triggered && on_triggered) {
            on_triggered();
            triggered = true;
        }
    }

    void Reset(){
        if (!single_trigger) triggered = false;
        trigger_timer = 0;
    }
};