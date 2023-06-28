#include "ShapesScene.h"
#include "Camera.h"
#include "shapes/Shape.h"
#include "shapes/Sphere.h"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include "shapes/Torus.h"
#include "shapes/Skybox.h"
#include <SupportCanvas3D.h>
#include <QFileDialog>

#include <sstream>

#include "shapes/ExampleShape.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "gl/shaders/CS123Shader.h"
#include "gl/shaders/Shader.h"
#include "gl/textures/TextureCube.h"

#include "ResourceLoader.h"
#include <iostream>

ShapesScene::ShapesScene(int width, int height) :
    m_shape(nullptr),
    m_width(width),
    m_height(height)
{
    initOpenGLScene();
    initializeSceneMaterial();
    initializeSceneLight();

    m_skyBox = std::make_unique<Skybox>(
                 QStringList{
                        ":/textures/assets/cubemaps/icebergs/left.jpg",
                        ":/textures/assets/cubemaps/icebergs/right.jpg",
                        ":/textures/assets/cubemaps/icebergs/top.jpg",
                        ":/textures/assets/cubemaps/icebergs/bottom.jpg",
                        ":/textures/assets/cubemaps/icebergs/back.jpg",
                        ":/textures/assets/cubemaps/icebergs/front.jpg"
                 });

    m_skyShader->setTexture("skybox", m_skyBox->getTexture());
    m_phongShader->setTexture("envMap", m_skyBox->getTexture());
}

ShapesScene::~ShapesScene()
{
    // Pro-tip: If you use smart pointers properly, this destructor should be empty
}

void ShapesScene::initializeSceneMaterial() {
    setSceneMaterial(settings.materialParameter1, settings.materialParameter2, settings.materialParameter3, settings.materialParameter4, settings.materialParameter5);
}

void ShapesScene::setSceneMaterial(float r, float g, float b, float gloss, float roughness) {
    m_material.clear();
    m_material.cAmbient.r = r / (settings.lightingType ? 3 : 1);
    m_material.cAmbient.g = g / (settings.lightingType ? 3 : 1);
    m_material.cAmbient.b = b / (settings.lightingType ? 3 : 1);
    m_material.cDiffuse.r = r;
    m_material.cDiffuse.g = g;
    m_material.cDiffuse.b = b;
    m_material.cSpecular.r = m_material.cSpecular.g = m_material.cSpecular.b = 1 - roughness;
    m_material.roughness = roughness;
    m_material.metalness = settings.materialParameter6;
    m_material.glossiness = gloss;
    m_material.transparency = settings.materialParameter7;
    m_material.ior = glm::vec3{settings.materialParameter8, settings.materialParameter9, settings.materialParameter10};
    m_material.abs = glm::vec3{settings.materialParameter11, settings.materialParameter12, settings.materialParameter13};
}

void ShapesScene::initializeSceneLight() {
    // Use a white directional light from the upper left corner
    m_lights.push_back(std::make_unique<CS123SceneLightData>());
    m_lights[0]->type = LightType::LIGHT_DIRECTIONAL;
    m_lights[0]->dir = m_lightDirection;
    m_lights[0]->color.r = 250.0 / 255;
    m_lights[0]->color.g = 234.0 / 255;
    m_lights[0]->color.b = 200.0 / 255;
    m_lights[0]->id = 0;
}

void ShapesScene::setSceneUniforms() {
    m_activeShader->setUniform("useArrowOffsets", false);
    m_activeShader->setUniform("lightingType", settings.lightingType);
    m_activeShader->setUniform("shadingType", settings.shadingType);
    m_activeShader->applyMaterial(m_material);
}

void ShapesScene::renderGeometry() {
    // TODO: [SHAPES] Render the shape. Lab 1 seems like it'll come in handy...
    if (m_shape) {
        m_shape->draw();
    } else if (m_mesh) {
        m_mesh->draw();
    }
}

void ShapesScene::setLights(const glm::mat4 viewMatrix) {
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    m_lights[0]->dir = m_lightDirection;

    clearLights();
    m_activeShader->setLight(*m_lights[0]);
}

void ShapesScene::settingsChanged() {
    switch (settings.shapeType) {
        case SHAPE_CUBE:
            m_shape = std::make_unique<ExampleShape>(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case SHAPE_SPHERE:
            if (settings.shapeParameter3 > fmax(settings.shapeParameter1, settings.shapeParameter2))
                m_shape = std::make_unique<Sphere>(settings.shapeParameter3);
            else
                m_shape = std::make_unique<Sphere>(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case SHAPE_CYLINDER:
            m_shape = std::make_unique<Cylinder>(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case SHAPE_CONE:
            m_shape = std::make_unique<Cone>(settings.shapeParameter1, settings.shapeParameter2);
            break;
        case SHAPE_TORUS:
            m_shape = std::make_unique<Torus>(settings.shapeParameter1, settings.shapeParameter2, settings.shapeParameter3);
            break;
        case SHAPE_MESH:
            m_shape = nullptr;
            break;
        default:
            break;
    }

    setActiveShader(settings.shadingType);
    setSceneMaterial(settings.materialParameter1, settings.materialParameter2, settings.materialParameter3, settings.materialParameter4, settings.materialParameter5);

}

void ShapesScene::setMesh(Mesh* m) {
    m_mesh = std::unique_ptr<Mesh>(m);
}

void ShapesScene::subdivideMesh() {
    if (m_mesh) {
        m_mesh->subdivide();
    }
}

