#include "Cone.h"
#include <cmath>
#include <iostream>

using namespace CS123::GL;

Cone::Cone()
{

}

Cone::Cone(int tesselation, glm::mat4x4 m) :
    Cone(tesselation)
{
    transform(m);
}

Cone::Cone(int tesselation) : Cone(tesselation, tesselation) {}

Cone::Cone(int stacks, int slices)
{
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));

    buildVAO();

    stacks = fmax(1, stacks);
    float h = 0.5;
    m_vertexData.reserve(slices * 6 + 1);
    m_footData.reserve(slices * 6 + 1);
    insertVec3(m_footData, glm::vec3(0, -h, 0));
    insertVec3(m_footData, glm::vec3(0, -1, 0));

    slices = fmax(3, slices);

    for (int i = 0; i < stacks; i++) {
        for (float j = 0; j <= slices; j+=1) {
            float angle_x = (2.0 * M_PI / slices) * j;
            float x = h * sin(angle_x);
            float z = h * cos(angle_x);

            insertVec3(m_vertexData, glm::vec3{x * (i * 1.0 / stacks), h * (1 - (i * 2.0 / stacks)), z * (i * 1.0 / stacks)});
            insertVec3(m_vertexData, glm::normalize(glm::vec3{x, 1 / (2 * h), z}));
            insertVec3(m_vertexData, glm::vec3{x * ((i + 1) * 1.0 / stacks), h * (1 - ((i + 1) * 2.0 / stacks)), z * ((i + 1) * 1.0 / stacks)});
            insertVec3(m_vertexData, glm::normalize(glm::vec3{x, 1 / (2 * h), z}));
            insertVec3(m_footData, glm::vec3{-x, -h, z});
            insertVec3(m_footData, glm::vec3{0, -1, 0});
        }
    }

    refresh();
}

void Cone::refresh() {
    m_vertVBO = std::make_unique<CS123::GL::VBO>(m_vertexData.data(), m_vertexData.size(), m_markers, VBO::LAYOUT_TRIANGLE_STRIP);
    m_footVBO = std::make_unique<CS123::GL::VBO>(m_footData.data(), m_footData.size(), m_markers, VBO::LAYOUT_TRIANGLE_FAN);
}

void Cone::draw() {
    if (m_VAO) {
        m_VAO->setVBO(*m_vertVBO, m_vertexData.size() / 6);
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
        m_VAO->setVBO(*m_footVBO, m_footData.size() / 6);
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}
