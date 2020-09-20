cmake_minimum_required(VERSION "3.14.0")

project("OpenglTutorials")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.2.1 REQUIRED)

add_subdirectory(standalone_examples)
add_subdirectory(renderer)
