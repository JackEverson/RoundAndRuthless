
class InstanceBuffer {
private:
    unsigned int m_RendererID;
public:
    InstanceBuffer(const void* data, unsigned int size);
    InstanceBuffer();
    ~InstanceBuffer();

    unsigned int m_init_size;

    void Bind() const;
    void Unbind() const;

    void Update(const void* data, unsigned int size);

};