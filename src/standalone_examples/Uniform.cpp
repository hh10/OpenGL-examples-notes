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
        shaderType = ShaderType::VERTEX;
      }
      if (line.find("fragment") != std::string::npos) {
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

  const std::string aim = "Rectangle using 2 triangles (Example 5, following videos till https://youtu.be/DE6Xlx_kbo0)";
  std::cout << aim << std::endl;
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, aim.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  /* It is necessary to create a valid OpenGL rendering context before calling
   * glewInit() to initialize extension entry points (http://glew.sourceforge.net/basic.html) */
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

  unsigned int buffer; // the id that represents the buffer created below
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer); // specifying to use this buffer
  glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), vertexPositions, GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData
  // vertex attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  unsigned int indexBuffer; // the id that represents the buffer created below
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
               indexBuffer); // specifying to use this buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData

  ShaderSources shaderSources = ParseShader("../resources/shaders/BasicShaderWithUniform");
  std::cout << "\n\nSHADER SOURCES:" << std::endl;
  std::cout << "VERTEX:\n" << shaderSources.vertexSource << std::endl;
  std::cout << "FRAGMENT:\n" << shaderSources.fragmentSource << std::endl;
  unsigned int shader = CreateShader(shaderSources.vertexSource, shaderSources.fragmentSource);
  glUseProgram(shader);  // shader is bound at this point

  // initialize the uniform by retrieving it from the shader
  int ulocation = glGetUniformLocation(shader, "u_Color");
  if (ulocation == -1) {
    std::cout << "Uniform not found..!" << std::endl;
    return 1;
  }
  glUniform4f(ulocation, 0.5f, 0.0f, 0.0f, 1.0f);
  float g = 0.0f, incG = 0.05f;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    // the glUniform call is one per draw call; can't have different uniforms for the elements being drawn with a single draw call,
    // unless the uniform is in terms of the vertex data and that has different color attributes assigned to it
    glUniform4f(ulocation, 0.5f, g, 0.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    if (g > 1.0f) {
      incG = -0.05f;
    } else if (g <= 0.0f) {
      incG = 0.05f;
    }
    g += incG;

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
