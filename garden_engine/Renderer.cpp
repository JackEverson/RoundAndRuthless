#include "Renderer.hpp"
#include "ShaderSource.hpp"
#include "gl_debug.hpp"

#include <algorithm>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer()
    : shader(TextureVertexShader, TextureFragmentShader),
      backgroundShader(BackgroundVertexShader, BackgroundFragmentShader) {
  initRenderData();
}

Renderer::~Renderer() {}

void Renderer::Clear(float r, float g, float b, float a) const {
  glClearColor(r, g, b, a);
  GLCall(glClear(GL_COLOR_BUFFER_BIT));
  GLCall(glClear(GL_DEPTH_BUFFER_BIT));
}

void Renderer::DrawBackground(const Texture &texture) {
  // Clear(sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);

  backgroundShader.Bind();
  texture.Bind(0);

  GLCall(glBindVertexArray(this->quadVAO));

  GLCall(glDisable(GL_BLEND));
  GLCall(glDisable(GL_DEPTH_TEST));

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  backgroundShader.Unbind();
  glBindVertexArray(0);
}

void Renderer::SetLights(const std::vector<PointLight> &lights, float ambient) {
  m_ambient_light = ambient;
  m_point_lights = lights;
}

void Renderer::BeginBatchDraw(int opaqueEstimate, int transparentEstimate) {
  batch.clear();
  batch.reserve(opaqueEstimate);

  transparent_batch.clear();
  transparent_batch.reserve(transparentEstimate);
}

void Renderer::SubmitSprite(const SpriteInstance &sprite) {
  batch.push_back(sprite);
}

void Renderer::SubmitTransparentSprite(const SpriteInstance &sprite) {
  transparent_batch.push_back(sprite);
}

void Renderer::RendBatch(glm::mat4 view, glm::mat4 projection, glm::vec3 campos,
                         float fogfactor) {
  if (batch.empty() && transparent_batch.empty())
    return;

  shader.Bind();
  shader.SetUniformMat4f("u_view", view);
  shader.SetUniformMat4f("u_projection", projection);
  shader.SetUniform3f("u_cam_pos", campos);
  shader.SetUniform1f("u_fog_factor", fogfactor);
  shader.SetUniform1f("u_ambient", m_ambient_light);

  int count = std::min((int)m_point_lights.size(),
                       8); // hardcoded 8 lights max in shader
  for (int i = 0; i < count; i++) {
    std::string base = "u_lights[" + std::to_string(i) + "].";
    shader.SetUniform3f(base + "position", m_point_lights[i].position);
    shader.SetUniform3f(base + "color", m_point_lights[i].color);
    shader.SetUniform1f(base + "radius", m_point_lights[i].radius);
  }
  shader.SetUniform1i("u_light_count", count);

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  GLCall(glEnable(GL_DEPTH_TEST));

  GLCall(glBindBuffer(GL_ARRAY_BUFFER, instanceVBO));
  GLCall(glBindVertexArray(quadVAO));

  std::vector<float> instances;
  instances.reserve(batch.size() * m_vertexSize);

  for (auto &s : batch) {
    instances.push_back(s.position.x);
    instances.push_back(s.position.y);
    instances.push_back(s.position.z);
    instances.push_back(s.size.x);
    instances.push_back(s.size.y);
    instances.push_back(s.color.r);
    instances.push_back(s.color.g);
    instances.push_back(s.color.b);
    instances.push_back(s.color.a);

    for (int i = 0; i < 4; i++) {
      instances.push_back(s.model_mat[i][0]);
      instances.push_back(s.model_mat[i][1]);
      instances.push_back(s.model_mat[i][2]);
      instances.push_back(s.model_mat[i][3]);
    }
  }
  GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(float),
                         instances.data()));

  size_t groupStart = 0;
  while (groupStart < batch.size()) {
    Texture *groupTex = batch[groupStart].texture;
    size_t groupEnd = groupStart;

    // find where this texture group ends
    while (groupEnd < batch.size() && batch[groupEnd].texture == groupTex)
      groupEnd++;

    groupTex->Bind();
    GLCall(glDrawElementsInstancedBaseInstance(
        GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
        (GLsizei)(groupEnd - groupStart), // how many instances
        (GLuint)groupStart));             // where in the buffer to start

    groupStart = groupEnd;
  }

  // Transparent objects being sorted for painters algo. 
  // Probably a better way to do this but fine until order independant transparency implemented
  std::sort(transparent_batch.begin(), transparent_batch.end(),
            [&campos](const SpriteInstance &a, const SpriteInstance &b) {
              return glm::distance(campos, a.position) >
                     glm::distance(campos, b.position);
            });

  std::vector<float> tinstances;
  tinstances.reserve(batch.size() * m_vertexSize);

  for (auto &s : transparent_batch) {
    tinstances.push_back(s.position.x);
    tinstances.push_back(s.position.y);
    tinstances.push_back(s.position.z);
    tinstances.push_back(s.size.x);
    tinstances.push_back(s.size.y);
    tinstances.push_back(s.color.r);
    tinstances.push_back(s.color.g);
    tinstances.push_back(s.color.b);
    tinstances.push_back(s.color.a);

    for (int i = 0; i < 4; i++) {
      tinstances.push_back(s.model_mat[i][0]);
      tinstances.push_back(s.model_mat[i][1]);
      tinstances.push_back(s.model_mat[i][2]);
      tinstances.push_back(s.model_mat[i][3]);
    }
  }
  GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, tinstances.size() * sizeof(float),
                         tinstances.data()));

  groupStart = 0;
  while (groupStart < transparent_batch.size()) {
    Texture *groupTex = transparent_batch[groupStart].texture;
    size_t groupEnd = groupStart;

    // find where this texture group ends
    while (groupEnd < transparent_batch.size() && transparent_batch[groupEnd].texture == groupTex)
      groupEnd++;

    groupTex->Bind();
    GLCall(glDrawElementsInstancedBaseInstance(
        GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
        (GLsizei)(groupEnd - groupStart), // how many instances
        (GLuint)groupStart));             // where in the buffer to start

    groupStart = groupEnd;
  }

  GLCall(glBindVertexArray(0));
}

