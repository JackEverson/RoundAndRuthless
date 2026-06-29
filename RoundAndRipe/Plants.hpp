#pragma once

#include "Texture.hpp"

#include "glm/ext/vector_float2.hpp"

#include <string>


enum class PlantType { Harvestable, Producing };

struct PlantDef {
    std::string name;
    PlantType   type;
    int days_to_grow = 1;    
    int days_to_ripen = 1;
    int biomass_yield = 1;  
    Texture*    growing_texture;         
    Texture*    ripe_texture;         
    glm::vec2   full_size;     
};

inline PlantDef Radish(Texture* growing, Texture* ripe){
    PlantDef radish;
    radish.name = "radish";
    radish.type = PlantType::Harvestable;
    radish.days_to_grow = 3;
    radish.biomass_yield = 1;
    radish.growing_texture = growing;
    radish.ripe_texture = ripe;
    radish.full_size = glm::vec2(0.8f, 0.8f);
    return radish;
}

inline PlantDef Carrot(Texture* growing, Texture* ripe){
    PlantDef carrot;
    carrot.name = "carrot";
    carrot.type = PlantType::Harvestable;
    carrot.days_to_grow = 10;
    carrot.biomass_yield = 5;
    carrot.growing_texture = growing;
    carrot.ripe_texture = ripe;
    carrot.full_size = glm::vec2(0.8f, 0.8f);
    return carrot;
}

inline PlantDef Tomato(Texture* growing, Texture* ripe){
    PlantDef tomato;
    tomato.name = "tomato";
    tomato.type = PlantType::Producing;
    tomato.days_to_grow = 5;
    tomato.days_to_ripen = 2;
    tomato.biomass_yield = 1;
    tomato.growing_texture = growing;
    tomato.ripe_texture = ripe;
    tomato.full_size = glm::vec2(0.8f, 1.4f);
    return tomato;
}