#ifndef SKYBOX_H
#define SKYBOX_H

#include "PolySoup.h"
#include "qstringlist.h"
#include "qimage.h"
#include "gl/textures/TextureCube.h"
#include <memory>

class Skybox
{
public:
    Skybox();
    Skybox(const QStringList& files);
    CS123::GL::TextureCube& getTexture();
    void draw();
private:
    std::unique_ptr<CS123::GL::TextureCube> m_texCube;
    std::unique_ptr<CS123::GL::TextureCube> m_texR2;
    std::unique_ptr<CS123::GL::TextureCube> m_texR4;
    std::unique_ptr<CS123::GL::TextureCube> m_texR6;
    std::unique_ptr<CS123::GL::TextureCube> m_texR8;
    std::unique_ptr<CS123::GL::TextureCube> m_texR10;
    std::unique_ptr<PolySoup> m_box;
};

#endif // SKYBOX_H
