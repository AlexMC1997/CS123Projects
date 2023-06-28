#include "Skybox.h"
#include "Imgutil.h"
#include <vector>

Skybox::Skybox(const QStringList& files)
{
    std::vector<std::unique_ptr<QImage>> imgs(6);

    for (int i = 0; i < 6; i++)
        imgs[i] = std::make_unique<QImage>(files[i]);

    m_texCube = std::make_unique<CS123::GL::TextureCube>(
                imgs[0]->bits(),
                imgs[1]->bits(),
                imgs[2]->bits(),
                imgs[3]->bits(),
                imgs[4]->bits(),
                imgs[5]->bits(),
                imgs[0]->width(),
                imgs[0]->height(),
                GL_UNSIGNED_BYTE
                );


    m_box = std::make_unique<PolySoup>();
    std::vector<float> skyVerts = {
        // positions
        -1.0f,  1.0f, -1.0f,
            0,     0,     0,
        -1.0f, -1.0f, -1.0f,
            0,     0,     0,
         1.0f, -1.0f, -1.0f,
            0,     0,     0,
         1.0f, -1.0f, -1.0f,
            0,     0,     0,
         1.0f,  1.0f, -1.0f,
            0,     0,     0,
        -1.0f,  1.0f, -1.0f,
            0,     0,     0,

        -1.0f, -1.0f,  1.0f,
            0,     0,     0,
        -1.0f, -1.0f, -1.0f,
            0,     0,     0,
        -1.0f,  1.0f, -1.0f,
            0,     0,     0,
        -1.0f,  1.0f, -1.0f,
            0,     0,     0,
        -1.0f,  1.0f,  1.0f,
            0,     0,     0,
        -1.0f, -1.0f,  1.0f,
            0,     0,     0,

         1.0f, -1.0f, -1.0f,
            0,     0,     0,
         1.0f, -1.0f,  1.0f,
            0,     0,     0,
         1.0f,  1.0f,  1.0f,
            0,     0,     0,
         1.0f,  1.0f,  1.0f,
            0,     0,     0,
         1.0f,  1.0f, -1.0f,
            0,     0,     0,
         1.0f, -1.0f, -1.0f,
            0,     0,     0,

        -1.0f, -1.0f,  1.0f,
            0,     0,     0,
        -1.0f,  1.0f,  1.0f,
            0,     0,     0,
         1.0f,  1.0f,  1.0f,
            0,     0,     0,
         1.0f,  1.0f,  1.0f,
            0,     0,     0,
         1.0f, -1.0f,  1.0f,
            0,     0,     0,
        -1.0f, -1.0f,  1.0f,
            0,     0,     0,

        -1.0f,  1.0f, -1.0f,
            0,     0,     0,
         1.0f,  1.0f, -1.0f,
            0,     0,     0,
         1.0f,  1.0f,  1.0f,
            0,     0,     0,
         1.0f,  1.0f,  1.0f,
            0,     0,     0,
        -1.0f,  1.0f,  1.0f,
            0,     0,     0,
        -1.0f,  1.0f, -1.0f,
            0,     0,     0,

        -1.0f, -1.0f, -1.0f,
            0,     0,     0,
        -1.0f, -1.0f,  1.0f,
            0,     0,     0,
         1.0f, -1.0f, -1.0f,
            0,     0,     0,
         1.0f, -1.0f, -1.0f,
            0,     0,     0,
        -1.0f, -1.0f,  1.0f,
            0,     0,     0,
         1.0f, -1.0f,  1.0f,
            0,     0,     0
    };

    m_box->copyData(skyVerts.size(), skyVerts.data());
}

CS123::GL::TextureCube& Skybox::getTexture() {
    return *m_texCube;
}

void Skybox::draw() {
    m_box->draw();
}
