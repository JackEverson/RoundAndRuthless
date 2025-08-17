#include <print>

#include "Engine.hpp"


int main() {
	std::string name = "Look At The Garden!";
	GardenEngine engine(name, 1280, 720);
	//GardenEngine engine(name, 1920, 1080);

	//float fps = 30.0f;
	//float fps = 60.0f;
	float fps = 144.0f;
	int engine_test_result = engine.Start(fps);

	std::println("Engine Exit code: {}", engine_test_result);
	
	return engine_test_result;
}