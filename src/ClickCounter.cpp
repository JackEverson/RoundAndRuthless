#include "ClickCounter.hpp"

ClickCounter::ClickCounter() :
    m_texture("res/textures/plant.jpeg"),
    m_shader("res/shaders/grubtexture.shader")
    //m_vertices(nullptr)
{


    float vertices[] = {
     0.1f,  0.1f, 0.0f, 1.0f, 1.0f,  // top right
     0.1f, -0.1f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.1f, -0.1f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.1f,  0.1f, 0.0f, 0.0f, 1.0f  // top left 
    };

    int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    m_vbo.Update(vertices, sizeof(vertices));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    m_vao.AddBuffer(m_vbo, layout);

    //vertices = new float[m_max_grub * 3];
    //m_ivbo.Update(m_vertices, sizeof(float) * m_max_grub * 3);
    //m_vao.AddInstancedBuffer(m_ivbo, 3);


}

ClickCounter::~ClickCounter()
{
}
