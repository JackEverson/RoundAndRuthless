#pragma once

#include "Plants.hpp"
#include "Renderer.hpp"

#include "Texture.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>


class Plot {
public:
    enum class State { Empty, Growing, Ripe };
    
    Plot(glm::vec3 position, Texture* soil_texture, Texture* pot_texture);

    void Update(float delta);
    void Render(Renderer& renderer, glm::vec3& campos);
    void Plant(PlantDef* plant);
    float Harvest();

private:

    const float PLOT_SIZE = 1.0f;
    const float SOIL_OFFSET = 0.05f;
    glm::vec3 m_position;
    std::vector<SpriteInstance> m_bed;

    float m_growth_timer = 0.0f;
    State m_state = State::Empty;
    const PlantDef* m_plant = nullptr;
    SpriteInstance m_plant_sprite;
};

inline Plot::Plot(glm::vec3 position, Texture* soil_texture, Texture* pot_texture) :
m_position(position)
{
    glm::vec2 side_size(PLOT_SIZE, PLOT_SIZE / 2);
    glm::vec2 top_size(PLOT_SIZE, PLOT_SIZE);
    
    SpriteInstance front;
    front.size = side_size;
    front.position = position + glm::vec3(0, PLOT_SIZE / 4, PLOT_SIZE / 2);
    front.texture = pot_texture;
    front.color = glm::vec4(1, 1, 1, 1);
    m_bed.push_back(front);

    SpriteInstance back;
    back.size = side_size;
    back.position = position + glm::vec3(0, PLOT_SIZE / 4, -PLOT_SIZE / 2);
    back.texture = pot_texture;
    back.color = glm::vec4(1, 1, 1, 1);
    m_bed.push_back(back);

    SpriteInstance side1;
    side1.size = side_size;
    side1.position = position + glm::vec3(PLOT_SIZE / 2, PLOT_SIZE / 4, 0);
    side1.texture = pot_texture;
    side1.color = glm::vec4(1, 1, 1, 1);
    side1.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0));
    m_bed.push_back(side1);

    SpriteInstance side2;
    side2.size = side_size;
    side2.position = position + glm::vec3(-PLOT_SIZE / 2, PLOT_SIZE / 4, 0);
    side2.texture = pot_texture;
    side2.color = glm::vec4(1, 1, 1, 1);
    side2.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
    m_bed.push_back(side2);

    // SpriteInstance base;
    // base.size = top_size;
    // base.position = position;
    // base.texture = pot_texture;
    // base.color = glm::vec4(1, 1, 1, 1);
    // base.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    // m_bed.push_back(base);

    SpriteInstance soil;
    soil.size = top_size;
    soil.position = position + glm::vec3(0, PLOT_SIZE / 2 - SOIL_OFFSET, 0);
    soil.texture = soil_texture;
    soil.color = glm::vec4(0.66f, 0.60f, 0.50f, 1.0f);
    soil.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
    m_bed.push_back(soil);

}

inline void Plot::Update(float delta) {
    if (m_state == State::Empty) return;

    m_growth_timer += delta;
    m_plant_sprite.size = m_plant->full_size * std::min((m_growth_timer / m_plant->growth_time), 1.0f);
    
    float soil_surface_y = m_position.y + (PLOT_SIZE / 2 - SOIL_OFFSET);
    m_plant_sprite.position.y = soil_surface_y + (m_plant_sprite.size.y / 2);   
}

inline void Plot::Render(Renderer& renderer, glm::vec3& campos){
    
    for (auto &p:m_bed){
        renderer.SubmitSprite(p);
    }

    if (m_state != State::Empty) {
    // non-billboarding
        //     renderer.SubmitTransparentSprite(m_plant_sprite);

    // billboarding
        glm::vec3 to_cam = campos - m_plant_sprite.position;
        float yaw = std::atan2(to_cam.x, to_cam.z);          // angle around Y
        m_plant_sprite.model_mat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0,1,0));
        renderer.SubmitTransparentSprite(m_plant_sprite);
    }

}

inline void Plot::Plant(PlantDef* plant){
    m_growth_timer = 0;
    m_plant = plant;
    m_state = State::Growing;

    SpriteInstance plant_sprite;
    plant_sprite.position = glm::vec3(m_position) + glm::vec3(0, SOIL_OFFSET, 0);
    plant_sprite.size = glm::vec2(0.0f);
    plant_sprite.texture = plant->texture;
    m_plant_sprite = plant_sprite;
}

inline float Plot::Harvest(){
    float yield = m_plant->biomass_yield;
    m_plant = nullptr;
    m_state = State::Empty;
    m_growth_timer = 0.0f;
    return yield;
}