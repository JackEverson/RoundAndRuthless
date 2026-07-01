#pragma once

#include "Event.hpp"
#include "GardenRoom.hpp"
#include "NotificationManager.hpp"
#include "Plants.hpp"
#include "PointLight.hpp"
#include "SaveSystem.hpp"
#include "Texture.hpp"
#include "Tile.hpp"
#include "Field.hpp"
#include "Renderer.hpp" 

#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

#include <cstddef>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

class GardenScene : public GardenRoom {

private:
  SimpleSoundManager &sound_manager;

  // Textures
  Texture m_wall_texture;
  Texture m_floor_texture;

  Texture m_sushi_texture;
  Texture m_house_texture;
  Texture m_chest_texture;
  Texture m_human_texture;

  Texture m_seed_maker_texture;
  Texture m_soil_texture;
  Texture m_rock_texture;
  Texture m_till_texture;
  Texture m_seeded_texture;

  Texture m_shovel_texture;
  Texture m_hoe_texture;
  Texture m_can_texture;
  Texture m_packet_texture;

  Texture m_veg_top_texture;
  Texture m_apple_texture;
  Texture m_radish_texture;
  Texture m_turnip_texture;
  Texture m_bush_texture;
  Texture m_tomato_texture;
  Texture m_blueberry_texture;
  Texture m_staring_cabbage_growing_texture;
  Texture m_staring_cabbage_ripe_texture;


  // enums and struct
  enum class MenuMode { None, SeedShop, UpgradeShop };
  enum class Tool { Shovel, Hoe, WateringCan, SeedPacket, None };
  enum class Outcome { Playing, Won, Lost };

  struct Seed {
    PlantDef def;
    int count = 0;
  };

  // events
  std::vector<std::unique_ptr<Event>> m_events;

  // Sprites
  SpriteInstance m_seed_maker;
  SpriteInstance m_house;
  SpriteInstance m_chest;

  SpriteInstance m_apple;
  TriggerVolume m_apple_trigger;
  bool m_apple_collected = false;
  float m_apple_timer = 0.0f;
  size_t m_apple_trigger_index = 0; // where the apple's trigger lives in m_triggers

  // menu
  float m_font_size = 2.0f;
  
  MenuMode m_menu_mode = MenuMode::None;

  // field
  PointLight m_highlight;
  bool m_show_highlight = false;
  Field m_field;

  // progression
  int m_tier = 0;
  int m_biomass = 0;
  double m_elapsed = 0.0;
  Outcome m_outcome = Outcome::Playing;

  const float SAVE_INTERVAL = 30.0f;
  float m_save_timer = 0.0f;

  // player
  Tool m_tool = Tool::None;
  int m_selected_seed = -1;
  std::vector<Seed> m_seeds;

  const glm::vec2 TOOL_SIZE = glm::vec2(0.35f);
  const float TOOL_FWD = 0.6f;   
  const float TOOL_SIDE = 0.30f; 
  const float TOOL_DROP = 0.30f; 


  // animation

  // const
  const std::vector<long long> TIER_COST = {100, 1000, 10000};//, 1000000, 100000000};

  const float TIME_LIMIT = 1000000000.0f; 

  const float FLOOR_TILE_SIZE = 100.0f;

  const float HOUSE_SIZE = 4.0f;
  const glm::vec3 HOUSE_POS = glm::vec3(-5.0f, HOUSE_SIZE / 2.0, -10.0f);

  const float CHEST_SIZE = 1.0f;
  const glm::vec3 CHEST_POS = glm::vec3(5.0f, CHEST_SIZE / 2.0, -10.0f);

  const float SUSHI_SIZE = 2.0f;
  const float HALF_SUSHI_SIZE = SUSHI_SIZE / 2;

  const float EMBED_SUSHI_DROP = 0.35f;
  const float EMBED_SUSHI_FORWARD = -0.25f;
  const float EMBED_SUSHI_SIDE = 0.15f;
  const glm::vec2 EMBED_SUSHI_SIZE = glm::vec2(0.35f, 0.35f);

  const float PLAYER_HEIGHT = 1.6f;
  const float BODY_DROP = PLAYER_HEIGHT / 2;
  const float BODY_FORWARD = -0.25f;
  const glm::vec2 BODY_SIZE = glm::vec2(BODY_DROP, PLAYER_HEIGHT);

  const float FADE_SPEED = 2.0f;
  const float APPLE_RESPAWN_TIME = 120.0f;

public:
  GardenScene();
  void onEnter(GLFWwindow &window) override;
  void onExit(GLFWwindow &window) override;
  Scene *update(GLFWwindow &window, float delta) override;
  void handleInput(GLFWwindow &window, float delta) override;
  void render(GLFWwindow &window, Renderer &renderer) override;
  void AdvanceDay(); 

  void PushNotification(const std::string &msg) { m_notification_manager.Push(msg); }

  // Query verbs — read-only conditions that reactive events poll each frame.
  bool HasPlantedTile() const {
    for (const auto &t : m_field.Tiles())
      if (t.HasPlant())
        return true;
    return false;
  }
  bool IsLookingDown() const { return m_camera.GetForward().y < -0.9f; }

  // Fire an event's start hook now, then keep it in the active list until it completes.
  void StartEvent(std::unique_ptr<Event> e) {
    e->OnStart();
    m_events.push_back(std::move(e));
  }

