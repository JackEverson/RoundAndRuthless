#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture.hpp"

struct SpriteInstance {
  glm::vec3 position;
  glm::vec2 size;
  float rotation;
  glm::vec4 color;
  Texture *texture;
};

class Renderer {
private:
	Shader shader;
	Shader backgroundShader;
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	unsigned int quadEBO = 0;
	unsigned int instanceVBO = 0;

	unsigned int backgroundVAO = 0;
	unsigned int backgroundVBO = 0;

	std::vector<SpriteInstance> batch;

	int m_vertexSize = 9;

public:
	Renderer();
	~Renderer();

	void Clear(float r, float g, float b, float a) const;
	void DrawBackground(const SpriteInstance& sprite);

	void DrawHallway(glm::mat4 view, glm::mat4 projection, Texture& floor, Texture& wall, Texture& ceiling);


	//void DrawSprite(Texture &texture, glm::vec2 position, glm::vec2 size,
	//float rotate, glm::vec4 color, glm::mat4 projection);
	void BeginBatchDraw(int countEstimate);
	void SubmitSprite(const SpriteInstance &sprite);
	void RendBatch(glm::mat4 view, glm::mat4 projection);
	void RendBatch(glm::mat4 model, glm::mat4 view, glm::mat4 projection);

private:
  void initRenderData();
};
