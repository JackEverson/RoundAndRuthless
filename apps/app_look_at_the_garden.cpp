#include <iostream>
#include "engine.hpp"


int main() {

	std::cout << "Hello Garden!" << std::endl;
	
	int test = 6;

	test += 6;

	GardenEngine engine;

	std::string name = "Look At The Garden";
	int engine_test_result = engine.testengine(name);

	std::cout << "engine test result: " << engine_test_result << std::endl;
	
	return engine_test_result;
}