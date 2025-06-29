#pragma once

class IndexBuffer{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
    IndexBuffer(const unsigned int* data, unsigned int count); // could change to 16 bit int for performance benefits
    IndexBuffer();
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    void Update(const unsigned int* data, unsigned int count);

    inline unsigned int GetCount() const { return m_Count; }
};
