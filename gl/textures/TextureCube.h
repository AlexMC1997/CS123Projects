#ifndef TEXTURECUBE_H
#define TEXTURECUBE_H

#include "GL/gl.h"
#include "gl/textures/Texture.h"
#include <vector>
#include <iostream>
#include "qimage.h"

namespace CS123 { namespace GL {

class TextureCube : public Texture
{
public:
    TextureCube();
    TextureCube(uchar posx[], uchar negx[], uchar posy[], uchar negy[], uchar posz[], uchar negz[], int width, int height, GLenum type);

    virtual void bind() const override;
    virtual void unbind() const override;
};

}}

#endif // TEXTURECUBE_H
