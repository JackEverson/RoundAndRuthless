#include <print>
#include "Engine.hpp"
#include "GameLoopScene.hpp"

int main() {
//#ifdef _DEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
	
	std::string name = "Look At The Garden!";
	// GardenEngine engine(name, 1280, 720);
	GardenEngine engine(name, 1920, 1080);

	//float fps = 30.0f;
	float fps = 60.0f;
	// float fps = 144.0f;

	auto scene = std::make_unique<GameLoopScene>();
	int engine_test_result = engine.Start(std::move(scene), fps);

	std::println("Engine Exit code: {}", engine_test_result);
	
	return engine_test_result;
}
