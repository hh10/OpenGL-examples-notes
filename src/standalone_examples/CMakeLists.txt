cmake_minimum_required(VERSION "3.14.0")

project("StandaloneOpenglTutorials")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.2.1 REQUIRED)

function(make_executable name file)
	add_executable(${name} ${file})
	target_link_libraries(${name} glfw OpenGL::GL ${GLEW_LIBRARIES})
endfunction(make_executable)

make_executable(Application Application.cpp)
make_executable(VB VertexBuffer.cpp)
make_executable(Shader Shader.cpp)
make_executable(IB IndexBuffer.cpp)
make_executable(Uniform Uniform.cpp)
make_executable(VertexArray VertexArray.cpp)
target_compile_definitions(VertexArray PRIVATE USE_OPENGL_CORE=1 USE_VERTEX_ARRAY=1)
