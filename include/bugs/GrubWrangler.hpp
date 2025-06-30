#include "BugWrangler.hpp"


class GrubWrangler : public BugWrangler {

public:
	GrubWrangler();
	~GrubWrangler();

	void Render() override;
	void AddBug(int index, float x, float y) override;

private:
	
	VertexArray m_vao;
	VertexBuffer m_vbo;
	VertexBuffer m_ivbo;
	IndexBuffer m_ebo;
	Texture m_texture;
	Shader m_shader;

	unsigned int m_grub_count = 0;

	float* m_vertices;
	unsigned int m_verticesCount = 0;

};