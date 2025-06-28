#include <iostream>
#include "engine.hpp"


int main() {
	std::string name = "Look At The Garden!";
	GardenEngine engine(name, 1920, 1080);

	int engine_test_result = engine.Start();

	std::cout << "Engine Exit code: " << engine_test_result << std::endl;
	
	return engine_test_result;
}