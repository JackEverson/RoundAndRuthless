#pragma once

#include "Event.hpp"
#include "Field.hpp"
#include "GardenRoom.hpp"
#include "NotificationManager.hpp"
#include "Plants.hpp"
#include "PointLight.hpp"
#include "Renderer.hpp"
#include "SaveSystem.hpp"
#include "Structures.hpp"
#include "Texture.hpp"
#include "Tile.hpp"

#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

#include <cstddef>
#include <cstdio>
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
  Texture m_fence_texture;
  Texture m_floor_texture;

  Texture m_sushi_texture;
  Texture m_sushi_eat_texture;
  Texture m_structure_maker_texture;
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
  Texture m_wrench_texture;
  Texture m_hand_texture;

  Texture m_sprinkler_texture;
  Texture m_hoer_texture;
  Texture m_planter_unloaded_texture;
  Texture m_planter_loaded_texture;
  Texture m_harvester_texture;

  Texture m_veg_top_texture;
  Texture m_apple_texture;
  Texture m_radish_texture;
  Texture m_turnip_texture;
  Texture m_bush_texture;
  Texture m_tomato_texture;
  Texture m_blueberry_texture;
  Texture m_kidneybean_texture;
  Texture m_staring_melon_growing_texture;
  Texture m_staring_melon_ripe_texture;
  Texture m_liverbounty_grow;
  Texture m_liverbounty_ripe;

  // enums and struct
public:
  enum class Outcome { Playing, Won, Lost };

private:
  enum class Tool { None, Hoe, WateringCan, SeedPacket, Wrench, Shovel };
  enum class MenuMode {
    None,
    Settings,
    SeedShop,
    SeedSelection,
    StructureShop,
    StructureSelection,
    UpgradeShop
  };

  struct Seed {
    PlantDef def;
    int count = 0;
  };

  struct StructureInv {
    StructureDef def;
    int count = 0;
  };

  // set dressing beyond the fence — never interacted with, never simulated
  struct AmbientSprite {
    SpriteInstance sprite;
    float phase = 0.0f;   // desyncs the sway so it reads as wind
    float sway = 1.0f;    // 0 = rigid (machines don't lean)
  };

  struct Wanderer {
    SpriteInstance sprite;
    glm::vec3 target{0.0f};
    glm::vec2 pen_min{0.0f}, pen_max{0.0f};  // stays inside its own pen
    float pause = 0.0f;                       // standing still (doing chores)
  };

  // events
  std::vector<std::unique_ptr<Event>> m_events;

  // Sprites
  SpriteInstance m_seed_maker;
  SpriteInstance m_structure_maker;
  SpriteInstance m_chest;

  SpriteInstance m_apple;
  TriggerVolume m_apple_trigger;
  bool m_apple_collected = false;
  float m_apple_timer = 0.0f;
  size_t m_apple_trigger_index =
      0; // where the apple's trigger lives in m_triggers

  // menu
  float m_font_size = 2.0f;

  MenuMode m_menu_mode = MenuMode::None;
  bool m_esc_held = false;

  // settings (persisted to settings.json; fullscreen default)
  float m_master_volume = 1.0f;
  float m_brightness = 1.0f; // scales the ambient light
  bool m_borderless = true;
  bool m_show_crosshair = true;

  const glm::vec4 FLOOR_COLOR = glm::vec4(0.45f, 0.44f, 0.42f, 1.0f);
  const glm::vec4 SOIL_COLOR = glm::vec4(0.58f, 0.28f, 0.20f, 1.0f);

  PointLight m_highlight;
  bool m_show_highlight = false;
  Field m_field;

  // progression
  std::string m_task_text = "";
  int m_tier = 0;
  long long m_biomass = 0;
  double m_elapsed = 0.0;
  float m_random_event_timer = 0.0f;
  float m_next_random_event = 45.0f; 
  const float RANDOM_EVENT_MIN = 60.0f; 
  const int RANDOM_EVENT_SPREAD = 90;
  bool m_quit_game = false;
  Outcome m_outcome = Outcome::Playing;
  double m_final_time = 0.0;    // m_elapsed frozen at the winning purchase
  int m_final_harvests = 0;     // m_harvest_count frozen at the same moment

  const float SAVE_INTERVAL = 30.0f;
  float m_save_timer = 0.0f;
  bool m_new_game = false;
  bool m_confirm_new = false;

  bool m_was_looking_down = false;
  float m_sushi_quip_cooldown = 0.0f;

  // player
  Tool m_tool = Tool::None;
  int m_selected_seed = -1;
  int m_selected_structure = -1;
  int m_harvest_count = 0;

  std::vector<Seed> m_seeds;
  std::vector<StructureInv> m_structure_inv;

  // surroundings
  std::vector<AmbientSprite> m_ambient_sprites;
  std::vector<Wanderer> m_wanderers;

  const glm::vec2 TOOL_SIZE = glm::vec2(0.35f);
  const float TOOL_FWD = 0.6f;
  const float TOOL_SIDE = 0.30f;
  const float TOOL_DROP = 0.30f;

  // animation

  // const
  const std::vector<long long> TIER_COST = {100, 1000,10000, 1000000, 1000000000};

  const float FLOOR_TILE_SIZE = 100.0f;

  // the pen: low fence the camera (1.6) sees over but the player can't cross
  const float FENCE_HEIGHT = 1.0f;    // full visible height — rails sit ON the ground
  const float FENCE_TILE_WIDTH = 2.0f; // world units per repeat of fence.png
  const glm::vec2 PEN_MIN = glm::vec2(-14.0f, -14.0f);   // x, z
  const glm::vec2 PEN_MAX = glm::vec2(14.0f, 30.0f);
  const float WANDERER_SPEED = 0.8f;

  const float HOUSE_SIZE = 2.0f;
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
  const float APPLE_RESPAWN_TIME = 60.0f;

  // const float DAY_LENGTH = 10.0f; 
  const float DAY_LENGTH = 300.0f; 
  const glm::vec3 DAY_SKY = glm::vec3(0.10f, 0.09f, 0.07f);
  const glm::vec3 NIGHT_SKY = glm::vec3(0.04f, 0.05f, 0.09f);
  const float DAY_AMBIENT = 0.25f;
  const float NIGHT_AMBIENT = 0.16f;
  const float DAY_FOG = 0.05f;
  const float NIGHT_FOG = 0.12f;

  // torch that follows the player; scaled by nightfall in render()
  // const glm::vec3 PLAYER_LIGHT_COLOR = glm::vec3(0.9f, 0.85f, 0.7f) * 0.7f;
  // const float PLAYER_LIGHT_RADIUS = 7.0f;

