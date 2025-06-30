#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "Renderer.hpp"


VertexArray::VertexArray(){
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray(){
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout){
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++){
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count, element.type, 
            element.normalized, layout.GetStride(), (const void*)offset));
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type); 
        m_next_attrib_pointer++;
    }
}

void VertexArray::AddInstancedBuffer(const VertexBuffer& vb, unsigned int stride){
    Bind();
    vb.Bind();

    GLCall(glEnableVertexAttribArray(m_next_attrib_pointer));
    GLCall(glVertexAttribPointer(m_next_attrib_pointer, stride, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)0));
    GLCall(glVertexAttribDivisor(m_next_attrib_pointer, 1));
    m_next_attrib_pointer++;
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexArray::Bind() const{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const{
    GLCall(glBindVertexArray(0));
}
