
#include "Texture.hpp"
#include "Shader.hpp"


class ClickCounter {

public:

private:
	unsigned int clicks = 0;

	unsigned int m_click_value = 1;
	unsigned int m_click_upgrade_cost = 100;


public:
	ClickCounter();
	~ClickCounter();

	void click() { clicks += m_click_value; }
	void addClicks(unsigned int amount) { clicks += amount; }
	void removeClicks(unsigned int amount) { 
		if (amount > clicks) clicks = 0;
		else clicks -= amount;
	}
	void ResetClicks() { clicks = 0; }
	
	unsigned int GetClicks() const { return clicks; }
	
	unsigned int GetClickUpgradeValue() { return m_click_upgrade_cost; }
	void UpgradeClickValue();

private:
	bool purchaseForClicks(unsigned int cost);

private:

};