  bool SpendBiomass(int cost) {
    if (m_biomass < cost) {
      m_notification_manager.Push("Not enough biomass", 1.5f);
      return false;
    }
    m_biomass -= cost;
    return true;
  }

  int SeedCost(PlantDef def) { return (def.biomass_yield / 2) + 1; }

  void CycleSeed(int dir) {
    int n = (int)m_seeds.size();
    int start = (m_selected_seed < 0) ? 0 : m_selected_seed;
    for (int i = 1; i <= n; i++) {
      int idx = ((start + dir * i) % n + n) % n; // wrap, handles negatives
      if (m_seeds[idx].count > 0) {
        m_selected_seed = idx;
        return;
      }
    }
    m_selected_seed = -1; // nothing in stock
  }

  void UseToolOn(Tile &t) {
    switch (m_tool) {
    case Tool::Shovel:
      if (t.HasPlant())
        t.PullUp();        // real-time: no confirm — careless shovelling costs you the crop
      else if (t.IsRefuse())
        t.Clear();
      break;

    case Tool::Hoe:
      if (t.IsEmpty())
        t.Till();
      break;
    case Tool::WateringCan:
      if (!t.IsWatered())
        t.Water();
      break;
    case Tool::None:
      if (t.IsHarvestable()) {
        m_biomass += t.Harvest();
      }
      break;
    case Tool::SeedPacket:
      if (t.IsTilled() && m_selected_seed >= 0 &&
          m_seeds[m_selected_seed].count > 0) {
        t.Plant(&m_seeds[m_selected_seed].def);
        if (--m_seeds[m_selected_seed].count == 0)
          m_selected_seed = -1;
      }
      break;
    }
  }

  const char *GetToolName(Tool t) {
    switch (t) {
    case Tool::Shovel:
      return "Shovel";
    case Tool::Hoe:
      return "Hoe";
    case Tool::WateringCan:
      return "Watering Can";
    case Tool::SeedPacket:
      return "Seed Packet";
    case Tool::None:
      return "Hands";
    }
    return "?";
  }

  Texture* ToolTexture(Tool t) {
    switch (t) {
      case Tool::Shovel:      return &m_shovel_texture;
      case Tool::Hoe:         return &m_hoe_texture;
      case Tool::WateringCan: return &m_can_texture;
      case Tool::SeedPacket:  return &m_packet_texture;
      case Tool::None:        return nullptr;   // hands = nothing
    }
    return nullptr;
  }


  const PlantDef *FindDef(const std::string &name) {
    for (auto &s : m_seeds)
      if (s.def.name == name)
        return &s.def;
    return nullptr;
  }

  int SeedIndex(const std::string &name) {
    for (int i = 0; i < (int)m_seeds.size(); i++)
      if (m_seeds[i].def.name == name)
        return i;
    return -1; // not found / "" → none
  }
  
  void PlaceApple() {
    // Floor spans ~[-FLOOR_TILE_SIZE/2, +FLOOR_TILE_SIZE/2] in x/z; the farm field
    // sits in x,z ∈ [-5, 5]. Roll a spot, reject any that land on the farm.
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> coord(-FLOOR_TILE_SIZE / 2 + 2.0f,
                                                 FLOOR_TILE_SIZE / 2 - 2.0f);
    glm::vec3 pos;
    do {
      pos = glm::vec3(coord(rng), m_apple.size.y / 2.0f, coord(rng));
    } while (pos.x > -5.0f && pos.x < 5.0f && pos.z > -5.0f && pos.z < 5.0f);

    m_apple.position = pos;
    m_triggers[m_apple_trigger_index].position = pos; // move the *registered* trigger
    m_apple_collected = false;
  }

  const int SAVE_VERSION = 2;
  const std::string SAVE_PATH = "./save.json";

  void Save() {
    GameState s;
    s.version = SAVE_VERSION;
    s.elapsed = m_elapsed;
    s.biomass = m_biomass;
    s.tier = m_tier;
    s.selected_seed =
        (m_selected_seed >= 0) ? m_seeds[m_selected_seed].def.name : "";
    for (auto &sd : m_seeds)
      s.seeds[sd.def.name] = sd.count;
    for (auto &t : m_field.Tiles())
      s.tiles.push_back({(int)t.GetState(), t.IsWatered(), t.SecondsGrowing(),
                         t.GetPlantName()});
    SaveSystem::Save(SAVE_PATH, s);
  }

  bool Load() {
    GameState s;
    if (!SaveSystem::Load(SAVE_PATH, SAVE_VERSION, s))
      return false;
    m_elapsed = s.elapsed;
    m_biomass = s.biomass;
    m_tier = s.tier;
    for (auto &sd : m_seeds)
      sd.count = s.seeds.count(sd.def.name) ? s.seeds[sd.def.name] : 0;
    m_selected_seed = SeedIndex(s.selected_seed);
    auto &tiles = m_field.Tiles();
    if (s.tiles.size() == tiles.size())
      for (size_t i = 0; i < tiles.size(); i++)
        tiles[i].Set((Tile::TileState)s.tiles[i].state, s.tiles[i].watered,
                     FindDef(s.tiles[i].plant), s.tiles[i].seconds_growing);
    return true;
  }
};
