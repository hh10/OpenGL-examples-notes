#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>

struct ShaderSources {
  const std::string vertexSource;
  const std::string fragmentSource;
};

static ShaderSources ParseShader(const std::string &filePath) {
  std::ifstream stream(filePath);
  std::string line;

  enum ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
  ShaderType shaderType = ShaderType::NONE;

  std::stringstream ss[2];
  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        // its a vertex shader
        shaderType = ShaderType::VERTEX;
      }
      if (line.find("fragment") != std::string::npos) {
        // its a fragment shader
        shaderType = ShaderType::FRAGMENT;
      }
    } else {
      ss[shaderType] << line << "\n";
    }
  }
  return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(const std::string &source,
                                  unsigned int type) {
  unsigned int id = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cout << "failed to compile"
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragement") << "shader"
              << std::endl;
    std::cout << message << std::endl;
    glDeleteShader(id);
    return 0;
  }
  return id;
}

// args are the source codes for the shaders, returns a unique id for the common
// shader generated
static unsigned int CreateShader(const std::string &vertexShader,
                                 const std::string &fragmentShader) {
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
  unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);
  // glDetachShader
  glDeleteShader(vs);
  glDeleteShader(fs);
  return program;
}

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

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* It is necessary to create a valid OpenGL rendering context before calling
   * glewInit() to initialize extension entry points
   * (http://glew.sourceforge.net/basic.html) */
  if (glewInit() != GLEW_OK) {
    std::cout << "Glew couldn't be initialized" << std::endl;
  }
  std::cout << glGetString(GL_VERSION) << std::endl;

  float vertexPositions[6] = {-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f};
  unsigned int buffer; // the id that represents the buffer created below
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer); // specifying to use this buffer
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexPositions,
               GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData

  /**
   * index for this attribute
   * size of the vector for each vertex (2 for 2dVertexPosition, 3 for
   * 3dVertexPosition, etc.) whether to be normalized (reqd. for colors)
   * datatype of attribute data
   * stride- how many bytes to shift from the begining of one vertex in the
   * buffer to the begining of the next vertex position (pointer)- what is the
   * byte offset within a single vertex for this attribute
   */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  ShaderSources shaderSources =
      ParseShader("../src/resources/shaders/BasicShader");
  /*std::cout << "VERTEX" << std::endl;
  std::cout << shaderSources.vertexSource << std::endl;
  std::cout << "FRAGMENT" << std::endl;
  std::cout << shaderSources.fragmentSource << std::endl;
  */
  unsigned int shader =
      CreateShader(shaderSources.vertexSource, shaderSources.fragmentSource);
  glUseProgram(shader);

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
  // clean up
  glDeleteProgram(shader);

  glfwTerminate();
  return 0;
}
