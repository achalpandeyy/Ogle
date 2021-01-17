#include "Mesh.h"

namespace Ogle
{
VertexBuffer::VertexBuffer(void* vertices, size_t vertices_size)
{
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    Unbind();
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &id);
}

IndexBuffer::IndexBuffer(void* indices, size_t indices_size)
{
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
    Unbind();
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &id);
}

VertexArray::VertexArray(VertexBuffer* vbo, IndexBuffer* ibo, VertexAttribs* attribs, GLuint attrib_count, GLsizei stride)
{
    glGenVertexArrays(1, &id);

    Bind();
    vbo->Bind();
    if (ibo) ibo->Bind();

    for (GLuint i = 0; i < attrib_count; ++i)
    {
        glVertexAttribPointer(i, attribs[i].dims, GL_FLOAT, GL_FALSE, stride, (const GLvoid*)attribs[i].offset);
        glEnableVertexAttribArray(i);
    }

    vbo->Unbind();
    if (ibo) ibo->Unbind();
    Unbind();
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
}

Mesh::Mesh(const float* vertices, unsigned int vertex_count, const unsigned int* indices, unsigned int index_count)
{
    glGenVertexArrays(1, &vao);
    BindVAO();

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    UnbindVAO();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}
}   // namespace Ogle