#pragma once

#include "Plants.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

#include <algorithm>
#include <string>

class Tile {
public:
    enum class TileState { Refuse, Empty, Tilled, Growing, Grown, Ripe };
    
    Tile(glm::vec3 position, glm::vec4 soil_color, Texture* soil_texture, Texture* refuse_texture, Texture* tilled_texture, Texture* seeded_texture, TileState state = TileState::Empty);

    void Update(float dt); 
    void Render(Renderer& renderer, const glm::vec3& campos);

    void Plant(const PlantDef* plant);
    void Set(TileState state, const PlantDef* plant, int days_growing);
    int Harvest();
    void Water();
    void PullUp();
    void Clear() { if (m_state == TileState::Refuse || m_state == TileState::Tilled) m_state = TileState::Empty; }
    void Till() { if (m_state == TileState::Empty) m_state = TileState::Tilled; }
    void SetStructure(bool s) {m_structure = s; }

    const TileState& GetState() const { return m_state; }
    int DaysGrowing() { return m_seconds_growing; }

    bool IsHarvestable()   const;
    bool IsTilled()   const { return m_state == TileState::Tilled; }
    bool IsGrowing() const { return m_state == TileState::Growing; }
    bool IsRefuse() const {return m_state == TileState::Refuse; }
    bool IsEmpty() const { return m_state == TileState::Empty; }
    bool IsWatered() const { return m_watered; }
    bool HasPlant() const { return m_plant != nullptr; }
    bool HasStructure() const { return m_structure; }


    glm::vec3 Position() const { return m_position; }
    float SecondsGrowing() const { return m_seconds_growing; }
    std::string GetPlantName() const { return m_plant ? m_plant->name : ""; } 

private:
    glm::vec4 m_soil_color = glm::vec4(1.0f);
    const float WET_FACTOR = 0.5;
    const float PLOT_SIZE = 1.0f;
    const float SOIL_OFFSET = 0.05f;
    const float WATER_DURATION = 30.0f; // seconds until soil dries out
    float m_water_timer = 0.0f;

    void RefreshState();

    glm::vec3 m_position;

    SpriteInstance m_soil_sprite;
    SpriteInstance m_till_sprite;
    SpriteInstance m_seeded_sprite;
    SpriteInstance m_refuse_sprite;

    float m_seconds_growing = 0.0f; 
    TileState m_state = TileState::Empty;
    bool m_watered = false;
    bool m_structure = false;
    const PlantDef* m_plant = nullptr;
    SpriteInstance m_growing_sprite;
    SpriteInstance m_ripe_sprite;

};

inline Tile::Tile(glm::vec3 position, glm::vec4 soil_color, Texture* soil_texture, Texture* refuse_texture, Texture* tilled_texture, Texture* seeded_texture, TileState state) :
m_position(position),
m_state(state),
m_soil_color(soil_color)
{ 

    glm::vec2 top_size(PLOT_SIZE, PLOT_SIZE);

    m_soil_sprite.size = top_size;
    m_soil_sprite.position = position + glm::vec3(0.0f, 0.001f, 0.0f);
    m_soil_sprite.texture = soil_texture;
    m_soil_sprite.color = m_soil_color;
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

inline void Tile::Update(float dt){
    RefreshState();
    if (m_watered) {
        m_water_timer += dt;
        if (m_water_timer >= WATER_DURATION) {
            m_watered = false;
            m_soil_sprite.color = m_soil_color;
            m_water_timer = 0.0f;
        }
    }
    else { return; }

    if (!m_plant) return;
    if (m_plant->type == PlantType::Harvestable && m_state != TileState::Growing) return;
    if (m_plant->type == PlantType::Producing && m_state == TileState::Ripe) return;
    m_seconds_growing += dt;   // dt is seconds; seconds_to_grow/ripen now read as seconds
}

inline void Tile::Render(Renderer& renderer, const glm::vec3& campos){
    
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
        if ((float)m_seconds_growing / (float)m_plant->seconds_to_grow < 0.1f){
            renderer.SubmitTransparentSprite(m_seeded_sprite);
        } 
        case TileState::Grown:
        case TileState::Ripe:
        glm::vec3 to_cam = campos - m_growing_sprite.position;
        float yaw = std::atan2(to_cam.x, to_cam.z);          // angle around Y
        
        if ((m_plant->type == PlantType::Harvestable && m_seconds_growing < m_plant->seconds_to_grow) || // harvestable
            (m_plant->type == PlantType::Producing && m_seconds_growing < m_plant->seconds_to_grow + m_plant->seconds_to_ripen)) // producing
        {   
            m_growing_sprite.model_mat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0,1,0));
            renderer.SubmitTransparentSprite(m_growing_sprite);
        }
        else{
            m_ripe_sprite.model_mat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0,1,0));
            renderer.SubmitTransparentSprite(m_ripe_sprite);
        }
        break;
    }
}

