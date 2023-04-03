cmake_minimum_required(VERSION "3.14.0")

project("StandaloneOpenglTutorials")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.2.1 REQUIRED)

function(make_executable name file)
    message("${ARGC} file args for ${name} exec: ${ARGV}")
	add_executable(${ARGV})
	target_link_libraries(${name} glfw OpenGL::GL ${GLEW_LIBRARIES})
endfunction(make_executable)


make_executable(BasicOpenGLApplication Application.cpp)
make_executable(VertexBuffer VertexBufferNDefaultArray.cpp utils.hh)
make_executable(Shader Shader.cpp)
make_executable(IndexBuffer IndexBuffer.cpp utils.hh)
make_executable(Uniform Uniform.cpp)
make_executable(VertexArray_Compat_DefVA VertexArray.cpp)
make_executable(VertexArray_Core_XVA VertexArray.cpp)
target_compile_definitions(VertexArray_Core_XVA PRIVATE USE_OPENGL_CORE USE_VERTEX_ARRAY)
