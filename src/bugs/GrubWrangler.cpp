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
         1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  // top right
         1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 1.0f, 0.0f, 1.0f  // top left 
    };
    m_vbo.Update(vertices, sizeof(vertices));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_vao.AddBuffer(m_vbo, layout);

    //////
    float instance[] = {
        0.8f, 0.5f,  0.0f,
        -0.8f, 0.5f,  0.0f,
        0.0f,  0.0f,  0.0f,
         0.8f, -1.0f,  0.0f,
        -0.8f, -1.0f,  0.0f,
    };
    m_grub_count = 5;
    /////

    m_ivbo.Update(instance, sizeof(instance));
    m_vao.AddInstancedBuffer(m_ivbo, 3);

    //unsigned int indices[]{
        //0, 1, 3,
        //1, 2, 3
            //};
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
    m_ivbo.Bind();
    m_shader.Bind();
    m_texture.Bind();

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    glDrawElementsInstanced(GL_TRIANGLES, m_ebo.GetCount(), GL_UNSIGNED_INT, 0, m_grub_count);
    //GLCall(glDrawElements(GL_TRIANGLES, m_ebo.GetCount(), GL_UNSIGNED_INT, nullptr));
    //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    //glBindVertexArray(0);
}

void GrubWrangler::AddBug(int index, float x, float y) 
{
    /*GLCall(glBindVertexArray(m_vao));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo));*/

    //const float vertices[] = {
    //     0.6f,  0.6f, 1.0f, 1.0f, 1.0f,  // top right
    //     0.6f, -0.6f, 1.0f, 1.0f, 0.0f, // bottom right
    //    -0.6f, -0.6f, 1.0f, 0.0f, 0.0f, // bottom left
    //    -0.6f,  0.6f, 1.0f, 0.0f, 1.0f// top left 
    //};
    //GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 5 * 4, &vertices));

}