public:
  GardenScene();
  void onEnter(GLFWwindow &window) override;
  void onExit(GLFWwindow &window) override;
  Scene *update(GLFWwindow &window, float delta) override;
  void handleInput(GLFWwindow &window, float delta) override;
  void render(GLFWwindow &window, Renderer &renderer) override;
  void AdvanceDay();

  void PushNotification(const std::string &msg, float duration = 4.5f) {
    m_notification_manager.Push(msg, duration);
  }

  // Query verbs — read-only conditions that reactive events poll each frame.
  bool HasPlantedTile() const {
    for (const auto &t : m_field.Tiles())
      if (t.HasPlant())
        return true;
    return false;
  }
  bool HasNoRocks() const {
    for (const auto &t : m_field.Tiles())
      if (t.IsRefuse())
        return false;
    return true;
  }
  bool HasHoedTile() const {
    for (const auto &t : m_field.Tiles())
      if (t.IsTilled())
        return true;
    return false;
  }
  bool HasWateredTile() const {
    for (const auto &t : m_field.Tiles())
      if (t.IsWatered())
        return true;
    return false;
  }
  bool HasRipeTile() const {
    for (const auto &t : m_field.Tiles())
      if (t.IsHarvestable())
        return true;
    return false;
  }
  bool PlayerHasBiomass() const { return m_biomass > 0; }
  bool PlayerHasSeed() const {
    for (const auto &s : m_seeds)
      if (s.count > 0)
        return true;
    return false;
  }

  // 8421 → "8421", 43718254 → "43.7M", 1000000000 → "1.0B"
  static std::string FormatBiomass(long long g) { 
    if (g < 10000) return std::to_string(g);   // small numbers read fine raw
    const char *suffix[] = {"k", "M", "B", "T"};
    double v = (double)g;
    int i = -1;
    while (v >= 1000.0 && i < 3) { v /= 1000.0; i++; }
    char buf[32];
    if (v >= 100.0) snprintf(buf, sizeof(buf), "%.0f%s", v, suffix[i]);
    else            snprintf(buf, sizeof(buf), "%.1f%s", v, suffix[i]);
    return buf;
  }

  bool IsLookingDown() const { return m_camera.GetForward().y < -0.98f; }
  int HarvestCount() const { return m_harvest_count; }
  void SetTaskText(const std::string &text) { m_task_text = text; }
  void ClearTaskText() { m_task_text = ""; }
  void SetOutcome(const Outcome &outcome) { m_outcome = outcome; }
  void QUIT() { m_quit_game = true; };

  void PlaySound(const std::string &name, float volume = 1.0f) {
    sound_manager.PlaySound(name, volume);   
  }
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

  void CycleTool(int dir) {
    m_tool = static_cast<Tool>((static_cast<int>(m_tool) + dir + 6) % 6);
  }

  void UseToolOn(Tile &t) {
    switch (m_tool) {

    case Tool::Shovel:
    if (t.HasStructure()) {
      if (Structure *s = m_field.StructureAtTile(t)) {
        std::string name = s->def->name;      // ⚠ capture BEFORE removal —
        m_field.RemoveStructureAt(t);         // erase invalidates s (dangling!)
        for (auto &si : m_structure_inv)      // refund the machine
          if (si.def.name == name) { ++si.count; break; }
        sound_manager.PlaySound("dig");
      }
    }
    else if (t.HasPlant()) {
        t.PullUp();
        sound_manager.PlaySound("dig");
      } else if (t.IsRefuse() || t.IsTilled()) {
        t.Clear();
        sound_manager.PlaySound("dig");
      }
    break;

    case Tool::Hoe:
      if (t.IsEmpty() && !t.HasStructure()) {
        t.Till();
        sound_manager.PlaySound("dig");
      }
      break;

    case Tool::WateringCan:
      t.Water();
      break;

    case Tool::None:
      if (t.IsHarvestable()) {
        m_biomass += t.Harvest();
        m_harvest_count++;
        sound_manager.PlaySound("pop");
      }
      break;

    case Tool::SeedPacket:
      if (t.HasStructure()) {
        Structure *s = m_field.StructureAtTile(t);
        if (s && s->def->kind == StructureKind::Planter &&
            m_selected_seed >= 0 &&
            s->crop != &m_seeds[m_selected_seed].def) {   // same seed = no-op
          if (m_seeds[m_selected_seed].count >= s->def->seed_load) {
            s->crop = &m_seeds[m_selected_seed].def;      // swap: old seeds are gone
            m_seeds[m_selected_seed].count -= s->def->seed_load;
            PushNotification("Planter loaded: " + s->crop->name);
            PlaySound("pop");
          } else {
            PushNotification("Planter needs " +
                             std::to_string(s->def->seed_load) +
                             " seeds to load", 3.0f);
          }
        }
      } else if (t.IsTilled() && m_selected_seed >= 0 &&
                 m_seeds[m_selected_seed].count > 0) {
        t.Plant(&m_seeds[m_selected_seed].def);
        --m_seeds[m_selected_seed].count;
      }
      break;

    case Tool::Wrench:
      if (m_selected_structure >= 0 &&
          m_structure_inv[m_selected_structure].count > 0) {
        if (m_field.PlaceStructureAt(
                t, &m_structure_inv[m_selected_structure].def)) {
          --m_structure_inv[m_selected_structure].count;
        }
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
    case Tool::Wrench:
      return "Wrench";
    }
    return "?";
  }

  Texture *ToolTexture(Tool t) {
    switch (t) {
    case Tool::Shovel:
      return &m_shovel_texture;
    case Tool::Hoe:
      return &m_hoe_texture;
    case Tool::WateringCan:
      return &m_can_texture;
    case Tool::SeedPacket:
      return &m_packet_texture;
    case Tool::Wrench:
      return &m_wrench_texture;
    case Tool::None:
      return &m_hand_texture; // hands = nothing
    }
    return nullptr;
  }

  const PlantDef *FindPlantDef(const std::string &name) {
    for (auto &s : m_seeds)
      if (s.def.name == name)
        return &s.def;
    return nullptr;
  }

  const StructureDef *FindStructureDef(const std::string &name){
    for (auto&s : m_structure_inv){
      if (s.def.name == name){
        return &s.def;
      }
    }
    return nullptr;
  }

  int SeedIndex(const std::string &name) {
    for (int i = 0; i < (int)m_seeds.size(); i++)
      if (m_seeds[i].def.name == name)
        return i;
    return -1; // not found / "" → none
  }

  void PlaceApple() {
    // Somewhere on the field itself (origin (-5,-5), 13x30) — the pen fence
    // made the old wasteland band unreachable.
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> fx(-4.5f, 7.5f);
    std::uniform_real_distribution<float> fz(-4.5f, 24.5f);
    glm::vec3 pos(fx(rng), m_apple.size.y / 2.0f, fz(rng));

    m_apple.position = pos;
    m_triggers[m_apple_trigger_index].position = pos; // move the *registered* trigger
    m_apple_collected = false;
  }

  void EaseLookDown(float dt, float target_pitch_deg = -89.0f,
                    float speed = 1.0f) {
    glm::vec3 f = m_camera.GetForward();
    float pitch = glm::degrees(asinf(glm::clamp(f.y, -1.0f, 1.0f)));
    float yaw =
        glm::degrees(atan2f(f.z, f.x)); // matches CalcLookAt's convention

    float t = 1.0f - expf(-speed * dt); // framerate-independent ease
    m_camera.SetRotation(yaw, pitch + (target_pitch_deg - pitch) * t);
  }

  const float GetUiScale(int h) {
    return m_font_size * (float)h / 1440.0f; // 1440p = your tuning reference
  };


  void Save() {
    GameState s;
    s.elapsed = m_elapsed;
    s.biomass = m_biomass;
    s.tier = m_tier;
    s.harvest_count = m_harvest_count;
    s.selected_seed =
        (m_selected_seed >= 0) ? m_seeds[m_selected_seed].def.name : "";
    for (auto &sd : m_seeds)
      s.seeds[sd.def.name] = sd.count;
    for (auto &t : m_field.Tiles())
      s.tiles.push_back({(int)t.GetState(), t.IsWatered(), t.SecondsGrowing(),
                         t.GetPlantName()});
    for (auto &si : m_structure_inv) {
      s.structure_inv[si.def.name] = si.count;
    }
    for (auto &sf : m_field.Structures()) {
      s.structures_field.push_back(
          {sf.def->name, sf.row, sf.col, sf.crop ? sf.crop->name : ""});
    }
    SaveSystem::Save(SAVE_PATH, s);
  }

  bool Load() {
    GameState s;
    if (!SaveSystem::Load(SAVE_PATH, s.version, s))
      return false;
    m_elapsed = s.elapsed;
    m_biomass = s.biomass;
    m_tier = s.tier;
    m_harvest_count = s.harvest_count;
    
    for (auto &sd : m_seeds) {
      sd.count = s.seeds.count(sd.def.name) ? s.seeds[sd.def.name] : 0;
      m_selected_seed = SeedIndex(s.selected_seed);
    }
    auto &tiles = m_field.Tiles();
    if (s.tiles.size() == tiles.size()) {
      for (size_t i = 0; i < tiles.size(); i++)
        tiles[i].Set((Tile::TileState)s.tiles[i].state,
                     FindPlantDef(s.tiles[i].plant), s.tiles[i].seconds_growing);
    }

    for (auto &si : m_structure_inv){
      si.count = s.structure_inv.count(si.def.name) ? s.structure_inv[si.def.name] : 0;
    }
    
    auto &structs = m_field.Structures();
    for (auto &st : s.structures_field){
      if (const StructureDef* def = FindStructureDef(st.name)) 
      { m_field.PlaceStructureAt(st.row, st.col, def, FindPlantDef(st.crop)); }
    }

    return true;
  }
};
