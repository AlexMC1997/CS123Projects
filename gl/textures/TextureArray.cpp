#include "TextureArray.h"
#include "gl/GLDebug.h"

using namespace CS123::GL;

TextureArray::TextureArray(int width, int height, int layers, GLenum type, GLenum format, int mipLevel, unsigned char* data) :
    m_format(format),
    m_type(type),
    m_width(width),
    m_height(height)
{
    glGenTextures(1, &m_handle);
    bind();
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, layers, 0, format, type, data);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, mipLevel);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    unbind();
}

TextureArray::~TextureArray() {
    glDeleteTextures(1, &m_handle);
}

//should change to generalize
void TextureArray::fillArray(unsigned char *data, int depth) {
    bind();
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, m_width, m_height, depth, m_type, GL_UNSIGNED_BYTE, data);
    unbind();
}

void TextureArray::linkFBO(int layer, int mipLevel, FBO& fbo, GLenum attachment) {
    fbo.bind();
    bind();
    glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, m_handle, mipLevel, layer);
    unbind();
    fbo.status();
    fbo.unbind();
}

void TextureArray::bind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle);
}

void TextureArray::unbind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
