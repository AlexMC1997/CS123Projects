#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include <glm/glm.hpp>
#include "GL/glew.h"

#include<memory>
#include <vector>

namespace CS123 { namespace GL {
class VAO;
}}

class Sphere : public Shape
{
public:
    Sphere();
    Sphere(int tesselation);
    Sphere(int stacks, int slices);
    Sphere(glm::mat4x4 m);
    Sphere(int tesselation, glm::mat4x4 m);
    void refresh() override;
    void draw() override;
};

#endif // SPHERE_H
