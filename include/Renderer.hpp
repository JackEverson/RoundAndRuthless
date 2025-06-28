#pragma once
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "Shader.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"

// #define ASSERT(x) if (!(x)) __debugbreak(); // for MSVC 
#define ASSERT(x) if (!(x)) std::cout << "OpenGL Error" << std::endl;

#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);


class Renderer{
public:
    void Clear(float r, float g, float b, float a) const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;


};
