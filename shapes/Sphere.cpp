#include "Sphere.h"
#include <cmath>
#include <iostream>

using namespace CS123::GL;

Sphere::Sphere() : Sphere(1) {}

Sphere::Sphere(glm::mat4x4 m) : Sphere(1, m) {}

Sphere::Sphere(int tesselation, glm::mat4x4 m) :
    Sphere(tesselation)
{
    transform(m);
}

Sphere::Sphere(int tesselation) : Sphere(tesselation, tesselation) {}

Sphere::Sphere(int stacks, int slices)
{
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_markers.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));

    buildVAO();

    stacks = stacks % 2 ? stacks : stacks - 1;
    slices = fmax(3, slices);
    stacks = fmax(5, stacks);
    m_vertexData.reserve((fmax(3, stacks) - 3) * slices * 3 * 2);

    float y1 = 0.5 - 2 * pow(1.0 / (stacks - 1), 2);
    float y2 = -0.5 + 2 * pow(1.0 / (stacks - 1), 2);

    for (int i = 0; i < stacks - 1; i++) {
        y1 = cos((2.0 * M_PI * i) / (stacks - 1)) / 2;
        y2 = cos((2.0 * M_PI * (i + 1)) / (stacks - 1)) / 2;
        for (float j = i % 2 ? 0.5 : 0; j <= slices + (i % 2 ? 1 : 0); j+=1) {
            float angle_x1 = (2.0 * M_PI / slices) * j;
            float angle_x2 = (2.0 * M_PI / slices) * (j + 0.5);
            float x = sin(angle_x1);
            float z = cos(angle_x1);
            float r = sqrt(0.25 - pow(y1, 2));
            auto nvec = glm::vec3{r*x, y1, r*z};
            insertVec3(m_vertexData, nvec);
            insertVec3(m_vertexData, nvec);
            x = sin(angle_x2);
            z = cos(angle_x2);
            r = sqrt(0.25 - pow(y2, 2));
            nvec = glm::vec3{r*x, y2, r*z};
            insertVec3(m_vertexData, nvec);
            insertVec3(m_vertexData, nvec);
        }
    }

    refresh();
}

void Sphere::refresh() {
    m_VAO->setVBO(VBO(m_vertexData.data(), m_vertexData.size(), m_markers, VBO::LAYOUT_TRIANGLE_STRIP), m_vertexData.size() / 6);
}

void Sphere::draw() {
    if (m_VAO) {
        m_VAO->bind();
        m_VAO->draw();
        m_VAO->unbind();
    }
}
