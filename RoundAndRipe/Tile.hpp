#pragma once

#include "Plants.hpp"
#include "Renderer.hpp"

#include "Texture.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class Tile {
public:
    enum class TileState { Refuse, Empty, Tilled, Growing, Ripe };
    
    Tile(glm::vec3 position, Texture* soil_texture, Texture* refuse_texture, Texture* tilled_texture, Texture* seeded_texture, TileState state = TileState::Refuse);

    void Advance();
    void Render(Renderer& renderer, const glm::vec3& campos);

    void Plant(const PlantDef* plant);
    void Set(TileState state, bool watered, const PlantDef* plant, int days_growing);
    int Harvest();
    void Water();
    void PullUp();
    void Clear() { if (m_state == TileState::Refuse) m_state = TileState::Empty; }
    void Till() { if (m_state == TileState::Empty) m_state = TileState::Tilled; }
    
    const TileState& GetState() const { return m_state; }
    int DaysGrowing() { return m_days_growing; }

    bool IsRipe()   const { return m_state == TileState::Ripe; }
    bool IsTilled()   const { return m_state == TileState::Tilled; }
    bool IsGrowing() const { return m_state == TileState::Growing; }
    bool IsRefuse() const {return m_state == TileState::Refuse; }
    bool IsEmpty() const { return m_state == TileState::Empty; }
    bool IsWatered() const { return m_watered; }
    bool HasPlant() const { return m_plant != nullptr; }

    glm::vec3 Position() const { return m_position; }

    std::optional<PointLight> RipeLight() const{ if (IsRipe()) return m_ripe_light; else return std::nullopt;};
    std::string GetPlantName() const { return m_plant ? m_plant->name : ""; } 

private:
    const glm::vec4 SOIL_COLOR = glm::vec4(0.62f, 0.52f, 0.40f, 1.0f);
    const float WET_FACTOR = 0.5;
    const float PLOT_SIZE = 1.0f;
    const float SOIL_OFFSET = 0.05f;

    void RefreshState();

    glm::vec3 m_position;
    PointLight m_ripe_light;

    SpriteInstance m_soil_sprite;
    SpriteInstance m_till_sprite;
    SpriteInstance m_seeded_sprite;
    SpriteInstance m_refuse_sprite;

    int m_days_growing = 0;
    TileState m_state = TileState::Empty;
    bool m_watered = false;
    const PlantDef* m_plant = nullptr;
    SpriteInstance m_plant_sprite;
};

inline Tile::Tile(glm::vec3 position, Texture* soil_texture, Texture* refuse_texture, Texture* tilled_texture, Texture* seeded_texture, TileState state) :
m_position(position),
m_state(state)
{
    glm::vec2 top_size(PLOT_SIZE, PLOT_SIZE);

    m_ripe_light.position = position;
    m_ripe_light.color = glm::vec3(0.85f, 0.92f, 1.0f) / 3.0f;;
    m_ripe_light.radius = PLOT_SIZE / 2;

    m_soil_sprite.size = top_size;
    m_soil_sprite.position = position + glm::vec3(0.0f, 0.001f, 0.0f);
    m_soil_sprite.texture = soil_texture;
    m_soil_sprite.color = SOIL_COLOR;
    m_soil_sprite.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

    m_till_sprite.size = top_size;
    m_till_sprite.position = position + glm::vec3(0.0f, 0.002f, 0.0f);
    m_till_sprite.texture = tilled_texture;
    m_till_sprite.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

    m_seeded_sprite.size = top_size;
    m_seeded_sprite.position = position + glm::vec3(0.0f, 0.002f, 0.0f);
    m_seeded_sprite.texture = seeded_texture;
    m_seeded_sprite.model_mat = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));

    m_refuse_sprite.position = glm::vec3(m_position) + glm::vec3(0, SOIL_OFFSET, 0);
    m_refuse_sprite.size = glm::vec2(0.8f);
    m_refuse_sprite.texture = refuse_texture;
}

inline void Tile::Advance(){
    bool was_watered = m_watered;
    m_watered = false; 
    m_soil_sprite.color = SOIL_COLOR;
    if (m_state != TileState::Growing) return;
    if (was_watered) {                       // only grows if it was watered today
        m_days_growing++;
        RefreshState();
    }
}

inline void Tile::Render(Renderer& renderer, const glm::vec3& campos){
    
    // for (auto &p:m_bed){
    //     renderer.SubmitSprite(p);
    // }
    renderer.SubmitSprite(m_soil_sprite);

    switch (m_state) {

        case TileState::Refuse:
        renderer.SubmitTransparentSprite(m_refuse_sprite);
        break;

        case TileState::Empty:
        break;
        
        case TileState::Tilled:
        renderer.SubmitTransparentSprite(m_till_sprite);
        break;

        case TileState::Growing:
        if ((float)m_days_growing / (float)m_plant->days_to_ripen < 0.1f){
            renderer.SubmitTransparentSprite(m_seeded_sprite);
        } 
        case TileState::Ripe:
        glm::vec3 to_cam = campos - m_plant_sprite.position;
        float yaw = std::atan2(to_cam.x, to_cam.z);          // angle around Y
        m_plant_sprite.model_mat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0,1,0));
        renderer.SubmitTransparentSprite(m_plant_sprite);
        break;
    }
}

inline void Tile::Plant(const PlantDef* plant){
    m_days_growing = 0;
    m_plant = plant;
    m_state = TileState::Growing;

    SpriteInstance plant_sprite;
    plant_sprite.position = glm::vec3(m_position) + glm::vec3(0, SOIL_OFFSET, 0);
    plant_sprite.size = glm::vec2(0.0f);
    plant_sprite.texture = plant->texture;
    m_plant_sprite = plant_sprite;
}

inline void Tile::Set(TileState state, bool watered, const PlantDef* plant, int days_growing){
    if (plant) {
        Plant(plant);
        m_days_growing = days_growing;
        RefreshState();
    } else{
        m_plant = nullptr;
        m_state = (state == TileState::Growing || state == TileState::Ripe)
            ? TileState::Empty
            : state;            // refuse / empty / tilled load
    }
    m_watered = watered;
}

inline void Tile::RefreshState(){
    float t = std::min((float)m_days_growing / m_plant->days_to_ripen, 1.0f);
    m_plant_sprite.size = m_plant->full_size * t;
    m_plant_sprite.position.y = m_position.y + (m_plant_sprite.size.y / 2);
    if (m_days_growing >= m_plant->days_to_ripen) m_state = TileState::Ripe;
}

inline void Tile::Water(){
    m_watered = true;
    m_soil_sprite.color = SOIL_COLOR * glm::vec4(WET_FACTOR, WET_FACTOR, WET_FACTOR, 1.0f);
}

inline int Tile::Harvest(){
int yield = m_plant->biomass_yield;

    if (m_plant->type == PlantType::Producing) {
        m_days_growing = m_plant->days_to_ripen - m_plant->regrow_days; // rewind
        m_state = TileState::Growing;
        RefreshState();   // shrinks sprite + drops it out of Ripe
    } else {
        PullUp();
    }
    return yield;
}

inline void Tile::PullUp(){
    m_plant = nullptr;
    m_state = TileState::Empty;
    m_days_growing = 0;
}
