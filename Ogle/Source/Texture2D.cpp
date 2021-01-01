#include "Texture2D.h"

Texture2D::Texture2D(unsigned int width, unsigned int height, GLint internal_format, GLenum format, GLenum type,
    GLint min_filter, GLint max_filter, const GLvoid* data)
{
    glGenTextures(1, &id);
    Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, max_filter);

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data);

    Unbind();
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &id);
}