#ifndef SCENE_H
#define SCENE_H

#include "CS123SceneData.h"
#include <memory>
#include <vector>
#include "shapes/Shape.h"
#include "mesh/Mesh.h"

class Camera;
class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);

protected:

    virtual void parseNode(CS123SceneNode* node, glm::mat4x4 transform);
    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    std::vector<std::unique_ptr<Shape>> m_objects;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<CS123SceneLightData>> m_lights;

};

#endif // SCENE_H
