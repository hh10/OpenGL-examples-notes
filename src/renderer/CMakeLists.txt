cmake_minimum_required(VERSION "3.14.0")

project("OpenglTutorials")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.2.1 REQUIRED)

file(GLOB SOURCES "*.cpp")
add_executable(renderer ${SOURCES})
target_link_libraries(renderer glfw OpenGL::GL ${GLEW_LIBRARIES})
