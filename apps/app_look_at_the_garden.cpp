#include <iostream>
#include "Engine.hpp"


int main() {
	std::string name = "Look At The Garden!";
	GardenEngine engine(name, 1920, 1080);

	float fps = 60.0f;
	int engine_test_result = engine.Start(fps);

	std::cout << "Engine Exit code: " << engine_test_result << std::endl;
	
	return engine_test_result;
}