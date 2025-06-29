#include "BugWrangler.hpp"


class GrubWrangler : public BugWrangler {

public:
	GrubWrangler();
	~GrubWrangler();

	void Render() override;
	void AddBug(int index) override;

private:

	Texture m_texture;
	Shader m_shader;

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;


};