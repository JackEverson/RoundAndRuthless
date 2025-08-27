#include <glm/glm.hpp>


class Camera {
	
public:

	
	Camera();
	~Camera();

	glm::vec3 GetLocation() const;

	glm::mat4 GetViewMat();
	glm::mat4 GetProjectionMat(float screen_width, float screen_height);

	void ShiftCamera(float x, float y, float z) {
		m_x += x;
		m_y += y;
		m_z += z;
	}

private:
	//glm::mat3 m_location;
	float m_x = 0;
	float m_y = 0;
	float m_z = 0;

	float m_fov = 45.0f;
	float m_clip_near = 0.00001f;
	float m_clip_far = 100.0f;

};
