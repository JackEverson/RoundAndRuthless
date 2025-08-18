#include "ClickCounter.hpp"

ClickCounter::ClickCounter() 
{
}

ClickCounter::~ClickCounter()
{
}


void ClickCounter::UpgradeClickValue()
{
	if (purchaseForClicks(GetClickUpgradeValue())) {
		m_click_value *= 2;
		m_click_upgrade_cost *= 3;
	}
}

bool ClickCounter::purchaseForClicks(unsigned int cost) {
	if (clicks >= cost) {
		clicks -= cost;
		return true;
	}
	return false;
}