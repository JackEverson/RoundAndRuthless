#include "InstanceBuffer.hpp"

InstanceBuffer::InstanceBuffer(const void* data, unsigned int size) :
    m_init_size(size)
{
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

VertexBuffer::VertexBuffer() {
    float vertices[] = {
     0.1f,  0.1f, 1.0f, 1.0f, 1.0f,  // top right
     0.1f, -0.1f, 1.0f, 1.0f, 0.0f, // bottom right
    -0.1f, -0.1f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.1f,  0.1f, 1.0f, 0.0f, 1.0f  // top left 
    };

    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW));
}


VertexBuffer::~VertexBuffer() {
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Update(const void* data, unsigned int size) {
    Bind();
    if (m_init_size > size) {
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
    }
    else {
        unsigned int newID;
        GLCall(glGenBuffers(1, &newID));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, newID));
        GLCall(GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW)));
        GLCall(glDeleteBuffers(1, &m_RendererID));
        m_RendererID = newID;
    }

}

void VertexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}
void VertexBuffer::Unbind() const {
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}