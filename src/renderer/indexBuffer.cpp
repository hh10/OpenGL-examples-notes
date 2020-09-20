#include "indexBuffer.hh"

#include "renderer.hh"

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count)
    : count(count) {
  glGenBuffers(1, &rendererId);
  Bind();
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data,
      GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData, unsigned int almost
                       // always 4 bytes on all platforms so = GLuint
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &rendererId); }

void IndexBuffer::Bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
               rendererId); // specifying to use this buffer
}

void IndexBuffer::Unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // specifying to use this buffer
}
