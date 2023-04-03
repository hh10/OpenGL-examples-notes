cmake_minimum_required(VERSION "3.14.0")

project("OpenglTutorials")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.2.1 REQUIRED)

add_custom_target(CopyShaderFiles ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources
    ${PROJECT_BINARY_DIR}/resources
    COMMENT "Copying shader files into binary directory"
)

add_subdirectory(standalone_examples)
add_subdirectory(renderer)
