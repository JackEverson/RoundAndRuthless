#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include "PointLight.hpp"
#include "glm/ext/vector_float4.hpp"

#include <glm/glm.hpp>

struct SpriteInstance
{
  glm::vec3 position;
  glm::vec2 size;
  glm::vec4 color;
  glm::mat4 model_mat;
  glm::vec2 uv_scale;   // >1 tiles the texture (needs a repeat-wrapped Texture)
  Texture *texture;

  SpriteInstance()
      : position(glm::vec3(0.0f)), size(glm::vec2(1.0f)),
        color(glm::vec4(1.0f)), model_mat(glm::mat4(1)),
        uv_scale(glm::vec2(1.0f)), texture(nullptr) {}
};

class Renderer
{
private:
  Shader shader;
  Shader backgroundShader;
  unsigned int quadVAO = 0;
  unsigned int quadVBO = 0;
  unsigned int quadEBO = 0;
  unsigned int instanceVBO = 0;

  unsigned int backgroundVAO = 0;
  unsigned int backgroundVBO = 0;

  std::vector<SpriteInstance> batch;
  std::vector<SpriteInstance> transparent_batch;

  int m_vertexSize = 27;   // 3 pos + 2 size + 4 color + 16 model + 2 uv_scale

  float m_ambient_light = 0.25f;
  std::vector<PointLight> m_point_lights;


public:
  Renderer();
  ~Renderer();

  void Clear(float r, float g, float b, float a);
  void DrawBackground(const Texture &texture);

  // void DrawSprite(Texture &texture, glm::vec2 position, glm::vec2 size,
  // float rotate, glm::vec4 color, glm::mat4 projection);
  void SetLights(const std::vector<PointLight>& lights, float ambient);

  void BeginBatchDraw(int opaqueEstimate = 10, int transparentEstimate = 10);
  void SubmitSprite(const SpriteInstance &sprite);
  void SubmitTransparentSprite(const SpriteInstance &sprite);
  void RendBatch(glm::mat4 view, glm::mat4 projection, glm::vec3 campos, float fogfactor = 0.15f);

private:
  void initRenderData();
  glm::vec4 m_clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};
