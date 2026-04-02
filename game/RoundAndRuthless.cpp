#include "Engine.hpp"
#include <cstdio>

// #include "IntroScene.hpp"
#include "TestScene.hpp"

int main() {
  // #ifdef _DEBUG
  //	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  // #endif

  std::string name = "Round And Ruthless";
  GardenEngine engine(name, true, 1920, 1080);
  // GardenEngine engine(name, false, 1920, 1080);

  // float fps = 30.0f;
  //  float fps = 60.0f;
  float fps = 144.0f;

  auto scene = std::make_unique<TestScene>();
  int engine_test_result = engine.Start(std::move(scene), fps);

  printf("Engine Exit code: %d\n", engine_test_result);

  return engine_test_result;
}