void Renderer::initRenderData() {
  float vertices[] = {
      // pos        // tex
      -0.5f, -0.5f, 0.0f,
      0.0f, // Bottom-left
      0.5f,  -0.5f, 1.0f,
      0.0f, // Bottom-right
      -0.5f, 0.5f,  0.0f,
      1.0f, // Top-left
      0.5f,  0.5f,  1.0f,
      1.0f, // Top-right
  };

  unsigned int indices[] = {0, 1, 2, 1, 2, 3};

  GLCall(glGenVertexArrays(1, &this->quadVAO));
  GLCall(glBindVertexArray(this->quadVAO));

  GLCall(glGenBuffers(1, &quadVBO));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, quadVBO));
  GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                      GL_STATIC_DRAW));
  // aPos
  GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                               (void *)0));
  GLCall(glEnableVertexAttribArray(0));
  // aTexCoord
  GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                               (void *)(2 * sizeof(float))));
  GLCall(glEnableVertexAttribArray(1));

  GLCall(glGenBuffers(1, &quadEBO));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                      GL_STATIC_DRAW));

  GLCall(glGenBuffers(1, &instanceVBO));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, instanceVBO));
  GLCall(glBufferData(GL_ARRAY_BUFFER, 10000 * m_vertexSize * sizeof(float),
                      nullptr, GL_DYNAMIC_DRAW));

  int offset = 0;
  // instancePos
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * m_vertexSize,
                        (void *)0);
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);
  offset += 3;

  // instanceSize
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * m_vertexSize,
                        (void *)(sizeof(float) * offset));
  glVertexAttribDivisor(3, 1);
  offset += 2;

  // instanceColor
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * m_vertexSize,
                        (void *)(sizeof(float) * offset));
  glVertexAttribDivisor(4, 1);
  offset += 4;

  // instance model matrix
  for (int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(5 + i);
    glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE,
                          sizeof(float) * m_vertexSize,
                          (void *)(sizeof(float) * (9 + i * 4)));
    glVertexAttribDivisor(5 + i, 1);
  }
}
