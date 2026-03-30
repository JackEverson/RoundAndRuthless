#pragma once
#include <string>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

struct ShaderSource {
  const std::string VertexSource;
  const std::string FragmentSource;
};

class Shader {
public:
  Shader(const std::string &filepath);
  Shader(const std::string &vertexshader, const std::string &fragmentshader);

  ~Shader();

  void Bind() const;
  void Unbind() const;

  void SetUniform1i(const std::string &name, int value);
  void SetUniform1f(const std::string &name, float f);
  void SetUniform3f(const std::string &name, float v0, float v1, float v2);
  void SetUniform3f(const std::string &name, glm::vec3 vec);
  void SetUniform4f(const std::string &name, float v0, float v1, float v2,
                    float v3);
  void SetUniform4f(const std::string &name, glm::vec4 vec);
  void SetUniformMat4f(const std::string &name, const glm::mat4 &matrix);

private:
  unsigned int m_RendererID;
  std::unordered_map<std::string, int> m_UniformLocationCache;

  ShaderSource ParseShader(const std::string &filepath);
  unsigned int CompileShaders(std::string vertexshaderstring,
                              std::string fragmentshaderstring);
  unsigned int GetUniformLocation(const std::string &name);
};
