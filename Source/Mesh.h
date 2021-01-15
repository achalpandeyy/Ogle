#ifndef MESH_H

#include <glad/glad.h>

namespace Ogle
{
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
