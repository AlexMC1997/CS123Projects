#include "Texture2D.h"

#include <utility>

namespace CS123 { namespace GL {

Texture2D::Texture2D(unsigned char *data, int width, int height, GLenum type) : Texture()
{
    GLenum internalFormat = type == GL_FLOAT ? GL_RGBA32F : GL_RGBA;

    bind();

    glTexImage2D(m_handle, 0, internalFormat, width, height, 0, GL_RGBA, type, data);

    unbind();
}

Texture2D::Texture2D(unsigned char *data, int width, int height, GLenum type, GLenum format) : Texture()
{
    bind();

    glTexImage2D(m_handle, 0, GL_DEPTH_COMPONENT32, width, height, 0, format, type, data);

    unbind();
}

void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

}}
