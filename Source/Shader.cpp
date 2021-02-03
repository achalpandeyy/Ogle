#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace Ogle
{
ShaderStorageBuffer::ShaderStorageBuffer(const GLvoid* data, GLsizeiptr size)
{
    glGenBuffers(1, &id);

    Bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
    Unbind();
}

Shader::Shader(const char* vertex_path, const char* fragment_path)
{
    GLuint vertex_shader = CreateShader(vertex_path, ShaderType::Vertex);
    GLuint fragment_shader = CreateShader(fragment_path, ShaderType::Fragment);

    id = glCreateProgram();
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    if (CheckShaderErrors(id, ShaderType::Program)) exit(1);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::Shader(const char* compute_path)
{
    GLuint compute_shader = CreateShader(compute_path, ShaderType::Compute);

    id = glCreateProgram();
    glAttachShader(id, compute_shader);
    glLinkProgram(id);

    if (CheckShaderErrors(id, ShaderType::Program)) exit(1);

    glDeleteShader(compute_shader);
}

Shader::Shader(const char* vs_path, const char* tesc_path, const char* tese_path, const char* fs_path)
{
    GLuint vs = CreateShader(vs_path, ShaderType::Vertex);
    GLuint tcs = CreateShader(tesc_path, ShaderType::TessellationControl);
    GLuint tes = CreateShader(tese_path, ShaderType::TessellationEvaluation);
    GLuint fs = CreateShader(fs_path, ShaderType::Fragment);

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, tcs);
    glAttachShader(id, tes);
    glAttachShader(id, fs);
    glLinkProgram(id);

    if (CheckShaderErrors(id, ShaderType::Program)) exit(1);

    glDeleteShader(vs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::SetInt(const char* name, const GLint value)
{
    GLint location = GetUniformLocation(name);
    glUniform1i(location, value);
}

void Shader::SetUnsignedInt(const char* name, const GLuint value)
{
    GLint location = GetUniformLocation(name);
    glUniform1ui(location, value);
}

void Shader::SetFloat(const char* name, const GLfloat value)
{
    GLint location = GetUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::SetMat4(const char* name, const GLfloat* value, GLboolean transpose)
{
    GLint location = GetUniformLocation(name);
    glUniformMatrix4fv(location, 1, transpose, value);
}

void Shader::SetVec2(const char* name, const GLfloat x, const GLfloat y)
{
    GLint location = GetUniformLocation(name);
    glUniform2f(location, x, y);
}

void Shader::SetVec3(const char* name, const GLfloat x, const GLfloat y, const GLfloat z)
{
    GLint location = GetUniformLocation(name);
    glUniform3f(location, x, y, z);
}

GLuint Shader::CreateShader(const char* path, ShaderType type) const
{
    std::ifstream file(path);
    if (file)
    {
        std::stringstream shader_stream;
        shader_stream << file.rdbuf();
        file.close();

        const std::string& shader_source = shader_stream.str();
        const char* shader_source_c_str = shader_source.c_str();

        GLenum shader_type = GL_VERTEX_SHADER;
        switch (type)
        {
            case ShaderType::Fragment:
                shader_type = GL_FRAGMENT_SHADER;
                break;
            case ShaderType::Compute:
                shader_type = GL_COMPUTE_SHADER;
                break;
            case ShaderType::TessellationControl:
                shader_type = GL_TESS_CONTROL_SHADER;
                break;
            case ShaderType::TessellationEvaluation:
                shader_type = GL_TESS_EVALUATION_SHADER;
                break;
        }

        GLuint shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &shader_source_c_str, 0);
        glCompileShader(shader);

        if (CheckShaderErrors(shader, type)) return GLuint(-1);

        return shader;
    }

    return GLuint(-1);
}

bool Shader::CheckShaderErrors(GLuint shader, ShaderType type) const
{
    GLint success;
    if (type == ShaderType::Program)
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar info_log[512];
            glGetProgramInfoLog(shader, 512, nullptr, info_log);

            std::cout << "Failed to link shader program.\n" << info_log << std::endl;
            return true;
        }
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar info_log[512];
            glGetShaderInfoLog(shader, 512, nullptr, info_log);

            std::cout << info_log << std::endl;

            return true;
        }
    }

    return false;
}

GLint Shader::GetUniformLocation(const char* name)
{
    if (uniform_location_cache.find(name) != uniform_location_cache.end())
        return uniform_location_cache[name];

    GLint location = glGetUniformLocation(id, name);
    if (location == -1)
        std::cout << "Warning: " << name << " uniform doesn't exist!" << std::endl;

    uniform_location_cache[name] = location;
    return location;
}
}   // namespace Ogle