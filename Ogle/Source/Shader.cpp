#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

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

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::SetMat4(const char* name, const GLfloat* value, GLboolean transpose) const
{
    GLint location = glGetUniformLocation(id, name);
    glUniformMatrix4fv(location, 1, transpose, value);
}

void Shader::SetVec3(const char* name, const GLfloat x, const GLfloat y, const GLfloat z) const
{
    GLint location = glGetUniformLocation(id, name);
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

            std::string shader_type = "vertex";
            switch (type)
            {
            case ShaderType::Fragment:
                shader_type = "fragment";
                break;
            case ShaderType::Compute:
                shader_type = "compute";
                break;
            }

            std::cout << "Failed to compile " << shader_type << " shader.\n"
                << info_log << std::endl;

            return true;
        }
    }

    return false;
}
