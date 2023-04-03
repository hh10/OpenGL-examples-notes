#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "utils.hh"

int main(void) {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1; 
 
  const std::string aim = "Simple Triangle (Example 2, following videos till https://youtu.be/x0H--CL2tUI)";
  std::cout << aim << std::endl;
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, aim.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* It is necessary to create a valid OpenGL rendering context before calling glewInit() to initialize extension entry points (http://glew.sourceforge.net/basic.html) */
  if (glewInit() != GLEW_OK) {
          std::cout << "Glew couldn't be initialized" << std::endl;
  }
  std::cout << glGetString(GL_VERSION) << std::endl;

  float vertexPositions[6] = {-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f};
  unsigned int buffer; // the id that represents the buffer created below 
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer); // specifying to use this buffer
  glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), vertexPositions, GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData

  GLCall(glEnableVertexAttribArray(0)); // this vertex array is the default one, unless a vertex array is
           // explicitly created above and the index for that is specified
  /**
   * index for this attribute
   * size of the vector for each vertex (2 for 2dVertexPosition, 3 for 3dVertexPosition, etc.)
   * whether to be normalized (reqd. for colors)
   * datatype of attribute data
   * stride- how many bytes to shift from the begining of one vertex in the buffer to the begining of the next vertex
   * position (pointer)- what is the byte offset within a single vertex for this attribute
   */
  // Covered in Video: https://youtu.be/x0H--CL2tUI?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0); // this call binds the attrib layout to the bound
                            // vertexBuffer which in this case is buffer in L32 above

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /*glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();*/
    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
