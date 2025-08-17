#pragma once

#include <glm/glm.hpp>

#include "Shader.hpp"
#include "Texture.hpp"


struct SpriteInstance {
    glm::vec2 position;
    glm::vec2 size;
    float rotation;
    glm::vec4 color;
    Texture* texture; // optional, if you batch by texture
};


class Renderer{

private:
    Shader shader;
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
    unsigned int quadEBO = 0;
    unsigned int instanceVBO = 0;
	std::vector<SpriteInstance> batch;

public:
    Renderer();
    ~Renderer();
    
    void Clear(float r, float g, float b, float a) const;

    void DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec4 color);
	void BeginBatchDraw(int countEstimate);
	void SubmitSprite(const SpriteInstance& sprite);
	void RendBatch(glm::mat4 view, glm::mat4 projection);


private:

    void initRenderData();
};
