#include "Scene.h"
#include "Camera.h"
#include "CS123ISceneParser.h"
#include "CS123SceneData.h"

#include "glm/gtx/transform.hpp"
#include "shapes/Sphere.h"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include "shapes/ExampleShape.h"
#include "Settings.h"

Scene::Scene()
{
}

Scene::Scene(Scene &scene)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)
    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(global);

    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.

}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()

    CS123SceneNode* cur = parser->getRootNode();
    auto t = glm::mat4x4 {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
    };
    CS123SceneLightData lt;
    for (std::size_t i = 0; i < parser->getNumLights(); i++) {
        parser->getLightData(i, lt);
        sceneToFill->addLight(lt);
    }
    sceneToFill->parseNode(cur, t);
}

void Scene::parseNode(CS123SceneNode* node, glm::mat4 transform) {
    auto t = transform;
    for (auto m : node->transformations) {
        switch (m->type) {
            case TRANSFORMATION_TRANSLATE:
                t = t * glm::translate(m->translate);
                break;
            case TRANSFORMATION_SCALE:
                t = t * glm::scale(m->scale);
                break;
            case TRANSFORMATION_ROTATE:
                t = t * glm::rotate(m->angle, m->rotate);
                break;
            case TRANSFORMATION_MATRIX:
                t = t * m->matrix;
                break;
        }
    }

    for (auto p : node->primitives)
        addPrimitive(*p, t);

    for (auto c : node->children)
        parseNode(c, t);
}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix) {
        switch (scenePrimitive.type) {
            case PrimitiveType::PRIMITIVE_SPHERE:
                m_objects.push_back(std::make_unique<Sphere>(30, matrix));
                m_objects.back()->setMaterial(scenePrimitive.material);
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                m_objects.push_back(std::make_unique<Cylinder>(30, matrix));
                m_objects.back()->setMaterial(scenePrimitive.material);
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                m_objects.push_back(std::make_unique<Cone>(30, matrix));
                m_objects.back()->setMaterial(scenePrimitive.material);
                break;
            case PrimitiveType::PRIMITIVE_CUBE:
                m_objects.push_back(std::make_unique<ExampleShape>(matrix));
                m_objects.back()->setMaterial(scenePrimitive.material);
                break;
            case PrimitiveType::PRIMITIVE_MESH:
                m_meshes.push_back(std::make_unique<Mesh>(scenePrimitive.meshfile));
                m_meshes.back()->transform(matrix);
                m_meshes.back()->setMaterial(scenePrimitive.material);
                break;
            default:
                break;
        }
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    m_lights.push_back(std::make_unique<CS123SceneLightData>(sceneLight));
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
}

