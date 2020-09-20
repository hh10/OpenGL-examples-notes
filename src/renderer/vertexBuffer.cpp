#include "vertexBuffer.hh"

#include "renderer.hh"

VertexBuffer::VertexBuffer(const void *data, unsigned int size) {
  glGenBuffers(1, &rendererId);
  Bind();
  glBufferData(GL_ARRAY_BUFFER, size, data,
               GL_STATIC_DRAW); // http://docs.gl/gl3/glBufferData
}

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &rendererId); }

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, rendererId); // specifying to use this buffer
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0); // specifying to use this buffer
}
