#pragma once

#include <GL/glew.h>
#include <vector>

struct VertexBufferElement {
  unsigned int type;
  unsigned int count;
  unsigned char normalize;

  static unsigned int GetSizeOfType(unsigned int type) {
    switch (type) {
    case GL_FLOAT:
      return 4;
    case GL_UNSIGNED_INT:
      return 4;
    case GL_UNSIGNED_BYTE:
      return 1;
    }
    return 0;
  }
};

class VertexBufferLayout {
private:
  std::vector<VertexBufferElement> vbElements;
  unsigned int stride;

public:
  VertexBufferLayout() : stride{0} {}

  template <typename T> void Push(unsigned int count);

  inline const std::vector<VertexBufferElement> &GetElements() const {
    return vbElements;
  }
  inline unsigned int GetStride() const { return stride; }
};
