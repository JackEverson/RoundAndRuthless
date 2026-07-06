#pragma once

#include "Texture.hpp"

#include "glm/ext/vector_float2.hpp"

#include <string>


enum class PlantType { Harvestable, Producing};

struct PlantDef {
    std::string name;
    PlantType   type;
    int seconds_to_grow = 60;    
    int seconds_to_ripen = 60;
    int biomass_yield = 1;  
    int biomass_cost = 1;
    int tier = 0;
    Texture*    growing_texture;         
    Texture*    ripe_texture;         
    glm::vec2   full_size;     
};

inline PlantDef Radish(Texture* growing, Texture* ripe){
    PlantDef radish;
    radish.name = "radish";
    radish.type = PlantType::Harvestable;
    radish.seconds_to_grow = 20;
    radish.biomass_yield = 4;
    radish.biomass_cost = 1;
    radish.growing_texture = growing;
    radish.ripe_texture = ripe;
    radish.full_size = glm::vec2(0.8f, 0.8f);
    radish.tier = 0;
    return radish;
}

inline PlantDef Blueberry(Texture* growing, Texture* ripe){
    PlantDef blueberry;
    blueberry.name = "blueberry";
    blueberry.type = PlantType::Producing;
    blueberry.seconds_to_grow = 40;
    blueberry.seconds_to_ripen = 30;
    blueberry.biomass_yield = 6;
    blueberry.biomass_cost = 6;
    blueberry.growing_texture = growing;
    blueberry.ripe_texture = ripe;
    blueberry.full_size = glm::vec2(0.8f, 1.4f);
    blueberry.tier = 0;
    return blueberry;
}

inline PlantDef Turnip(Texture* growing, Texture* ripe){
    PlantDef turnip;
    turnip.name = "turnip";
    turnip.type = PlantType::Harvestable;
    turnip.seconds_to_grow = 40;
    turnip.biomass_yield = 40;
    turnip.biomass_cost = 10;
    turnip.growing_texture = growing;
    turnip.ripe_texture = ripe;
    turnip.full_size = glm::vec2(0.8f, 0.8f);
    turnip.tier = 1;
    return turnip;
}

inline PlantDef Tomato(Texture* growing, Texture* ripe){
    PlantDef tomato;
    tomato.name = "tomato";
    tomato.type = PlantType::Producing;
    tomato.seconds_to_grow = 40;
    tomato.seconds_to_ripen = 30;
    tomato.biomass_yield = 20;
    tomato.biomass_cost = 20;
    tomato.growing_texture = growing;
    tomato.ripe_texture = ripe;
    tomato.full_size = glm::vec2(0.8f, 1.4f);
    tomato.tier = 1;
    return tomato;
}

inline PlantDef StaringCabbage(Texture* growing, Texture* ripe){
    PlantDef cabbage;
    cabbage.name = "staring cabbage";
    cabbage.type = PlantType::Harvestable;
    cabbage.seconds_to_grow = 60;
    cabbage.biomass_yield = 200;
    cabbage.biomass_cost = 50;
    cabbage.growing_texture = growing;
    cabbage.ripe_texture = ripe;
    cabbage.full_size = glm::vec2(0.9f, 0.9f);
    cabbage.tier = 2;
    return cabbage;
}

inline PlantDef Tier2Producing(Texture* growing, Texture* ripe){
    PlantDef crop;
    crop.name = "producingplanttier2";
    crop.type = PlantType::Producing;
    crop.seconds_to_grow = 60;
    crop.seconds_to_ripen = 30;
    crop.biomass_yield = 100;       // half the cabbage's, but repeats
    crop.biomass_cost = 100;
    crop.growing_texture = growing;
    crop.ripe_texture = ripe;
    crop.full_size = glm::vec2(0.9f, 1.4f);
    crop.tier = 2;
    return crop;
}

inline PlantDef Tier3Havest(Texture* growing, Texture* ripe){
    PlantDef crop;
    crop.name = "tier 3 placeholder havestable";
    crop.type = PlantType::Harvestable;
    crop.seconds_to_grow = 80;
    crop.biomass_yield = 500;      // ~2000 harvests to clear the 1M gate
    crop.biomass_cost = 100;
    crop.growing_texture = growing;
    crop.ripe_texture = ripe;
    crop.full_size = glm::vec2(1.0f, 1.0f);
    crop.tier = 3;
    return crop;
}

inline PlantDef Tier3Producing(Texture* growing, Texture* ripe){
    PlantDef crop;
    crop.name = "producingplanttier3";
    crop.type = PlantType::Producing;
    crop.seconds_to_grow = 80;
    crop.seconds_to_ripen = 30;
    crop.biomass_yield = 250;       // half the harvestable's, but repeats
    crop.biomass_cost = 200;
    crop.growing_texture = growing;
    crop.ripe_texture = ripe;
    crop.full_size = glm::vec2(1.0f, 1.4f);
    crop.tier = 3;
    return crop;
}

inline PlantDef Tier4Placeholder(Texture* growing, Texture* ripe){
    PlantDef crop;
    crop.name = "tier4placeholderhavestable";
    crop.type = PlantType::Harvestable;
    crop.seconds_to_grow = 100;
    crop.biomass_yield = 500000;   // ~2000 harvests to clear the 1B gate
    crop.biomass_cost = 100000;
    crop.growing_texture = growing;
    crop.ripe_texture = ripe;
    crop.full_size = glm::vec2(1.1f, 1.1f);
    crop.tier = 4;
    return crop;
}

inline PlantDef Tier4Producing(Texture* growing, Texture* ripe){
    PlantDef crop;
    crop.name = "producingplanttier4";
    crop.type = PlantType::Producing;
    crop.seconds_to_grow = 100;
    crop.seconds_to_ripen = 30;
    crop.biomass_yield = 250000;    // half the harvestable's, but repeats
    crop.biomass_cost = 200000;
    crop.growing_texture = growing;
    crop.ripe_texture = ripe;
    crop.full_size = glm::vec2(1.1f, 1.5f);
    crop.tier = 4;
    return crop;
}
