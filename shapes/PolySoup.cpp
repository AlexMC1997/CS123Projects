#include "PolySoup.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include <cstring>

using namespace CS123::GL;

PolySoup::PolySoup() : Shape()
{
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));
    buildVAO();
}

void PolySoup::copyData(std::size_t len, float data[]) {
    buildVAO();
    m_vertexData.resize(len);
    memcpy(m_vertexData.data(), data, len * sizeof(float));
    refresh();
}

void PolySoup::addVert(glm::vec3 vert, glm::vec3 norm) {
    insertVec3(m_vertexData, vert);
    insertVec3(m_vertexData, norm);
}

void PolySoup::refresh() {
    m_VAO->setVBO(VBO(m_vertexData.data(), m_vertexData.size(), m_markers, VBO::LAYOUT_TRIANGLES), m_vertexData.size() / 6);
}

void PolySoup::draw() {
    if (m_VAO) {
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}
