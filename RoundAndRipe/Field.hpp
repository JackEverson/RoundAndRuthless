#pragma once

#include "Plants.hpp"
#include "Structures.hpp"
#include "Texture.hpp"
#include "Tile.hpp"
#include "Renderer.hpp"

#include "glm/ext/vector_float3.hpp"


class Field {
public:
  Field(glm::vec3 origin, int w, int h, float tileSize, Texture *soil,
        Texture *rock, Texture *till,Texture *seeded);
  void Render(Renderer &renderer, const glm::vec3 &campos);
  void CollectLights(std::vector<PointLight> &out) const;
  Tile *TileAtRay(glm::vec3 origin, glm::vec3 dir);
  
  bool RemoveStructureAt(Tile &t);
  Structure* StructureAt(int row, int col);
  Structure* StructureAtTile(Tile &t);   // for tools that only hold a Tile&
  bool PlaceStructureAt(int row, int col, const StructureDef *def, const PlantDef *pdef = nullptr);
  bool PlaceStructureAt(Tile &t, const StructureDef* structure);
  int RunStructures(float delta);

  // for load
  std::vector<Tile>& Tiles() { return m_tiles; } 
  std::vector<Structure>& Structures() { return m_structures; }

  // for save
  const std::vector<Tile>& Tiles() const { return m_tiles; } 
  const std::vector<Structure>& Structures() const { return m_structures; }


private:
  std::vector<Tile> m_tiles;
  std::vector<Structure> m_structures;
  void PlaceStructure(Tile &t, const StructureDef *def, int row, int col, const PlantDef *pdef = nullptr);
  int m_w = 0, m_h = 0;
  float m_tile_size = 1.0f;
  glm::vec3 m_origin{0.0f};

  // universal machine cycle: all structures act in synchronized waves.
  // second 0 → hoers, 1 → planters, 2 → harvesters (sprinklers continuous)
  static constexpr float CYCLE_PERIOD = 3.0f;
  float m_cycle = 0.0f;
  int m_last_slot = -1;
};

inline Field::Field(glm::vec3 origin, int w, int h, float tileSize,
                    Texture *soil, Texture *refuse, Texture *till, Texture *seeded)
    : m_origin(origin), m_w(w), m_h(h), m_tile_size(tileSize) {
  m_tiles.reserve(m_w * m_h);

  for (int col = 0; col < m_h; col++) {
    for (int row = 0; row < m_w; row++) {
      m_tiles.emplace_back(
          origin + glm::vec3((row + 0.5f) * m_tile_size, 0, (col + 0.5f) * m_tile_size), 
          soil, refuse, till, seeded);
    }
  }
}

inline void Field::Render(Renderer &renderer, const glm::vec3 &campos) {
  for (auto &tile : m_tiles) {
    tile.Render(renderer, campos);
  }
  for (auto &s : m_structures) {   // yaw-billboard, same treatment as crops
    glm::vec3 to_cam = campos - s.sprite.position;
    float yaw = std::atan2(to_cam.x, to_cam.z);
    s.sprite.model_mat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0, 1, 0));
    renderer.SubmitTransparentSprite(s.sprite);
  }
}

inline Tile *Field::TileAtRay(glm::vec3 origin, glm::vec3 dir) {
  if (std::abs(dir.y) < 1e-6f)
    return nullptr; // ray parallel to ground → never meets it
  float t =
      (m_origin.y - origin.y) / dir.y; // distance along ray to the tiles' plane
  if (t < 0.0f)
    return nullptr; // plane is behind you (looking away)
  glm::vec3 hit =
      origin + t * dir; // the point where you're aiming at the ground

  int row = (int)std::floor((hit.x - m_origin.x) /
                            m_tile_size); // x → 'row' (0 .. m_w-1)
  int col = (int)std::floor((hit.z - m_origin.z) /
                            m_tile_size); // z → 'col' (0 .. m_h-1)
  if (row < 0 || row >= m_w || col < 0 || col >= m_h)
    return nullptr; // off the grid

  return &m_tiles[col * m_w + row];
}

