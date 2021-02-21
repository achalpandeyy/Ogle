#ifndef TEXTURE_2D_H

#include <glad/glad.h>

namespace Ogle
{
struct Texture2D
{
    Texture2D(unsigned int width_, unsigned int height_, GLint internal_format_, GLenum format, GLenum type,
        GLint min_filter = GL_NEAREST, GLint max_filter = GL_NEAREST, GLint wrap_r = GL_CLAMP_TO_BORDER,
        GLint wrap_s = GL_CLAMP_TO_BORDER, const GLvoid* data = 0);

    static Texture2D* CreateFromFile(const char* path, bool flip_vertically = false);

    inline void Bind(const unsigned int unit = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    inline void Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    void SetWrappingParams(GLint wrap_r, GLint wrap_s);

    void BindImage(GLuint unit, GLenum access, GLenum format) const;

    ~Texture2D();

    GLuint id = GLuint(-1);
    unsigned int width;
    unsigned int height;
    GLint internal_format;
};
}   // namespace Ogle

#define TEXTURE_2D_H
#endif
