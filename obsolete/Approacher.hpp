#pragma once

#include "Texture.hpp"

#include <chrono>

class Approacher {
public:
	float m_distanceAway;
	float m_approachSpeed;
	bool stopped = false;

private:
	std::chrono::steady_clock::time_point last_cycle_time;

public:

	Approacher(float distance, float speed_per_second);
	~Approacher();
	
	void Step();

private:



};