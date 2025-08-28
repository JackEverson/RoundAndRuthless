#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.hpp"
#include "gl_debug.hpp"
#include "glm/fwd.hpp"

Renderer::Renderer() : shader("./res/shaders/basictexture.shader") {
  initRenderData();
}

Renderer::~Renderer() {}

void Renderer::Clear(float r, float g, float b, float a) const {
  glClearColor(r, g, b, a);
  GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::DrawSprite(Texture &texture, glm::vec2 position, glm::vec2
size,
                          float rotate, glm::vec4 color, glm::mat4
                          projection) {
//
//   // Activate shader
//   this->shader.Bind();
//   glm::mat4 model = glm::mat4(1.0f);
//
//   // Translate
//   model = glm::translate(model, glm::vec3(position, 0.0f));
//
//   // Rotate around center
//   model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y,
//   0.0f)); model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f,
//   0.0f, 1.0f)); model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f
//   * size.y, 0.0f));
//
//   // Scale
//   model = glm::scale(model, glm::vec3(size, 1.0f));
//
//   this->shader.SetUniformMat4f("aModel", model);
//   this->shader.SetUniformMat4f("aProjection", projection);
//   this->shader.SetUniform4f("aColor", color);
//
//   texture.Bind(0);
//
//   GLCall(glEnable(GL_BLEND));
//   GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
//
//   GLCall(glBindVertexArray(this->quadVAO));
//   GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
//   GLCall(glBindVertexArray(0));
}

void Renderer::BeginBatchDraw(int countEstimate) {
  batch.clear();
  batch.reserve(countEstimate * 8);
}

void Renderer::SubmitSprite(const SpriteInstance &sprite) {
  batch.push_back(sprite);
}

void Renderer::RendBatch(glm::mat4 view, glm::mat4 projection) {
  if (batch.empty())
    return;

  Texture *tex = batch[0].texture;
  tex->Bind(0);

  // shader.SetUniform4f("aColor", batch[0].color);
  shader.SetUniformMat4f("aView", view);
  shader.SetUniformMat4f("aProjection", projection);

  std::vector<float> instances;
  instances.reserve(batch.size() * 8);

  // for (auto& s : batch) {
  //     SpriteInstance inst;
  //     inst.position = s.position;
  //     inst.size = s.size;
  //     inst.rotation = s.rotation;
  //     instances.push_back(inst);
  // }

  for (auto &s : batch) {
    instances.push_back(s.position.x);
    instances.push_back(s.position.y);
    instances.push_back(s.size.x);
    instances.push_back(s.size.y);
    instances.push_back(s.color.r);
    instances.push_back(s.color.g);
    instances.push_back(s.color.b);
    instances.push_back(s.color.a);
  }

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(glm::vec2),
                  instances.data());

  shader.Bind();
  glBindVertexArray(quadVAO);
  // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances.size());
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                          instances.size());
  glBindVertexArray(0);
}

void Renderer::initRenderData() {

  float vertices[] = {
      // pos        // tex
      -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left
      0.5f,  -0.5f, 1.0f, 0.0f, // Bottom-right
      -0.5f, 0.5f,  0.0f, 1.0f, // Top-left
      0.5f,  0.5f,  1.0f, 1.0f, // Top-right
  };

  unsigned int indices[] = {0, 1, 2, 1, 2, 3};

  GLCall(glGenVertexArrays(1, &this->quadVAO));
  GLCall(glBindVertexArray(this->quadVAO));

  GLCall(glGenBuffers(1, &quadVBO));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, quadVBO));
  GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                      GL_STATIC_DRAW));
  GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                               (void *)0));
  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                               (void *)(2 * sizeof(float))));
  GLCall(glEnableVertexAttribArray(1));

  GLCall(glGenBuffers(1, &quadEBO));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                      GL_STATIC_DRAW));

  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER,
               1000 *
                   (sizeof(glm::vec2) * 2 + sizeof(float) + sizeof(glm::vec4)),
               nullptr, GL_DYNAMIC_DRAW);

  int offset = 0;
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);
  offset += 2;

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                        (void *)(sizeof(float) * offset));
  glVertexAttribDivisor(3, 1);
  offset += 2;

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
                        (void *)(sizeof(float) * offset));
  glVertexAttribDivisor(4, 1);
  offset += 4;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  GLCall(glBindVertexArray(0));
}
