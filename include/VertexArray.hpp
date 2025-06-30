#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

class VertexArray{
private:
	unsigned int m_RendererID;
	unsigned int m_next_attrib_pointer = 0;
public:
	VertexArray();
	~VertexArray();

	
	
	void Bind() const;
	void Unbind() const;


	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void AddInstancedBuffer(const VertexBuffer& vb, unsigned int stride);

};
