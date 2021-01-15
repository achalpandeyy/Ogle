#ifndef TEXTURE_2D_H

#include <glad/glad.h>

namespace Ogle
{
struct Texture2D
{
    Texture2D(unsigned int width_, unsigned int height_, GLint internal_format, GLenum format, GLenum type,
        GLint min_filter = GL_NEAREST, GLint max_filter = GL_NEAREST, const GLvoid* data = 0);

    ~Texture2D();

    inline void Bind(const unsigned int unit = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }
    inline void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    GLuint id = GLuint(-1);
    unsigned int width;
    unsigned int height;
};
}   // namespace Ogle

#define TEXTURE_2D_H
#endif
