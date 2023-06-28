#include "ExampleShape.h"

using namespace CS123::GL;

ExampleShape::ExampleShape()
{
}

ExampleShape::ExampleShape(glm::mat4x4 m)
{
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));
    buildVAO();
    m_vertexData = CUBE_DATA_POSITIONS;
    transform(m);
    refresh();
}

ExampleShape::ExampleShape(int param1, int param2) :
    m_param1(param1),
    m_param2(param2)
{
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));
    buildVAO();
    m_vertexData = CUBE_DATA_POSITIONS;
    refresh();
}

void ExampleShape::refresh() {
    m_VAO->setVBO(VBO(m_vertexData.data(), m_vertexData.size(), m_markers, VBO::LAYOUT_TRIANGLES), m_vertexData.size() / 6);
}

ExampleShape::~ExampleShape()
{
}
