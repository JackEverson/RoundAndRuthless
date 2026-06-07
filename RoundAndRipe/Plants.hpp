#pragma once

#include "Texture.hpp"

#include "glm/ext/vector_float2.hpp"

#include <string>


enum class PlantType { Harvestable, Producing };

struct PlantDef {
    std::string name;
    PlantType   type;
    float       growth_time;    // seconds to fully grow
    float         biomass_yield;  // granted on harvest (used in Pass B)
    Texture*    texture;         // the plant billboard texture
    glm::vec2   full_size;      // size when fully grown
    // later: produce_interval, produce_yield (producers); seed/plot cost
};


inline PlantDef Radish(Texture* sprite){
    PlantDef radish;
    radish.name = "radish";
    radish.type = PlantType::Harvestable;
    radish.growth_time = 10.0f;
    radish.biomass_yield = 0.01f;
    radish.texture = sprite;
    radish.full_size = glm::vec2(0.8f, 0.8f);
    return radish;
}

