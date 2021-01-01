#ifndef TEXTURE_2D_H

#include <glad/glad.h>

struct Texture2D
{
    Texture2D(unsigned int width, unsigned int height, GLint internal_format, GLenum format, GLenum type,
        GLint min_filter = GL_NEAREST, GLint max_filter = GL_NEAREST, const GLvoid* data = 0);

    ~Texture2D();

    inline void Bind() const { glBindTexture(GL_TEXTURE_2D, id); }
    inline void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    GLuint id = GLuint(-1);
};

#define TEXTURE_2D_H
#endif
