#pragma once

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
  void RunSprinklers();

  std::vector<Tile>& Tiles() { return m_tiles; } // for load
  const std::vector<Tile>& Tiles() const { return m_tiles; } // for save

private:
  std::vector<Tile> m_tiles;
  int m_w = 0, m_h = 0;
  float m_tile_size = 1.0f;
  glm::vec3 m_origin{0.0f};
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

inline void Field::RunSprinklers() {
  for (int r = 0; r < m_h; r++)
    for (int c = 0; c < m_w; c++) {
      if (!m_tiles[r * m_w + c].HasSprinkler()) continue;
      for (int dr : {-1, 1})
        for (int dc : {-1, 1}) {
          int nr = r + dr, nc = c + dc;
          if (nr >= 0 && nr < m_h && nc >= 0 && nc < m_w)
            m_tiles[nr * m_w + nc].Water();   // every frame = permanently topped up
        }
    }
}