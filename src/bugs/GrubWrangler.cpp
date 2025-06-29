#include <iostream>  
#include <string>

#include "GrubWrangler.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

GrubWrangler::GrubWrangler() :
	m_texture("res/textures/sushi.png"),
	m_shader("res/shaders/grubtexture.shader")
{
    Shader m_shader("res/shaders/basictexture.shader");
    Texture m_texture("res/textures/sushi.png");
}

GrubWrangler::~GrubWrangler()
{
    GLCall(glDeleteVertexArrays(1, &VAO));
    GLCall(glDeleteBuffers(1, &VBO));
    GLCall(glDeleteBuffers(1, &EBO));
}

void GrubWrangler::Render()
{
    unsigned int indices[]{
    0, 1, 3,
    1, 2, 3
    };

    const float vertices[] = {
     0.6f,  0.6f, 1.0f, 1.0f, 1.0f,  // top right
     0.6f, -0.6f, 1.0f, 1.0f, 0.0f, // bottom right
    -0.6f, -0.6f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.6f,  0.6f, 1.0f, 0.0f, 1.0f// top left 
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray va;
    VertexBuffer vb(vertices, 4 * 5 * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);
    IndexBuffer ib(indices, 6);
    //Shader shader ("res/shaders/basic.shader");
    m_shader.Bind();
    //Texture texture("res/textures/container.jpg");
    //Texture texture("res/textures/container2.png");
    m_texture.Bind();

    m_shader.SetUniform1i("atexCoord", 0);


    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));





    
    //GLCall(glBindVertexArray(VAO));
    //GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    //GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    //m_shader.Bind();
    //m_texture.Bind();

    //GLCall(glEnable(GL_BLEND));
    //GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    //m_shader.SetUniform3f("aLoc", 0.0f, 0.0f, 0.0f);
    //m_shader.SetUniform3f("aColor", 0.1f, 0.1f, 0.1f);

    //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    //// Unbind
    //glBindVertexArray(0);
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
