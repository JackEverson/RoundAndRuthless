#pragma once
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

struct ShaderSource{
	const std::string VertexSource;
	const std::string FragmentSource;
};

class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();
	
	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float f);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private: 
	ShaderSource ParseShader(std::string& filepath);
	unsigned int CompileShaders(std::string vertexshaderstring, std::string fragmentshaderstring);

	unsigned int GetUniformLocation(const std::string& name);
};
