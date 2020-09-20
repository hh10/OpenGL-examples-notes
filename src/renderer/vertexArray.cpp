#include "vertexArray.hh"

#include "renderer.hh"

VertexArray::VertexArray() { glGenVertexArrays(1, &rendererId); }

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &rendererId); }

void VertexArray::Bind() const { glBindVertexArray(rendererId); }
void VertexArray::Unbind() const { glBindVertexArray(0); }

void VertexArray::AddBufferLayout(const VertexBuffer &vb,
                                  const VertexBufferLayout &vbl) {
  Bind();
  vb.Bind();
  const auto &vbElements = vbl.GetElements();
  unsigned int offset = 0;
  for (uint32_t i = 0; i < vbElements.size(); ++i) {
    /**
     * index for this attribute
     * size of the vector for each vertex (2 for 2dVertexPosition, 3 for
     * 3dVertexPosition, etc.) whether to be normalized (reqd. for colors)
     * datatype of attribute data
     * stride- how many bytes to shift from the begining of one vertex in the
     * buffer to the begining of the next vertex position (pointer)- what is the
     * byte offset within a single vertex for this attribute
     */
    const auto &element = vbElements[i];
    glEnableVertexAttribArray(
        i); // this vertex array is the default one, unless a vertex array is
            // explicitly created above and the index for that is specified
    glVertexAttribPointer(
        i, element.count, element.type, element.normalize, vbl.GetStride(),
        (const void *)offset); // this call binds the attrib layout to the bound
                               // veretxBuffer which in this case is buffer
    offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
  }
}

// full template specializations, therefore in the cpp file
template <> void VertexBufferLayout::Push<float>(unsigned int count) {
  vbElements.push_back({GL_FLOAT, count, GL_FALSE});
  stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}
template <> void VertexBufferLayout::Push<unsigned int>(unsigned int count) {
  vbElements.push_back({GL_UNSIGNED_INT, count, GL_TRUE});
  stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}
template <> void VertexBufferLayout::Push<unsigned char>(unsigned int count) {
  vbElements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
  stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}
