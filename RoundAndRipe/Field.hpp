#pragma once

#include "Renderer.hpp"
#include "Texture.hpp"
#include "Tile.hpp"

#include "glm/ext/vector_float3.hpp"
#include <cstddef>


class Field {
public:
  Field(glm::vec3 origin, int w, int h, float tileSize, Texture *soil,
        Texture *rock, Texture *till);
  void Advance();
  void Render(Renderer &renderer, const glm::vec3 &campos);
  void CollectLights(std::vector<PointLight> &out) const;
  Tile *TileAtRay(glm::vec3 origin, glm::vec3 dir);

private:
  std::vector<Tile> m_tiles;
  int m_w = 0, m_h = 0;
  float m_tile_size = 1.0f;
  glm::vec3 m_origin{0.0f};
};

inline Field::Field(glm::vec3 origin, int w, int h, float tileSize,
                    Texture *soil, Texture *refuse, Texture *till)
    : m_origin(origin), m_w(w), m_h(h), m_tile_size(tileSize) {
  m_tiles.reserve(m_w * m_h);

  for (int col = 0; col < m_h; col++) {
    for (int row = 0; row < m_w; row++) {
      m_tiles.emplace_back(
          origin + glm::vec3(row * m_tile_size, 0, col * m_tile_size), soil,
          refuse, till);
    }
  }
}

inline void Field::Advance() {
  for (auto &tile : m_tiles) {
    tile.Advance();
  }
}

inline void Field::Render(Renderer &renderer, const glm::vec3 &campos) {
  for (auto &tile : m_tiles) {
    tile.Render(renderer, campos);
  }
}

inline void Field::CollectLights(std::vector<PointLight> &out) const {
  for (auto &tile : m_tiles) {
    if (auto light = tile.RipeLight()) {
      out.push_back(*light);
    }
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
