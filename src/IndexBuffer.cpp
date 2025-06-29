#include "IndexBuffer.hpp"
#include "Renderer.hpp"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count):
    m_Count(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint)); // paranoid check that unsigned int is the same size as GLuint

    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer():
    m_Count(6)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint)); // paranoid check that unsigned int is the same size as GLuint

    unsigned int indices[]{
    0, 1, 3,
    1, 2, 3
    };

    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));
}


IndexBuffer::~IndexBuffer(){
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}
void IndexBuffer::Unbind() const{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::Update(const unsigned int* data, unsigned int count) {
    ASSERT(sizeof(unsigned int) == sizeof(GLuint)); // paranoid check that unsigned int is the same size as GLuint

    GLCall(glDeleteBuffers(1, &m_RendererID));
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}
