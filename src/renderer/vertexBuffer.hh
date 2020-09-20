#pragma once

class VertexBuffer {
private:
  // unique id of the vertex buffer object for the renderer to refer to it
  unsigned int rendererId;

public:
  VertexBuffer(const void *data, unsigned int size);
  ~VertexBuffer();

  void Bind() const;
  void Unbind() const;
};