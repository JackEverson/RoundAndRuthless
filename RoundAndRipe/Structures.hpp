#pragma once

#include "Plants.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"


enum class StructureKind { Sprinkler, Harvester, Hoer, Planter };

struct StructureDef {
  std::string name;
  std::string description;
  StructureKind kind;
  int biomass_cost;
  int tier;
  float interval = 0.0f;
  int seed_load = 0;
  Texture* texture; 
  Texture* texture2 = nullptr; 
  glm::vec2 full_size;
};

class Structure {
  public:
  const StructureDef* def;
  int row, col;
  float timer = 0.0f;    
  SpriteInstance sprite;
  const PlantDef* crop = nullptr;
};



inline StructureDef Sprinkler(Texture* tex){
    StructureDef s; 
    s.name = "sprinkler";
    s.description = "Keeps the 8 surrounding tiles watered.";
    s.kind = StructureKind::Sprinkler;
    s.biomass_cost = 50;
    s.tier = 1;
    s.interval = 2.0f;             
    s.texture = tex;
    s.full_size = glm::vec2(0.8f, 0.8f);
    return s;
}

inline StructureDef Harvester(Texture* tex){
    StructureDef s;
    s.name = "harvester";
    s.description = "Collects ripe crops from the 8 surrounding tiles.";
    s.kind = StructureKind::Harvester;
    s.biomass_cost = 200;
    s.tier = 2;
    s.interval = 2.0f;
    s.texture = tex;
    s.full_size = glm::vec2(0.8f, 0.8f);
    return s;
}

inline StructureDef Hoer(Texture* tex){
    StructureDef s;
    s.name = "hoer";
    s.description = "Tills empty ground in the 8 surrounding tiles.";
    s.kind = StructureKind::Hoer;
    s.biomass_cost = 500;
    s.tier = 3;
    s.interval = 3.0f;
    s.texture = tex;
    s.full_size = glm::vec2(0.8f, 0.8f);
    return s;
}

inline StructureDef Planter(Texture* unloaded, Texture* loaded){
    StructureDef s;
    s.name = "planter";
    s.description = "Plants its bound crop in tilled ground around it. Loading it costs 100 seeds.";
    s.kind = StructureKind::Planter;
    s.biomass_cost = 500;
    s.tier = 3;
    s.interval = 3.0f;
    s.seed_load = 100;             
    s.texture = unloaded;
    s.texture2 = loaded;
    s.full_size = glm::vec2(0.8f, 0.8f);
    return s;
}

