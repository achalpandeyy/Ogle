#include "Texture2D.h"

#include <stb_image.h>
#include <iostream>

namespace Ogle
{
Texture2D::Texture2D(unsigned int width_, unsigned int height_, GLint internal_format, GLenum format, GLenum type,
    GLint min_filter, GLint max_filter, const GLvoid* data) : width(width_), height(height_)
{
    glGenTextures(1, &id);
    Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, max_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data);

    Unbind();
}

Texture2D* Texture2D::CreateFromFile(const char* path, bool flip_vertically)
{
    Texture2D* result = nullptr;

    stbi_set_flip_vertically_on_load(flip_vertically);

    int width, height, channel_count;
    stbi_uc* data = stbi_load(path, &width, &height, &channel_count, 0);

    if (data)
    {
        GLint internal_format(-1);
        GLenum format(-1);

        switch (channel_count)
        {
            case 3:
            {
                internal_format = GL_RGB8;
                format = GL_RGB;
            } break;

            case 4:
            {
                internal_format = GL_RGBA8;
                format = GL_RGBA;
            } break;

            default:
            {
                std::cout << "File format not supported yet!" << std::endl;
            } break;
        }

        result = new Texture2D(width, height, internal_format, format, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST, data);
    }
    else
    {
        std::cout << "Failed to load image at path: " << path << std::endl;
    }

    stbi_image_free(data);

    return result;
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &id);
}
}   // namespace Ogle