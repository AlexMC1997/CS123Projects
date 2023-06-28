#ifndef FRAMEBUFFERTEXTUREARRAY_H
#define FRAMEBUFFERTEXTUREARRAY_H

#include "gl/textures/Texture.h"
#include "GL/glew.h"
#include "gl/datatype/FBO.h"

namespace CS123 { namespace GL {


class TextureArray : public Texture
{
public:
    TextureArray(int width, int height, int layers, GLenum type, GLenum format, int mipLevel, unsigned char* data);
    ~TextureArray();
    void bind() const override;
    void unbind() const override;
    void linkFBO(int layer, int mipLevel, FBO& fbo, GLenum attachment);
    void fillArray(unsigned char *data, int layer);
private:
    GLenum m_format;
    GLenum m_type;
    int m_width;
    int m_height;
};

}};

#endif // FRAMEBUFFERTEXTUREARRAY_H
