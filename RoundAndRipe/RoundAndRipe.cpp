#include "MenuScene.hpp"

#include "Engine.hpp"

#include <cstdio>

int main() {
  std::string name = "Round And Ripe";
  // GardenEngine engine(name, true, 1920, 1080);
  GardenEngine engine(name, false, 1920, 1080);

  float fps = 144.0f;

  // auto scene = std::make_unique<TestScene>();
  auto scene = std::make_unique<MenuScene>();
  int engine_test_result = engine.Start(std::move(scene), fps);

  printf("Engine Exit code: %d\n", engine_test_result);

  return engine_test_result;
}
