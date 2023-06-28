#include "Cylinder.h"
#include <cmath>
#include <iostream>

using namespace CS123::GL;

Cylinder::Cylinder()
{

}

Cylinder::Cylinder(int tesselation, glm::mat4x4 m) :
    Cylinder(0, tesselation)
{
    transform(m);
}

Cylinder::Cylinder(int tesselation) : Cylinder(tesselation, tesselation) {}

Cylinder::Cylinder(int stacks, int slices) : Cylinder()
{
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));

    buildVAO();

    stacks = fmax(1, stacks);
    float h = 0.5 + (0.02 * stacks);
    insertVec3(m_capData, glm::vec3(0, h, 0));
    insertVec3(m_capData, glm::vec3(0, 1, 0));
    insertVec3(m_footData, glm::vec3(0, -h, 0));
    insertVec3(m_footData, glm::vec3(0, -1, 0));

    slices = fmax(3, slices);
    m_vertexData.reserve(slices * 12);
    m_capData.reserve(slices * 6);
    m_footData.reserve(slices * 6);

    for (float j = 0; j <= slices; j+=1) {
        float angle_x = (2.0 * M_PI / slices) * j;
        float x = sin(angle_x) / 2;
        float z = cos(angle_x) / 2;

        insertVec3(m_vertexData, glm::vec3{x, h, z});
        insertVec3(m_vertexData, glm::normalize(glm::vec3{x, 0, z}));
        insertVec3(m_capData, glm::vec3{x, h, z});
        insertVec3(m_capData, glm::vec3{0, 1, 0});


        insertVec3(m_vertexData, glm::vec3{x, -h, z});
        insertVec3(m_vertexData, glm::normalize(glm::vec3{x, 0, z}));
        insertVec3(m_footData, glm::vec3{-x, -h, z});
        insertVec3(m_footData, glm::vec3{0, -1, 0});
    }

    refresh();
}

void Cylinder::refresh() {
    m_vertVBO = std::make_unique<CS123::GL::VBO>(m_vertexData.data(), m_vertexData.size(), m_markers, VBO::LAYOUT_TRIANGLE_STRIP);
    m_capVBO = std::make_unique<CS123::GL::VBO>(m_capData.data(), m_capData.size(), m_markers, VBO::LAYOUT_TRIANGLE_FAN);
    m_footVBO = std::make_unique<CS123::GL::VBO>(m_footData.data(), m_footData.size(), m_markers, VBO::LAYOUT_TRIANGLE_FAN);
}

void Cylinder::draw() {
    if (m_VAO) {
        m_VAO->setVBO(*m_vertVBO, m_vertexData.size() / 6);
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
        m_VAO->setVBO(*m_capVBO, m_capData.size() / 6);
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
        m_VAO->setVBO(*m_footVBO, m_footData.size() / 6);
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}
