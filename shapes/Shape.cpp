#include "Shape.h"

using namespace CS123::GL;

Shape::Shape() :
    m_VAO(nullptr)
{

}

Shape::~Shape()
{
}

void Shape::draw() {
    if (m_VAO) {
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}

void Shape::buildVAO() {
    m_VAO = std::make_unique<VAO>();
}

void Shape::transform(glm::mat4x4 m) {
    m_world = m;
}

glm::mat4x4 Shape::world() {
    return m_world;
}

void Shape::refresh() {
    m_VAO->setVBO(VBO(m_vertexData.data(), m_vertexData.size(), m_markers, VBO::LAYOUT_TRIANGLE_STRIP), m_vertexData.size() / 6);
}

CS123SceneMaterial Shape::getMaterial() {
    return m_material;
}

void Shape::setMaterial(CS123SceneMaterial mat) {
    m_material = mat;
}
