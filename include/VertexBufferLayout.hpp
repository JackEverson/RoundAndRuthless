#pragma once

#include <cassert>
#include <vector>
#include <glad/glad.h>


struct VertexBufferElement{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type){
	switch(type){
	    case GL_FLOAT:	    return sizeof(GLfloat);
	    case GL_UNSIGNED_INT:   return sizeof(GLuint);
	    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
	}
	assert(false);
	return 0;
    }
};

class VertexBufferLayout{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
VertexBufferLayout():
    m_Stride(0){};

    template<typename T>
    void Push(unsigned int count){
	static_assert(false, "Unsupported type for VertexBufferLayout::Push");
    }

    inline const std::vector<VertexBufferElement>& GetElements() const{ return m_Elements; }
    inline unsigned int GetStride() const{ return m_Stride; }
};

template<>
void VertexBufferLayout::Push<float>(unsigned int count);

template<>
void VertexBufferLayout::Push<unsigned int>(unsigned int count);

template<>
void VertexBufferLayout::Push<unsigned char>(unsigned int count);
