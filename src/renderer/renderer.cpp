#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "renderer.hh"
#include "vertexBuffer.hh"

void GLClearError() {
  while (glGetError() != GL_NO_ERROR)
    ;
}

bool GLLogCall(const char *functionName, const char *filePath, int lineNum) {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL error] (" << error << "): " << functionName << " "
              << filePath << " " << lineNum << std::endl;
    return false;
  }
  return true;
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib,
                    const Shader &shader) const {
  shader.Bind();
  va.Bind();
  ib.Bind();
  glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Clear() const { glClear(GL_COLOR_BUFFER_BIT); }

int main(void) {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Simple Traingle", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef USE_OPENGL_CORE
  std::cout << "core" << std::endl;
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
  std::cout << "compat" << std::endl;
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  glfwSwapInterval(1);

  /* It is necessary to create a valid OpenGL rendering context before calling
   * glewInit() to initialize extension entry points
   * (http://glew.sourceforge.net/basic.html) */
  if (glewInit() != GLEW_OK) {
    std::cout << "Glew couldn't be initialized" << std::endl;
  }
  std::cout << glGetString(GL_VERSION) << std::endl;

  // drawing rectangle without index buffer
  // float vertexPositions[12] = {-0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f,
  // -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
  float vertexPositions[8] = {-0.5f, -0.5f, 0.5f,  0.5f,
                              0.5f,  -0.5f, -0.5f, 0.5f};
  unsigned int indices[] = {0, 1, 2, 0, 1, 3};

  VertexBuffer vb(vertexPositions, 4 * 2 * sizeof(float));

  // explicit vertex array creation
  VertexArray vao;
  VertexBufferLayout vbl;
  vbl.Push<float>(2);
  vao.AddBufferLayout(vb, vbl);
  IndexBuffer ib(indices, 6);

  Shader shader("../src/resources/shaders/BasicShaderWithUniform");
  shader.Bind();
  float color_rgba[4] = {0.5f, 0.0f, 0.0f, 1.0f};
  shader.SetUniform4f("u_Color", color_rgba);

  // unbind everything
  vao.Unbind();
  shader.Unbind();
  vb.Unbind();
  ib.Unbind();

  Renderer renderer;
  float incG = 0.05f;
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    renderer.Clear();

    // bind all required states before draw call
    shader.Bind();
    // the glUniform call is one per draw call; can't have different uniforms
    // for the elements being drawn with a single draw call, unless the uniform
    // is in terms of the vertex data and that has different color attributes
    // assigned to it
    shader.SetUniform4f("u_Color", color_rgba);
    renderer.Draw(vao, ib, shader);

    if (color_rgba[1] > 1.0f) {
      incG = -0.05f;
    } else if (color_rgba[1] <= 0.0f) {
      incG = 0.05f;
    }
    color_rgba[1] += incG;

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
