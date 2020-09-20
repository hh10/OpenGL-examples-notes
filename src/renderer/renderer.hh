#pragma once

#include <GL/glew.h>

#include "indexBuffer.hh"
#include "shader.hh"
#include "vertexArray.hh"

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

void GLClearError();
bool GLLogCall(const char *functionName, const char *filePath, int lineNum);

class Renderer {
public:
  void Clear() const;
  void Draw(const VertexArray &va, const IndexBuffer &ib,
            const Shader &shader) const;
};