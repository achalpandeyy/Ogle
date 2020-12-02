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

        GLuint shader = glCreateShader(type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
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

            std::cout << "Failed to compile " << (type == ShaderType::Vertex ? "vertex" : "fragment") << " shader.\n"
                << info_log << std::endl;

            return true;
        }
    }

    return false;
}

