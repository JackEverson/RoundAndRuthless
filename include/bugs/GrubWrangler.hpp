#include "BugWrangler.hpp"


class GrubWrangler : public BugWrangler {

public:
	GrubWrangler();
	~GrubWrangler();

	void Render() override;
	void AddBug(int index) override;

private:
	
	VertexArray m_vao;
	VertexBuffer m_vbo;
	IndexBuffer m_ebo;
	Texture m_texture;
	Shader m_shader;


	float* m_vertices;
	unsigned int m_verticesCount = 0;

};