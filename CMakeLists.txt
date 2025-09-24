cmake_minimum_required(VERSION 3.4...4.0)

project(RoundAndRuthless VERSION 1.0
		    DESCRIPTION "Round And Ruthless game"
		    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -WX-")
else()
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++23")
endif()

#External libraries
add_subdirectory(external/glad)
add_subdirectory(external/glfw)
add_subdirectory(external/single_headers)
add_subdirectory(external/glm)

set(LIBTYPE STATIC CACHE STRING "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
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
#GardenEngine
set(GardenEngine_src src)
set(GardenEngine_include include )
set(GardenEngine_src_files
	${GardenEngine_src}/Engine.cpp
	${GardenEngine_src}/Renderer.cpp
	${GardenEngine_src}/Shader.cpp
	${GardenEngine_src}/Texture.cpp
	${GardenEngine_src}/Camera.cpp
	${GardenEngine_src}/ClickCounter.cpp
	${GardenEngine_src}/Audio.cpp
	${GardenEngine_src}/Approacher.cpp
	${GardenEngine_src}/Button.cpp

	# include files included for IDE sake
	${GardenEngine_include}/Engine.hpp
	${GardenEngine_include}/Renderer.hpp
	${GardenEngine_include}/Shader.hpp
	${GardenEngine_include}/Texture.hpp
	${GardenEngine_include}/Camera.hpp
	${GardenEngine_include}/ClickCounter.hpp
	${GardenEngine_include}/gl_debug.hpp
	${GardenEngine_include}/Audio.hpp
	${GardenEngine_include}/Approacher.hpp
	${GardenEngine_include}/Scene.hpp
	${GardenEngine_include}/Button.hpp
)

add_library(GardenEngine STATIC ${GardenEngine_src_files})
target_include_directories(GardenEngine PUBLIC ${GardenEngine_include} )
target_compile_features(GardenEngine PUBLIC cxx_std_23)
target_link_libraries(GardenEngine PUBLIC
	glad 
	glfw
	single_headers 
	glm
	imgui
	OpenAL
)

#Copy Resources
add_custom_target(copy_resources
	COMMENT "Copying Resources"
)

add_custom_command(TARGET copy_resources PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
	"${CMAKE_SOURCE_DIR}/res/"
	"res/"
)

#GamePlay code
set(GamePlay_files
	apps/LiverGameState.cpp

	apps/LiverGameState.hpp
	apps/GameLoopScene.hpp
	apps/IntroScene.hpp
)
add_library(GamePlay STATIC ${GamePlay_files})
target_include_directories(GamePlay PUBLIC ${apps})
target_compile_features(GamePlay PUBLIC cxx_std_23)
target_link_libraries(GamePlay PUBLIC GardenEngine)


#RoundAndRuthless executable

if(MSVC)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")
endif()

add_executable(${PROJECT_NAME} apps/round_and_ruthless.cpp ${APP_ICON_RESOURCE})
target_link_libraries(${PROJECT_NAME} PUBLIC 
	GardenEngine 
	GamePlay
)
add_dependencies(${PROJECT_NAME} copy_resources)

add_executable("test" apps/test.cpp)
target_link_libraries("test" PUBLIC GardenEngine OpenAL)
