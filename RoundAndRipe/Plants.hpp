#pragma once

#include "Texture.hpp"

#include "glm/ext/vector_float2.hpp"

#include <string>


enum class PlantType { Harvestable, Producing };

struct PlantDef {
    std::string name;
    PlantType   type;
    int seconds_to_grow = 60;    
    int seconds_to_ripen = 60;
    int biomass_yield = 1;  
    int biomass_cost = 1;
    Texture*    growing_texture;         
    Texture*    ripe_texture;         
    glm::vec2   full_size;     
};

inline PlantDef Radish(Texture* growing, Texture* ripe){
    PlantDef radish;
    radish.name = "radish";
    radish.type = PlantType::Harvestable;
    radish.seconds_to_grow = 20;
    radish.biomass_yield = 2;
    radish.biomass_cost = 1;
    radish.growing_texture = growing;
    radish.ripe_texture = ripe;
    radish.full_size = glm::vec2(0.8f, 0.8f);
    return radish;
}

inline PlantDef Carrot(Texture* growing, Texture* ripe){
    PlantDef carrot;
    carrot.name = "carrot";
    carrot.type = PlantType::Harvestable;
    carrot.seconds_to_grow = 60;
    carrot.biomass_yield = 10;
    carrot.biomass_cost = 3;
    carrot.growing_texture = growing;
    carrot.ripe_texture = ripe;
    carrot.full_size = glm::vec2(0.8f, 0.8f);
    return carrot;
}

inline PlantDef Tomato(Texture* growing, Texture* ripe){
    PlantDef tomato;
    tomato.name = "tomato";
    tomato.type = PlantType::Producing;
    tomato.seconds_to_grow = 60;
    tomato.seconds_to_ripen = 30;
    tomato.biomass_yield = 1;
    tomato.biomass_cost = 5;
    tomato.growing_texture = growing;
    tomato.ripe_texture = ripe;
    tomato.full_size = glm::vec2(0.8f, 1.4f);
    return tomato;
}