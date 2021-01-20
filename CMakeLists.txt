cmake_minimum_required(VERSION 3.14.5)

project("Ogle")

# Todo: Technically, GLFW and GLAD shouldn't be directly accessible by the user of the framework
set(OGLE_INCLUDE_DIRS
	"${CMAKE_CURRENT_SOURCE_DIR}/Source"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/glfw/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/glm/"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/glad/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image")

set(OGLE_SOURCE
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Application.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Mesh.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Shader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Texture2D.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Camera.cpp"
	
	"${CMAKE_CURRENT_SOURCE_DIR}/External/glad/src/glad.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image/stb_image.cpp")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/glfw)

add_library(Ogle ${OGLE_SOURCE})
target_include_directories(Ogle PUBLIC ${OGLE_INCLUDE_DIRS})

target_link_libraries(Ogle glfw)