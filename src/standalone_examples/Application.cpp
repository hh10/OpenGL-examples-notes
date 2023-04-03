#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define ASSERT(x)                                                              \
  if (!(x))                                                                    \
    asm("0:"                                                                   \
        ".pushsection embed-breakpoints;"                                      \
        ".quad 0b;"                                                            \
        ".popsection;");
#define GLCall(x)                                                              \
  GLClearError();                                                              \
  x;                                                                           \
  ASSERT(GLLogCall(#x, __FILE__, __LINE__));

// preferably use anonymous namespace than static functions in general (teh
// former allowing translation unit local declarations of user defined types and
// preventing violation of One definition rule for objects and non-inline
// functions)
static void GLClearError() {
  while (glGetError() != GL_NO_ERROR)
    ;
}

static bool GLLogCall(const char *functionName, const char *filePath,
                      int lineNum) {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL error] (" << error << "): " << functionName << " "
              << filePath << " " << lineNum << std::endl;
    return false;
  }
  return true;
}

int main(void) {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  const std::string aim = "Simple Triangle (Example 1, following videos till https://youtu.be/H2E3yO0J7TM)";
  std::cout << aim << std::endl;
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, aim.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* It is necessary to create a valid OpenGL rendering context before calling
   * glewInit() to initialize extension entry points
   * (http://glew.sourceforge.net/basic.html) */
  GLCall(glewInit());
  if (glewInit() != GLEW_OK) {
    std::cout << "Glew couldn't be initialized" << std::endl;
  }
  std::cout << glGetString(GL_VERSION) << std::endl;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
