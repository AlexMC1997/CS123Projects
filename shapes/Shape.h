#ifndef SHAPE_H
#define SHAPE_H

/** imports the OpenGL math library https://glm.g-truc.net/0.9.2/api/a00001.html */
#include <glm/glm.hpp>
#include "GL/glew.h"

#include "gl/datatype/VAO.h"
#include "gl/datatype/VBO.h"
#include "gl/datatype/VBOAttribMarker.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include "CS123SceneData.h"

#include<memory>
#include <vector>

/**
 *
 * inserts a glm::vec3 into a vector of floats
 * this will come in handy if you want to take advantage of vectors to build your shape
 * make sure to call reserve beforehand to speed this up
 */
inline void insertVec3(std::vector<float> &data, glm::vec3 v){
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

namespace CS123 { namespace GL {
class VAO;
}}

class Shape
{
public:
    Shape();
    virtual ~Shape();
    virtual void draw();
    virtual void refresh();
    void transform(glm::mat4x4 m);
    glm::mat4x4 world();
    CS123SceneMaterial getMaterial();
    void setMaterial(CS123SceneMaterial mat);

protected:
    /** builds the VAO, pretty much the same as from lab 1 */
    void buildVAO();

    glm::mat4x4 m_world;
    CS123SceneMaterial m_material;
    std::vector<CS123::GL::VBOAttribMarker> m_markers;
    std::vector<GLfloat> m_vertexData;
    std::unique_ptr<CS123::GL::VAO> m_VAO;

};

#endif // SHAPE_H
