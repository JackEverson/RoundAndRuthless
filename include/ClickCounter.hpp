
#include "Texture.hpp"
#include "Shader.hpp"


class ClickCounter {

public:

private:
	unsigned int clicks = 0;
	unsigned int m_click_value = 1;
	




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