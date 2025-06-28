cmake_minimum_required(VERSION 3.4...4.0)

project(LookAtTheGarden VERSION 1.0
                    DESCRIPTION "GardenEngine for look at the garden"
		    LANGUAGES C CXX)

# set(CMAKE_CXX_STANDARD 17)
if(MSVC)
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -WX-")
else()
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")
endif()


#GLAD
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)


#GLFW - prebuilt
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/glfw3/lib/glfw3.lib)
target_include_directories(glfw INTERFACE ${CMAKE_SOURCE_DIR}/external/glfw3/include)

# find_library(glfw NAMES glfw3 PATHS external/glfw3/lib)
# target_include_directories(glfw PUBLIC external/glfw3/include)

#stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE external/stb_image/)

#GardenEngine
set(GargenEngine_src src)
set(GardenEngine_include include)
set(GardenEngine_src_files
	${GargenEngine_src}/Engine.cpp
	${GargenEngine_src}/IndexBuffer.cpp
	${GargenEngine_src}/Renderer.cpp
	${GargenEngine_src}/Shader.cpp
	${GargenEngine_src}/Texture.cpp
	${GargenEngine_src}/VertexArray.cpp
	${GargenEngine_src}/VertexBuffer.cpp
	${GargenEngine_src}/VertexBufferLayout.cpp
)

add_library(GardenEngine STATIC ${GardenEngine_src_files})
target_include_directories(GardenEngine PUBLIC ${GardenEngine_include} )
target_compile_features(GardenEngine PUBLIC cxx_std_17)
target_link_libraries(GardenEngine PUBLIC 
	glad 
	glfw
	stb_image 
)

#Copy shaders and textures into build
file(GLOB res_files "${PROJECT_SOURCE_DIR}/res/*")
add_custom_target(res_files
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
	"${CMAKE_CURRENT_SOURCE_DIR}/res/"
	"res/"
	COMMENT "Copying Resources"
)

#LookAtTheGarden executable
add_executable(${PROJECT_NAME} apps/app_look_at_the_garden.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC GardenEngine)
