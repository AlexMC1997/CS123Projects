#ifndef Torus_H
#define Torus_H

#include "Shape.h"
#include <glm/glm.hpp>
#include "GL/glew.h"

#include<memory>
#include <vector>

namespace CS123 { namespace GL {
class VAO;
}}

class Torus : public Shape
{
public:
    Torus();
    Torus(int tesselation, float rad, glm::mat4x4 m);
    Torus(int stacks, int slices, float rad);
    void draw() override;
    void refresh() override;
private:
    std::vector<GLfloat> m_innerData;
    std::vector<GLfloat> m_outerData;


    std::unique_ptr<CS123::GL::VBO> m_innerVBO;
    std::unique_ptr<CS123::GL::VBO> m_outerVBO;
};

#endif // Torus_H
