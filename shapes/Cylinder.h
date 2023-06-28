#ifndef Cylinder_H
#define Cylinder_H

#include "Shape.h"
#include <glm/glm.hpp>
#include "GL/glew.h"

#include<memory>
#include <vector>

namespace CS123 { namespace GL {
class VAO;
}}

class Cylinder : public Shape
{
public:
    Cylinder();
    Cylinder(int tesselation, glm::mat4x4 m);
    Cylinder(int tesselation);
    Cylinder(int stacks, int slices);
    void refresh() override;
    void draw();
private:
    std::vector<GLfloat> m_capData;
    std::vector<GLfloat> m_footData;

    std::unique_ptr<CS123::GL::VBO> m_capVBO;
    std::unique_ptr<CS123::GL::VBO> m_vertVBO;
    std::unique_ptr<CS123::GL::VBO> m_footVBO;
};

#endif // Cylinder_H
