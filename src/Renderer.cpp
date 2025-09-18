#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.hpp"
#include "gl_debug.hpp"
#include "glm/fwd.hpp"

#include <print>

Renderer::Renderer() :
    shader("./res/shaders/basictexture.shader"),
	backgroundShader("./res/shaders/background.shader")
{
  initRenderData();
}

Renderer::~Renderer() {}

void Renderer::Clear(float r, float g, float b, float a) const {
  glClearColor(r, g, b, a);
  GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::DrawBackground(const Texture& texture) {
	
    //Clear(sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);

    backgroundShader.Bind();
    texture.Bind(0);

	GLCall(glBindVertexArray(this->quadVAO));

    GLCall(glDisable(GL_BLEND));
    GLCall(glDisable(GL_DEPTH_TEST));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    backgroundShader.Unbind();
    glBindVertexArray(0);
}

void Renderer::DrawHallway(glm::mat4 view, glm::mat4 projection, Texture& floor, Texture& wall, Texture& ceiling) {
    int segs = 10;
    float wall_seg_sizes = 1.0f;
    float wall_height = 1.0f;
    float hallway_width = 1.0f;

    BeginBatchDraw(segs);
    for (int i = 0; i < segs; i++) {

        SpriteInstance wall_left;
        wall_left.position = glm::vec3(-hallway_width / 2, 0, -(float)i + 0.5f);
        wall_left.size = glm::vec2(1.0f, 1.0f);
        wall_left.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        wall_left.texture = &wall;
        SubmitSprite(wall_left);
    }
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    RendBatch(rotation, view, projection);

    BeginBatchDraw(segs);
    for (int i = 0; i < segs; i++) {

        SpriteInstance wall_right;
        wall_right.position = glm::vec3(hallway_width / 2, 0, -(float)i + 0.5f);
        wall_right.size = glm::vec2(1.0f, 1.0f);
        wall_right.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        wall_right.texture = &wall;
        SubmitSprite(wall_right);
    }
    glm::mat4 rotation2 = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    RendBatch(rotation2, view, projection);

    BeginBatchDraw(segs);
    for (int i = 0; i < segs; i++) {

        SpriteInstance sceiling;
        sceiling.position = glm::vec3(0, wall_height * 0.5f, -(float)i + 0.5f);
        sceiling.size = glm::vec2(1.0f, 1.0f);
        sceiling.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        sceiling.texture = &ceiling;
        SubmitSprite(sceiling);
    }
    glm::mat4 rotation3 = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    RendBatch(rotation3, view, projection);

    BeginBatchDraw(segs);
    for (int i = 0; i < segs; i++) {

        SpriteInstance sfloor;
        sfloor.position = glm::vec3(0, -wall_height * 0.5f, -(float)i + 0.5f);
        sfloor.size = glm::vec2(1.0f, 1.0f);
        sfloor.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        sfloor.texture = &floor;
        SubmitSprite(sfloor);
    }
    RendBatch(rotation3, view, projection);
}


void Renderer::BeginBatchDraw(int countEstimate) {
  batch.clear();
  batch.reserve(countEstimate * 9);
}

void Renderer::SubmitSprite(const SpriteInstance &sprite) {
  batch.push_back(sprite);
}

void Renderer::RendBatch(glm::mat4 view, glm::mat4 projection) {
    RendBatch(glm::identity<glm::mat4>(), view, projection);
}

void Renderer::RendBatch(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
  if (batch.empty())
    return;

  Texture *tex = batch[0].texture;
  tex->Bind(0);

  shader.Bind();
  shader.SetUniformMat4f("aModel", model);
  shader.SetUniformMat4f("aView", view);
  shader.SetUniformMat4f("aProjection", projection);

  std::vector<float> instances;
  instances.reserve(batch.size() * m_vertexSize);

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
    instances.push_back(s.position.z);
    instances.push_back(s.size.x);
    instances.push_back(s.size.y);
    instances.push_back(s.color.r);
    instances.push_back(s.color.g);
    instances.push_back(s.color.b);
    instances.push_back(s.color.a);
  }

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  //GLCall(glEnable(GL_DEPTH_TEST));
  GLCall(glDisable(GL_DEPTH_TEST));

  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(glm::vec2),
                  instances.data());

  glBindVertexArray(quadVAO);
  // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instances.size());
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                          instances.size() / m_vertexSize);
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

  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER,
               1000 * (sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(float) +
                       sizeof(glm::vec4)),
               nullptr, GL_DYNAMIC_DRAW);

  int offset = 0;
  // instancePos
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void *)0);
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);
  offset += 3;

  // instanceSize
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9,
                        (void *)(sizeof(float) * offset));
  glVertexAttribDivisor(3, 1);
  offset += 2;

  // instanceColor
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9,
                        (void *)(sizeof(float) * offset));
  glVertexAttribDivisor(4, 1);
  offset += 4;
}
