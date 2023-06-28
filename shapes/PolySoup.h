#ifndef POLYSOUP_H
#define POLYSOUP_H

#include "Shape.h"

namespace CS123 { namespace GL {
class VAO;
}}

class PolySoup : public Shape
{
public:
    PolySoup();
    void addVert(glm::vec3 vert, glm::vec3 norm);
    void copyData(std::size_t len, float data[]);
    void refresh() override;
    void draw();
};

#endif // POLYSOUP_H
