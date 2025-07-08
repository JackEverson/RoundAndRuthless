#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"

Camera::Camera(){
	m_z = -1.0f;
}

Camera::~Camera() {

}

glm::vec3 Camera::GetLocation() const
{
	return glm::vec3(m_x, m_y, m_z);
}

glm::mat4 Camera::GetViewMat(){
	// Converts Worldspace to Viewspace
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(-m_x, -m_y, m_z)); 
	//glm::mat4 view = glm::lookAt(
	//	glm::vec3(m_x, m_y, m_z), // Camera position
	//	glm::vec3(0.0f, 0.0f, 0.0f), // Look at point
	//	glm::vec3(0.0f, 1.0f, 0.0f) // Up vector
	//);
	return view;
}

glm::mat4 Camera::GetProjectionMat(float screen_width, float screen_height) {
	// Converts Viewspace to Clipspace
	glm::mat4 proj = glm::perspective(glm::radians(m_fov), screen_width / screen_height, m_clip_near, m_clip_far);
	//glm::mat4 proj = glm::ortho(0.0f, screen_width, 0.0f, screen_height, m_clip_near, m_clip_far);
	return proj;
}
