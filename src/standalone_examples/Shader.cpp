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
  glShaderSource(id, 1, &src, nullptr);  /* args: shader_id, # shader sources, ** to shader src, length of the shader string if not null-terminated */
  glCompileShader(id);

  // Error handling for the compiled shader
  int compilation_result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_result);  /* i in iv => integer, v => vector/array/pointer */
  if (compilation_result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));  /* alloca allows allocating memory dynamically in stack */
    glGetShaderInfoLog(id, length, &length, message);
    std::cout << "failed to compile"
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader"
              << std::endl;
    std::cout << message << std::endl;
    glDeleteShader(id);
    return 0;
  }
  return id;
}

// args are the source codes for the actual shaders, returns a unique id for the common
// shader generated
static unsigned int CreateShader(const std::string &vertexShader,
                                 const std::string &fragmentShader) {
  unsigned int program = glCreateProgram();  /* return the handle of the program getting created */
  unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
  unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);
  // todo: add a check for 0 returns

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);
  // glDetachShader  /* should be called after linking, but debugging may become harder as a result */
  /* delete the intermediate shader objects since they have been stored in the program */
  glDeleteShader(vs);
  glDeleteShader(fs);
  return program;
}

int main(void) {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  const std::string aim = "Simple Triangle (Example 3, following videos till https://youtu.be/71BLZwRGUJE)";
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
   * glewInit() to initialize extension entry points (http://glew.sourceforge.net/basic.html) */
  if (glewInit() != GLEW_OK) {
    std::cout << "Glew couldn't be initialized" << std::endl;
  }
  std::cout << glGetString(GL_VERSION) << std::endl;

  float vertexPositions[6] = {-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f};
  unsigned int buffer; // the id that represents the buffer created below
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer); // specifying to use this buffer
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexPositions, GL_STATIC_DRAW);

  // vertex attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  ShaderSources shaderSources = ParseShader("../resources/shaders/BasicShader");
  std::cout << "\n\nSHADER SOURCES:" << std::endl;
  std::cout << "VERTEX:\n" << shaderSources.vertexSource << std::endl;
  std::cout << "FRAGMENT:\n" << shaderSources.fragmentSource << std::endl;

  unsigned int shader = CreateShader(shaderSources.vertexSource, shaderSources.fragmentSource);
  // bind the shader
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
