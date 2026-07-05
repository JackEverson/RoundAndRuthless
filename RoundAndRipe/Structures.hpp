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
  float interval; 
  Texture* texture; glm::vec2 full_size;
};

class Structure {              // a placed instance
  public:
  const StructureDef* def;    // borrowed, resolved by name on load — same as crops
  int row, col;               // grid-anchored
  float timer = 0.0f;         // per-instance state (harvester sweep)
  SpriteInstance sprite;
  const PlantDef* crop = nullptr;
};


// inline PlantDef Sprinkler(Texture* tex){
//     PlantDef sprinkler;
//     sprinkler.name = "sprinkler";
//     sprinkler.type = PlantType::Structure;
//     sprinkler.seconds_to_grow = 0;
//     sprinkler.biomass_yield = 0;
//     sprinkler.biomass_cost = 25;
//     sprinkler.growing_texture = tex;
//     sprinkler.ripe_texture = tex;
//     sprinkler.full_size = glm::vec2(0.8f, 0.8f);
//     sprinkler.tier = 1;
//     return sprinkler;
// }

