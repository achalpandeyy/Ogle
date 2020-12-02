#ifndef SHADER_H

#include <glad/glad.h>

struct Shader
{
    Shader(const char* vertex_path, const char* fragment_path);

    void Bind() const { glUseProgram(id); }
    void Unbind() const { glUseProgram(0); }

private:
    enum class ShaderType
    {
        Vertex,
        Fragment,
        Program
    };

    GLuint CreateShader(const char* path, ShaderType type) const;
    bool CheckShaderErrors(GLuint shader, ShaderType type) const;

    GLuint id;
};

#define SHADER_H
#endif
