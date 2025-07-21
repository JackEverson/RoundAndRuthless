cmake_minimum_required(VERSION 3.4...4.0)

project(LookAtTheGarden VERSION 1.0
                    DESCRIPTION "GardenEngine for look at the garden"
		    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
if(MSVC)
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -WX-")
else()
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
endif()

#External libraries
add_subdirectory(external/glad)
add_subdirectory(external/glfw)
add_subdirectory(external/stb_image)
add_subdirectory(external/glm)

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
set(GardenEngine_include_bugs include/bugs )
set(GardenEngine_src_files
	${GardenEngine_src}/Engine.cpp
	${GardenEngine_src}/IndexBuffer.cpp
	${GardenEngine_src}/Renderer.cpp
	${GardenEngine_src}/Shader.cpp
	${GardenEngine_src}/Texture.cpp
	${GardenEngine_src}/VertexArray.cpp
	${GardenEngine_src}/VertexBuffer.cpp
	${GardenEngine_src}/VertexBufferLayout.cpp
	${GardenEngine_src}/Camera.cpp

	${GardenEngine_src}/bugs/BugWrangler.cpp
	${GardenEngine_src}/bugs/GrubWrangler.cpp

	# include files included for IDE sake
	${GardenEngine_include}/Engine.hpp
	${GardenEngine_include}/IndexBuffer.hpp
	${GardenEngine_include}/Renderer.hpp
	${GardenEngine_include}/Shader.hpp
	${GardenEngine_include}/Texture.hpp
	${GardenEngine_include}/VertexArray.hpp
	${GardenEngine_include}/VertexBuffer.hpp
	${GardenEngine_include}/VertexBufferLayout.hpp
	${GardenEngine_include}/Camera.hpp

	${GardenEngine_include_bugs}/BugWrangler.hpp
	${GardenEngine_include_bugs}/GrubWrangler.hpp
)

add_library(GardenEngine STATIC ${GardenEngine_src_files})
target_include_directories(GardenEngine PUBLIC ${GardenEngine_include} ${GardenEngine_include_bugs} )
target_compile_features(GardenEngine PUBLIC cxx_std_23)
target_link_libraries(GardenEngine PUBLIC
	glad 
	glfw
	stb_image 
	glm
	imgui
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


#LookAtTheGarden executable
add_executable(${PROJECT_NAME} apps/app_look_at_the_garden.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC GardenEngine )
add_dependencies(${PROJECT_NAME} copy_resources)

