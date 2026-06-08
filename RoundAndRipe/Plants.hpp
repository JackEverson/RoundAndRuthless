#pragma once

#include "Texture.hpp"

#include "glm/ext/vector_float2.hpp"

#include <string>


enum class PlantType { Harvestable, Producing };

struct PlantDef {
    std::string name;
    PlantType   type;
    int days_to_ripen;    
    int biomass_yield;  
    Texture*    texture;         
    glm::vec2   full_size;     
    // later: produce_interval, produce_yield (producers); seed/plot cost
};


inline PlantDef Radish(Texture* sprite){
    PlantDef radish;
    radish.name = "radish";
    radish.type = PlantType::Harvestable;
    radish.days_to_ripen = 3;
    radish.biomass_yield = 1;
    radish.texture = sprite;
    radish.full_size = glm::vec2(0.8f, 0.8f);
    return radish;
}

