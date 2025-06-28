#include "Renderer.hpp"

#include <iostream>

void GLClearError(){
    while(glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, int line){
    while(GLenum error = glGetError()){
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear(float r, float g, float b, float a) const{
    glClearColor(r, g, b, a);
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const{
    shader.Bind();
    va.Bind();
    ib.Bind();
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
