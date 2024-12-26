cmake_minimum_required(VERSION 3.0)
project(Toward_a_Futurustic_Emerald_Isle)

find_package(OpenGL REQUIRED)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glad-opengl-3.3/include/
	external/tinygltf-2.9.3/
	src/
)
#[[
add_executable(lab4_skeleton
	src/lab4_skeleton.cpp
	src/render/shader.cpp
 "src/main.cpp" "src/Application.cpp" "src/Application.h")
target_link_libraries(lab4_skeleton
	${OPENGL_LIBRARY}
	glfw
	glad
)
]]
add_executable(main
	src/main.cpp
	src/render/shader.cpp
 "src/main.cpp" "src/Application.cpp" "src/Application.h"  "src/plane.cpp" "src/loader.h")
target_link_libraries(main
	${OPENGL_LIBRARY}
	glfw
	glad
)