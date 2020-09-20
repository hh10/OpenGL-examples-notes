#pragma once

#include "vertexBuffer.hh"
#include "vertexBufferLayout.hh"

class VertexArray {
private:
  unsigned int rendererId;

public:
  VertexArray();
  ~VertexArray();

  void AddBufferLayout(const VertexBuffer &vb, const VertexBufferLayout &vbl);

  void Bind() const;
  void Unbind() const;
};