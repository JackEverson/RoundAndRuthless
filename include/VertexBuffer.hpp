#pragma once

class VertexBuffer{
private:
	unsigned int m_RendererID;
public:
    VertexBuffer(const void* data, unsigned int size);
    VertexBuffer();
    ~VertexBuffer();

    unsigned int m_init_size;

    void Bind() const;
    void Unbind() const;

    void Update(const void* data, unsigned int size);

};
