#ifndef Cone_H
#define Cone_H

#include "Shape.h"
#include <glm/glm.hpp>
#include "GL/glew.h"

#include<memory>
#include <vector>

namespace CS123 { namespace GL {
class VAO;
}}

class Cone : public Shape
{
public:
    Cone();
    Cone(int tesselation, glm::mat4x4 m);
    Cone(int tesselation);
    Cone(int stacks, int slices);
    void refresh() override;
    void draw();
private:
    std::vector<GLfloat> m_footData;

    std::unique_ptr<CS123::GL::VBO> m_footVBO;
    std::unique_ptr<CS123::GL::VBO> m_vertVBO;
};

#endif // Cone_H
