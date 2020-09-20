#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>

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

  unsigned int buffer; // the id that represents the buffer created below
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer); // specifying to use this buffer
  glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), vertexPositions,
               GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData

// explicit vertex array creation
#ifdef USE_VERTEX_ARRAY
  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
#endif

  /**
   * index for this attribute
   * size of the vector for each vertex (2 for 2dVertexPosition, 3 for
   * 3dVertexPosition, etc.) whether to be normalized (reqd. for colors)
   * datatype of attribute data
   * stride- how many bytes to shift from the begining of one vertex in the
   * buffer to the begining of the next vertex position (pointer)- what is the
   * byte offset within a single vertex for this attribute
   */
  GLCall(glEnableVertexAttribArray(
      0)); // thsi vertex array is the default one, unless a vertex array is
           // explicitly created above and the index for that is specified
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        0); // this call binds the attrib layout to the bound
                            // veretxBuffer which in this case is buffer

  unsigned int indexBuffer; // the id that represents the buffer created below
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
               indexBuffer); // specifying to use this buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices,
               GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData

  ShaderSources shaderSources =
      ParseShader("../src/resources/shaders/BasicShaderWithUniform");
  /*std::cout << "VERTEX" << std::endl;
  std::cout << shaderSources.vertexSource << std::endl;
  std::cout << "FRAGMENT" << std::endl;
  std::cout << shaderSources.fragmentSource << std::endl;*/

  unsigned int shader =
      CreateShader(shaderSources.vertexSource, shaderSources.fragmentSource);
  glUseProgram(shader);

  int ulocation = glGetUniformLocation(shader, "u_Color");
  if (ulocation == -1) {
    std::cout << "Uniform not found..!" << std::endl;
    return 1;
  }

  // unbind everything
  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glUniform4f(ulocation, 0.5f, 0.0f, 0.0f, 1.0f);
  float g = 0.0f;
  float incG = 0.05f;
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    // bind all required states before draw call
    glUseProgram(shader);
#ifdef USE_VERTEX_ARRAY
    glBindVertexArray(vao);
#else
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
#endif
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    // the glUniform call is one per draw call; can't have different uniforms
    // for the elements being drawn with a single draw call, unless the uniform
    // is in terms of the vertex data and that has different color attributes
    // assigned to it
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
