#include <iostream>  
#include <string>

#include "GrubWrangler.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

GrubWrangler::GrubWrangler() :
    m_texture("res/textures/sushi.png"),
    m_shader("res/shaders/grubtexture.shader"),
    m_vertices(nullptr)
{

    float vertices[] = {
         0.9f,  0.2f, 1.0f, 1.0f, 1.0f,  // top right
         0.2f, -0.2f, 1.0f, 1.0f, 0.0f, // bottom right
        -0.2f, -0.2f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.2f,  0.2f, 1.0f, 0.0f, 1.0f  // top left 
    };
    
    m_vbo.Update(vertices, sizeof(vertices));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_vao.AddBuffer(m_vbo, layout);

    //unsigned int indices[]{
    //    0, 1, 3,
    //    1, 2, 3
    //        };

    //m_ebo = IndexBuffer(indices, sizeof(indices));
    //m_ebo = IndexBuffer(indices, 6);
}

GrubWrangler::~GrubWrangler()
{
}

void GrubWrangler::Render()
{
    m_vao.Bind();
    m_vbo.Bind();
    m_ebo.Bind();
    m_shader.Bind();
    m_texture.Bind();

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    m_shader.SetUniform1i("atexCoord", 0);

    GLCall(glDrawElements(GL_TRIANGLES, m_ebo.GetCount(), GL_UNSIGNED_INT, nullptr));
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    glBindVertexArray(0);
}

void GrubWrangler::AddBug(int index) 
{
    //GLCall(glBindVertexArray(VAO));
    //GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));

    //const float vertices[] = {
    //     0.6f,  0.6f, 1.0f, 1.0f, 1.0f,  // top right
    //     0.6f, -0.6f, 1.0f, 1.0f, 0.0f, // bottom right
    //    -0.6f, -0.6f, 1.0f, 0.0f, 0.0f, // bottom left
    //    -0.6f,  0.6f, 1.0f, 0.0f, 1.0f// top left 
    //};
    //GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 5 * 4, &vertices));

}
