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
    int regrow_days = 1;
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

inline PlantDef Carrot(Texture* sprite){
    PlantDef carrot;
    carrot.name = "carrot";
    carrot.type = PlantType::Harvestable;
    carrot.days_to_ripen = 10;
    carrot.biomass_yield = 5;
    carrot.texture = sprite;
    carrot.full_size = glm::vec2(0.8f, 0.8f);
    return carrot;
}

inline PlantDef Tomato(Texture* sprite){
    PlantDef tomato;
    tomato.name = "tomato";
    tomato.type = PlantType::Producing;
    tomato.days_to_ripen = 5;
    tomato.biomass_yield = 1;
    tomato.texture = sprite;
    tomato.full_size = glm::vec2(0.8f, 1.4f);
    tomato.regrow_days = 2;
    return tomato;
}