inline void Tile::Plant(const PlantDef* plant){
    m_seconds_growing = 0;
    m_plant = plant;
    m_state = TileState::Growing;

    SpriteInstance grow_sprite;
    grow_sprite.position = glm::vec3(m_position) + glm::vec3(0, SOIL_OFFSET, 0);
    grow_sprite.size = glm::vec2(0.0f);
    grow_sprite.texture = plant->growing_texture;
    m_growing_sprite = grow_sprite;

    SpriteInstance ripe_sprite;
    ripe_sprite.position = glm::vec3(m_position) + glm::vec3(0, SOIL_OFFSET + (plant->full_size.y / 2), 0);
    ripe_sprite.size = plant->full_size;
    ripe_sprite.texture = plant->ripe_texture;
    m_ripe_sprite = ripe_sprite;
}

inline void Tile::Set(TileState state, const PlantDef* plant, int days_growing){
    if (plant) {
        Plant(plant);
        m_seconds_growing = days_growing;
    } else{
        m_plant = nullptr;
        m_state = (state == TileState::Growing || state == TileState::Grown || state == TileState::Ripe)
            ? TileState::Empty
            : state;            // refuse / empty / tilled load
    }
}

inline void Tile::RefreshState(){
    if (m_watered) m_soil_sprite.color = m_soil_color * glm::vec4(WET_FACTOR, WET_FACTOR, WET_FACTOR, 1.0f);
    else m_soil_sprite.color = m_soil_color;
    
    if (!m_plant) return;
    float t = std::min((float)m_seconds_growing / m_plant->seconds_to_grow, 1.0f);
    m_growing_sprite.size = m_plant->full_size * t;
    m_growing_sprite.position.y = m_position.y + (m_growing_sprite.size.y / 2);

    m_ripe_sprite.size = m_plant->full_size;
    m_ripe_sprite.position = glm::vec3(m_position) + glm::vec3(0, SOIL_OFFSET + (m_plant->full_size.y / 2), 0);

    if (m_seconds_growing >= m_plant->seconds_to_grow) m_state = TileState::Grown;
    if (m_seconds_growing >= m_plant->seconds_to_grow + m_plant->seconds_to_ripen) m_state = TileState::Ripe;
}

inline void Tile::Water(){
    m_watered = true;
    m_water_timer = 0.0f;
}

inline int Tile::Harvest(){
    int yield = m_plant->biomass_yield;

    if (m_plant->type == PlantType::Producing) {
        m_seconds_growing = m_plant->seconds_to_grow; 
        m_state = TileState::Grown;
        RefreshState();   
    } else {
        PullUp();
    }
    return yield;
}

inline void Tile::PullUp(){
    m_plant = nullptr;
    m_state = TileState::Empty;
    m_seconds_growing = 0;
}

inline bool Tile::IsHarvestable() const{
    if (!m_plant) return false;
    if (m_structure) return false;
    return (m_plant->type == PlantType::Producing) ? m_state == TileState::Ripe
                                                   : m_state == TileState::Grown;
}