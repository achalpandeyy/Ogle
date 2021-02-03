#ifndef SHADER_H

#include <glad/glad.h>
#include <unordered_map>

namespace Ogle
{
struct ShaderStorageBuffer
{
    ShaderStorageBuffer(const GLvoid* data, GLsizeiptr size);

    inline void Bind() const { glBindBuffer(GL_SHADER_STORAGE_BUFFER, id); }
    inline void Unbind() const { glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); }
    inline void BindBase(GLuint binding_index) const { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_index, id); }

private:
    GLuint id;
};

struct Shader
{
    Shader(const char* vertex_path, const char* fragment_path);
    Shader(const char* compute_path);
    Shader(const char* vs_path, const char* tesc_path, const char* tese_path, const char* fs_path);

    ~Shader();

    void Bind() const { glUseProgram(id); }
    void Unbind() const { glUseProgram(0); }

    void SetInt(const char* name, const GLint value);
    void SetUnsignedInt(const char* name, const GLuint value);
    void SetFloat(const char* name, const GLfloat value);
    void SetMat4(const char* name, const GLfloat* value, GLboolean transpose = false);
    void SetVec2(const char* name, const GLfloat x, const GLfloat y);
    void SetVec3(const char* name, const GLfloat x, const GLfloat y, const GLfloat z);

    GLuint id;

private:
    enum class ShaderType
    {
        Vertex,
        Fragment,
        Compute,
        TessellationControl,
        TessellationEvaluation,
        Program
    };

    GLuint CreateShader(const char* path, ShaderType type) const;
    bool CheckShaderErrors(GLuint shader, ShaderType type) const;
    GLint GetUniformLocation(const char* name);

    std::unordered_map<const char*, GLint> uniform_location_cache;
};
}   // namespace Ogle

#define SHADER_H
#endif
