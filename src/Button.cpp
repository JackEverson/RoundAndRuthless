
#include <print>
#include "Button.hpp"

#include <glm/ext/matrix_projection.hpp>
#include <GLFW/glfw3.h>




Button::Button(glm::vec3 worldPos, glm::vec2 size, Texture texture, std::function<void()> onClickFunc) :
	m_worldPosition(worldPos),
	m_size(size),
	texture(texture),
	onClick(onClickFunc) {


}

Button::~Button() {
}

void Button::click() {
	if (onClick) onClick();
}

bool Button::IsMouseOverButton(
	const glm::mat4 & viewMatrix, 
	const glm::mat4 & projectionMatrix, 
	const glm::vec2 & mousePos,
	int screenWidth,
	int screenHeight){

	glm::vec3 screenPosTL = glm::project(
		glm::vec3(m_worldPosition.x - m_size.x / 2, m_worldPosition.y - m_size.y / 2, m_worldPosition.z),
		viewMatrix,
		projectionMatrix,
		glm::vec4(0, 0, screenWidth, screenHeight) // usually glm::vec4(0, 0, screenWidth, screenHeight)
	);

	glm::vec3 screenPosBR = glm::project(
		glm::vec3(m_worldPosition.x + m_size.x / 2, m_worldPosition.y + m_size.y / 2, m_worldPosition.z),
		viewMatrix,
		projectionMatrix,
		glm::vec4(0, 0, screenWidth, screenHeight) // usually glm::vec4(0, 0, screenWidth, screenHeight)
	);
	
	std::println("mouse: {}x{}, TL: {}x{}, BR: {}x{}", mousePos.x, mousePos.y, screenPosTL.x, screenPosTL.y, screenPosBR.x, screenPosBR.y);

	return mousePos.x >= screenPosTL.x &&
		mousePos.x <= screenPosBR.x &&
		mousePos.y >= screenPosTL.y &&
		mousePos.y <= screenPosBR.y;
}



