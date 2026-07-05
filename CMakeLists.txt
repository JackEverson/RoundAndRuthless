cmake_minimum_required(VERSION 3.4...4.0)

project(RoundAndRuthless VERSION 1.0
      DESCRIPTION "Round And Ruthless,
      a short horror game about liver and roundies"
      LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -WX-")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -WX-")
else()
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++23")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
endif()


#External libraries
add_subdirectory(external/glad)
add_subdirectory(external/glfw)
add_subdirectory(external/single_headers)
add_subdirectory(external/glm)

set(LIBTYPE STATIC CACHE STRING "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF)
add_subdirectory(external/openal-soft)


#ImGui
file(GLOB imgui_src
"external/imgui/*.cpp"
"external/imgui/*.h"
"external/imgui/misc/cpp/imgui_stdlib.*"
external/imgui/backends/imgui_impl_opengl3.cpp
external/imgui/backends/imgui_impl_opengl3.h
external/imgui/backends/imgui_impl_glfw.cpp
external/imgui/backends/imgui_impl_glfw.h
)
add_library(imgui STATIC ${imgui_src})
target_include_directories(imgui PUBLIC
  external/imgui
  external/imgui/backends
)
target_link_libraries(imgui PUBLIC
  glfw
)

#my libraries and resources
#garden_engine
set(garden_engine_src_files
  garden_engine/Audio.cpp
  garden_engine/Engine.cpp
  garden_engine/Renderer.cpp
  garden_engine/Shader.cpp
  garden_engine/Texture.cpp

  # include files included for IDE sake
  garden_engine/Audio.hpp
  garden_engine/Button.hpp
  garden_engine/Camera.hpp
  garden_engine/Engine.hpp
  garden_engine/FPSController.hpp
  garden_engine/gl_debug.hpp
  garden_engine/TriggerVolume.hpp
  garden_engine/NotificationManager.hpp
  garden_engine/PointLight.hpp
  garden_engine/Renderer.hpp
  garden_engine/Scene.hpp
  garden_engine/Shader.hpp
  garden_engine/ShaderSource.hpp
  garden_engine/Surface.hpp
  garden_engine/Texture.hpp
)

add_library(garden_engine STATIC ${garden_engine_src_files})
target_include_directories(garden_engine PUBLIC garden_engine)
target_compile_features(garden_engine PUBLIC cxx_std_23)
target_compile_definitions(garden_engine PUBLIC GLFW_INCLUDE_NONE)
target_link_libraries(garden_engine PUBLIC
  glad
  glfw
  single_headers
  glm
  imgui
  OpenAL
)

# Precompiled header: parse these heavy, rarely-changing headers ONCE and reuse
# across every translation unit. PUBLIC so targets linking garden_engine reuse it.
# NOTE: keep json.hpp OUT — it's only used in SaveSystem.cpp; adding it here would
# re-impose its cost on every TU.
target_precompile_headers(garden_engine PUBLIC
  <vector>
  <string>
  <memory>
  <map>
  <functional>
  <algorithm>
  <glm/glm.hpp>
  <imgui.h>
)

#Copy Resources
add_custom_target(copy_resources
COMMENT "Copying Resources"
)

add_custom_command(TARGET copy_resources PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
"${CMAKE_CURRENT_SOURCE_DIR}/res/"
"${CMAKE_BINARY_DIR}/res/"
)

set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")

#RoundAndRipe executable
set(RoundAndRipe_files
  RoundAndRipe/GardenScene.cpp
  RoundAndRipe/SaveSystem.cpp

  RoundAndRipe/Event.hpp
  RoundAndRipe/GardenScene.hpp
  RoundAndRipe/GardenRoom.hpp
  RoundAndRipe/Tile.hpp
  RoundAndRipe/Field.hpp
  RoundAndRipe/Plants.hpp
  RoundAndRipe/SaveSystem.hpp
  RoundAndRipe/RoundAndRipeEvents.hpp
  RoundAndRipe/Structures.hpp
)

add_library(RoundAndRipe_game STATIC ${RoundAndRipe_files})
target_include_directories(RoundAndRipe_game PUBLIC RoundAndRipe)
target_compile_features(RoundAndRipe_game PUBLIC cxx_std_23)
target_link_libraries(RoundAndRipe_game PUBLIC garden_engine)

add_executable(RoundAndRipe RoundAndRipe/RoundAndRipe.cpp ${APP_ICON_RESOURCE})
target_link_libraries(RoundAndRipe PUBLIC RoundAndRipe_game )
add_dependencies(RoundAndRipe copy_resources)


# #RoundAndRuthless executable
# set(GamePlay_files
#   game/TestScene.hpp
#   game/Room1Scene.hpp
# )
# add_library(GamePlay STATIC ${GamePlay_files})
# target_include_directories(GamePlay PUBLIC game)
# target_compile_features(GamePlay PUBLIC cxx_std_23)
# target_link_libraries(GamePlay PUBLIC garden_engine)

# add_executable(${PROJECT_NAME} game/RoundAndRuthless.cpp ${APP_ICON_RESOURCE})
# target_link_libraries(${PROJECT_NAME} PUBLIC
# garden_engine
# GamePlay
# )
# add_dependencies(${PROJECT_NAME} copy_resources)