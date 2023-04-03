#pragma once
// code as described in video https://youtu.be/FBbPWSOQ0-w

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
