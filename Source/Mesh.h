#ifndef MESH_H

#include <glad/glad.h>

namespace Ogle
{
struct VertexBuffer
{
    VertexBuffer(void* vertices, size_t vertices_size);
    ~VertexBuffer();

    inline void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); }
    inline void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

private:
    GLuint id;
};

struct IndexBuffer
{
    IndexBuffer(void* indices, size_t indices_size);
    ~IndexBuffer();

    inline void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }
    inline void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

private:
    GLuint id;
};

struct VertexAttribs
{
    GLint dims;
    uint64_t offset;
};

struct VertexArray
{
    VertexArray(VertexBuffer* vbo, IndexBuffer* ibo, VertexAttribs* attribs, GLuint attrib_count, GLsizei stride);
    ~VertexArray();

    inline void Bind() const { glBindVertexArray(id); }
    inline void Unbind() const { glBindVertexArray(0); }

private:
    GLuint id;
};

// Todo: Make Mesh use the aforementioned classes, or do we need Mesh at all?
struct Mesh
{
    Mesh(const float* vertices, unsigned int vertex_count, const unsigned int* indices, unsigned int index_count);
    ~Mesh();

    inline void BindVAO() const { glBindVertexArray(vao); }
    inline void UnbindVAO() const { glBindVertexArray(0); }

private:
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
};
}   // namespace Ogle

#define MESH_H
#endif
