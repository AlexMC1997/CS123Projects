#include "Torus.h"
#include "glm/gtx/transform.hpp"
#include <cmath>
#include <iostream>

using namespace CS123::GL;

Torus::Torus()
{

}

Torus::Torus(int tesselation, float rad, glm::mat4x4 m) :
    Torus(tesselation, tesselation, rad)
{
    transform(m);
}

Torus::Torus(int stacks, int slices, float rad)
{
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));

    rad *= 0.5;
    m_outerData = std::vector<GLfloat>();
    buildVAO();

    stacks = stacks % 2 ? stacks : stacks - 1;
    slices = fmax(3, slices);
    stacks = fmax(5, stacks);
    m_outerData.reserve((fmax(3, stacks) - 3) * slices * 3 * 2);

    float y1 = 0.5 - 2 * pow(1.0 / (stacks - 1), 2);
    float y2 = -0.5 + 2 * pow(1.0 / (stacks - 1), 2);


    for (int i = 0; i < stacks - 1; i++) {
        y1 = cos((2.0 * M_PI * i) / (stacks - 1)) / 2;
        y2 = cos((2.0 * M_PI * (i + 1)) / (stacks - 1)) / 2;
        for (float j = 0; j <= slices; j+=1) {
            float angle_x = (2.0 * M_PI / slices) * j;
            float x = sin(angle_x);
            float z = cos(angle_x);
            float r = sqrt(0.25 - pow(y1, 2));
            auto d = glm::vec3{rad * x, 0, rad * z};
            auto nvec = glm::vec3{r*x, y1, r*z};
            insertVec3(m_outerData, nvec + d);
            insertVec3(m_outerData, nvec);
            nvec.x = -nvec.x;
            nvec.z = -nvec.z;
            insertVec3(m_innerData, nvec + d);
            insertVec3(m_innerData, nvec);
            r = sqrt(0.25 - pow(y2, 2));
            nvec = glm::vec3{r*x, y2, r*z};
            insertVec3(m_outerData, nvec + d);
            insertVec3(m_outerData, nvec);
            nvec.x = -nvec.x;
            nvec.z = -nvec.z;
            insertVec3(m_innerData, nvec + d);
            insertVec3(m_innerData, nvec);
        }
    }

    refresh();
}

void Torus::refresh() {
    m_innerVBO = std::make_unique<CS123::GL::VBO>(m_innerData.data(), m_innerData.size(), m_markers, VBO::LAYOUT_TRIANGLE_STRIP);
    m_outerVBO = std::make_unique<CS123::GL::VBO>(m_outerData.data(), m_outerData.size(), m_markers, VBO::LAYOUT_TRIANGLE_STRIP);
}

void Torus::draw() {
    if (m_VAO) {
        m_VAO->setVBO(*m_outerVBO, m_outerData.size() / 6);
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
        m_VAO->setVBO(*m_innerVBO, m_innerData.size() / 6);
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}
