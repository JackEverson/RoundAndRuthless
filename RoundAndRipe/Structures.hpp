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
  float interval;        // seconds between sweeps (0 = continuous)
  int seed_load = 0;     // planter: seeds consumed at placement to bind a crop
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


// The automation ladder: each tier hands the player one more verb of the loop.
// All act on the 8 tiles surrounding their own. Costs/intervals = first-draft tunables.

inline StructureDef Sprinkler(Texture* tex){
    StructureDef s;
    s.name = "sprinkler";
    s.description = "Keeps the 8 surrounding tiles watered.";
    s.kind = StructureKind::Sprinkler;
    s.biomass_cost = 25;
    s.tier = 1;
    s.interval = 0.0f;             // continuous — tops neighbours up every frame
    s.texture = tex;
    s.full_size = glm::vec2(0.8f, 0.8f);
    return s;
}

inline StructureDef Harvester(Texture* tex){
    StructureDef s;
    s.name = "harvester";
    s.description = "Collects ripe crops from the 8 surrounding tiles.";
    s.kind = StructureKind::Harvester;
    s.biomass_cost = 150;
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
    s.biomass_cost = 750;
    s.tier = 3;
    s.interval = 3.0f;
    s.texture = tex;
    s.full_size = glm::vec2(0.8f, 0.8f);
    return s;
}

inline StructureDef Planter(Texture* tex){
    StructureDef s;
    s.name = "planter";
    s.description = "Plants its bound crop in tilled ground around it. Loading it costs 100 seeds.";
    s.kind = StructureKind::Planter;
    s.biomass_cost = 3000;
    s.tier = 4;
    s.interval = 3.0f;
    s.seed_load = 100;             // one-time load at placement, binds the crop
    s.texture = tex;
    s.full_size = glm::vec2(0.8f, 0.8f);
    return s;
}

