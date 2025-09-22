#pragma once

#include "Texture.hpp"

#include <functional>
#include <glm/glm.hpp>

class Button{
private:
	std::function<void()> onClick;

public:
	Texture texture;
	glm::vec3 m_worldPosition;
	glm::vec2 m_size;

public:

	Button(glm::vec3 worldPos, glm::vec2 size, Texture texture, std::function<void()> onClickFunc);
	~Button();

	bool IsMouseOverButton(
		const glm::mat4& viewMatrix,
		const glm::mat4& projectionMatrix,
		const glm::vec2& mousePos,
		int screenWidth,
		int screenHeight);
	
	void click();

};
