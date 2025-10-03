#include <print>
#include "Engine.hpp"

#include "TestScene.hpp"

int main() {

	std::string name = "R&R TEST";
	GardenEngine engine(name, 1920, 1080);

	float fps = 60.0f;

	auto scene = std::make_unique<TestScene>();
	int engine_test_result = engine.Start(std::move(scene), fps);

	std::println("Engine Exit code: {}", engine_test_result);

	return engine_test_result;
}