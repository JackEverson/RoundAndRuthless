#pragma once
#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture.hpp"

struct SpriteInstance
{
  glm::vec3 position;
  glm::vec2 size;
  glm::vec4 color;
  glm::mat4 model_mat;
  Texture *texture;

  SpriteInstance()
      : position(glm::vec3(0.0f)), size(glm::vec2(1.0f)),
        color(glm::vec4(1.0f)), model_mat(glm::mat4(1)), texture(nullptr) {}
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

  int m_vertexSize = 25;

public:
  Renderer();
  ~Renderer();

  void Clear(float r, float g, float b, float a) const;
  void DrawBackground(const Texture &texture);

  // void DrawSprite(Texture &texture, glm::vec2 position, glm::vec2 size,
  // float rotate, glm::vec4 color, glm::mat4 projection);
  void BeginBatchDraw(int countEstimate);
  void SubmitSprite(const SpriteInstance &sprite);
  void RendBatch(glm::mat4 view, glm::mat4 projection, glm::vec3 campos, float fogfactor = 0.15f);

private:
  void initRenderData();
};
