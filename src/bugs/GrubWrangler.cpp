#include <iostream>  
#include <string>

#include <glm/glm.hpp>

#include "GrubWrangler.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

GrubWrangler::GrubWrangler() :
    m_texture("res/textures/sushi.png"),
    m_shader("res/shaders/grubtexture.shader"),
    m_vertices(nullptr)
{

    float vertices[] = {
         0.1f,  0.1f, 0.0f, 1.0f, 1.0f,  // top right
         0.1f, -0.1f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.1f, -0.1f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.1f,  0.1f, 0.0f, 0.0f, 1.0f  // top left 
    };
    m_vbo.Update(vertices, sizeof(vertices));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_vao.AddBuffer(m_vbo, layout);

    m_vertices = new float[m_max_grub * 3];
    m_ivbo.Update(m_vertices, sizeof(float) * m_max_grub * 3);
    m_vao.AddInstancedBuffer(m_ivbo, 3);


	this->AddBug(0, 0.0f, 0.0f); // Add the first bug at the origin
    //unsigned int indices[]{
    //    0, 1, 3,
    //    1, 2, 3
    //        };
    //m_ebo = IndexBuffer(indices, 6);
}

GrubWrangler::~GrubWrangler()
{
    delete m_vertices;
    m_vertices = nullptr;
}

void GrubWrangler::Render(const glm::mat4& view, const glm::mat4& projection)
{
    m_vao.Bind();
    m_vbo.Bind();
    m_ebo.Bind();
    m_ivbo.Bind();
    m_shader.Bind();
    m_texture.Bind();

    m_shader.SetUniformMat4f("view", view);
	m_shader.SetUniformMat4f("projection", projection);

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    glDrawElementsInstanced(GL_TRIANGLES, m_ebo.GetCount(), GL_UNSIGNED_INT, 0, m_grub_count);
    //GLCall(glDrawElements(GL_TRIANGLES, m_ebo.GetCount(), GL_UNSIGNED_INT, nullptr));
    //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    //glBindVertexArray(0);
}

void GrubWrangler::AddBug(int index, float x, float y) 
{
    m_grub_count++;
    m_grub_count = m_grub_count % m_max_grub;


    m_vertices[m_grub_count * 3 + 0] = x;
    m_vertices[m_grub_count * 3 + 1] = y;
    m_vertices[m_grub_count * 3 + 2] = 0.0f;


    m_ivbo.Bind();
    m_vao.Bind();


    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_max_grub * 3, m_vertices));

    m_ivbo.Unbind();
    m_vao.Unbind();
}
