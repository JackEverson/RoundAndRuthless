
#include "Texture.hpp"
#include "Shader.hpp"


class ClickCounter {

public:

private:
	unsigned int clicks = 0;
	unsigned int m_click_value = 1;
	
	VertexArray m_vao;
	VertexBuffer m_vbo;
	VertexBuffer m_ivbo;
	IndexBuffer m_ebo;
	Texture m_texture;
	Shader m_shader;



public:
	ClickCounter();
	~ClickCounter();

	void click() { clicks += m_click_value; }
	void addClicks(unsigned int amount) { clicks += amount; }
	void ResetClicks() { clicks = 0; }
	unsigned int GetClicks() const { return clicks; }

	bool purchaseForClicks(unsigned int cost) {
		if (clicks >= cost) {
			clicks -= cost;
			return true;
		}
		return false;
	}

private:

};