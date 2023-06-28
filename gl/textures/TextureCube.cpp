#include "TextureCube.h"

CS123::GL::TextureCube::TextureCube() {

}

CS123::GL::TextureCube::TextureCube(
        uchar posx[],
        uchar negx[],
        uchar posy[],
        uchar negy[],
        uchar posz[],
        uchar negz[],
        int width, int height, GLenum type
) : Texture()
{
    GLenum internalFormat = type == GL_FLOAT ? GL_RGBA32F : GL_RGBA;
    bind();

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, width, height, 0, internalFormat, type, posx);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, width, height, 0, internalFormat, type, negx);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, width, height, 0, internalFormat, type, posy);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, width, height, 0, internalFormat, type, negy);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, width, height, 0, internalFormat, type, posz);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, width, height, 0, internalFormat, type, negz);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    unbind();
}

void CS123::GL::TextureCube::bind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);
}

void CS123::GL::TextureCube::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