inline bool Field::RemoveStructureAt(Tile &t) {
  int idx = (int)(&t - m_tiles.data());
  if (idx < 0 || idx >= (int)m_tiles.size()) return false;
  int row = idx % m_w, col = idx / m_w;

  for (auto it = m_structures.begin(); it != m_structures.end(); ++it) {
    if (it->row == row && it->col == col) {
      m_structures.erase(it);
      t.SetStructure(false);        
      return true;
    }
  }
  return false;
}

inline bool Field::PlaceStructureAt(int row, int col, const StructureDef *def, const PlantDef *pdef) {
  if (row < 0 || row >= m_w || col < 0 || col >= m_h) return false;
  Tile& t = m_tiles[(col * m_w)+ row];

  PlaceStructure(t, def, row, col, pdef);
  return true;
}

inline bool Field::PlaceStructureAt(Tile &t, const StructureDef *def) {
  int idx = (int)(&t - m_tiles.data());          // pointer arithmetic: which element is this?
  if (idx < 0 || idx >= (int)m_tiles.size()) return false;
  int row = idx % m_w, col = idx / m_w;          // invert your col * m_w + row indexing

  if (t.IsTilled()) { t.Clear(); }
  if (!t.IsEmpty() || t.HasStructure()) return false;

  PlaceStructure(t, def, row, col);
  return true;
}

inline void Field::PlaceStructure(Tile &t, const StructureDef *def, int row, int col, const PlantDef* pdef){
  Structure s;
  s.def = def;
  s.row = row; s.col = col;

  s.sprite.texture = def->texture;
  s.sprite.size = def->full_size;
  s.sprite.color = glm::vec4(1.0f);
  s.sprite.position = t.Position() + glm::vec3(0.0f, def->full_size.y / 2.0f, 0.0f);
  s.crop = pdef;

  m_structures.push_back(s);
  t.SetStructure(true);                          // ← Field is the flag's writer
}

inline Structure* Field::StructureAtTile(Tile &t) {
  int idx = (int)(&t - m_tiles.data());
  if (idx < 0 || idx >= (int)m_tiles.size()) return nullptr;
  return StructureAt(idx % m_w, idx / m_w);
}

inline Structure* Field::StructureAt(int row, int col){
  for(auto& s : m_structures){
    if (row == s.row && col == s.col) return &s;
  }
  return nullptr;
}

inline int Field::RunStructures(float delta) {
  int collected = 0;

  // advance the shared cycle; a phase fires only on the frame its second begins
  m_cycle += delta;
  if (m_cycle >= CYCLE_PERIOD) m_cycle -= CYCLE_PERIOD;
  int slot = (int)m_cycle;                       // 0, 1 or 2
  bool phase_fired = (slot != m_last_slot);
  m_last_slot = slot;

  static constexpr StructureKind PHASE[3] = {
      StructureKind::Hoer, StructureKind::Planter, StructureKind::Harvester};

  for (auto &s : m_structures) {
    bool acts = (s.def->kind == StructureKind::Sprinkler)   // continuous
             || (phase_fired && s.def->kind == PHASE[slot]); // this wave's turn
    if (!acts) continue;

    // visit the 8 surrounding tiles
    for (int dr = -1; dr <= 1; dr++) {
      for (int dc = -1; dc <= 1; dc++) {
        if (dr == 0 && dc == 0) continue;               // skip own tile
        int nr = s.row + dr, nc = s.col + dc;
        if (nr < 0 || nr >= m_w || nc < 0 || nc >= m_h) continue;
        Tile &n = m_tiles[nc * m_w + nr];

        switch (s.def->kind) {
        case StructureKind::Sprinkler:
          n.Water();                                     // topped up every frame
          break;
        case StructureKind::Harvester:
          if (n.IsHarvestable())
            collected += n.Harvest();
          break;
        case StructureKind::Hoer:
          if (!n.HasStructure())
            n.Till();                                    // Till() itself requires Empty
          break;
        case StructureKind::Planter:
          if (s.crop && n.IsTilled() && !n.HasStructure())
            n.Plant(s.crop);
          break;
        }
      }
    }
  }
  return collected;   // harvester income — scene adds it to biomass
}