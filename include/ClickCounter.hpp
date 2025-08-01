
class ClickCounter {
public:
	ClickCounter();
	~ClickCounter();

	void click() {
		clicks++;
	}
	
	void addClicks(unsigned int amount) {
		clicks += amount;
	}

	void ResetClicks() {
		clicks = 0;
	}

	unsigned int GetClicks() const {
		return clicks;
	}

	bool purchaseForClicks(unsigned int cost) {
		if (clicks >= cost) {
			clicks -= cost;
			return true;
		}
		return false;
	}

private:
	unsigned int clicks = 0;

};