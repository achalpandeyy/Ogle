#ifndef SHADER_H

#include <glad/glad.h>

// Todo: You need a uniform cache
struct Shader
{
    Shader(const char* vertex_path, const char* fragment_path);
    Shader(const char* compute_path);

    ~Shader();

    void Bind() const { glUseProgram(id); }
    void Unbind() const { glUseProgram(0); }

    void SetInt(const char* name, const GLint value) const;
    void SetMat4(const char* name, const GLfloat* value, GLboolean transpose) const;
    void SetVec3(const char* name, const GLfloat x, const GLfloat y, const GLfloat z) const;

    GLuint id;

private:
    enum class ShaderType
    {
        Vertex,
        Fragment,
        Compute,
        Program
    };

    GLuint CreateShader(const char* path, ShaderType type) const;
    bool CheckShaderErrors(GLuint shader, ShaderType type) const;
};

#define SHADER_H
#endif
