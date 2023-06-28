#ifndef SHAPESSCENE_H
#define SHAPESSCENE_H

#include "OpenGLScene.h"

#include <memory>

#include <GL/glew.h>

#include "gl/datatype/FBO.h"
#include "Settings.h"
#include "mesh/Mesh.h"
#include "shapes/PolySoup.h"
#include "shapes/Skybox.h"

class Shape;

/**
 *
 * @class ShapesScene
 *
 * A scene that is used to render a single shape.
 *
 * This scene has no notion of the scene graph, so it will not be useful to you in
 * assignments requiring the display of multiple shapes. Luckily, the first time you
 * will need that kind of functionality is in the Sceneview assignment... and we've
 * left that task up to you in the SceneviewScene class.
 *
 * By default, the ShapesScene displays only a single triangle. You'll need to do
 * a little work here to render your shapes. You could render the shapes directly
 * in this class, or you could pass the work on to one or more subclasses. Think
 * carefully about your design here - you'll be reusing your shapes multiple times
 * during this course!
 */
class ShapesScene : public OpenGLScene {
public:
    ShapesScene(int width, int height);
    virtual ~ShapesScene();

    virtual void settingsChanged() override;
    void setMesh(Mesh* mesh);
    void subdivideMesh();


protected:
    // Set the light uniforms for the lights in the scene. (The view matrix is used so that the
    // light can follows the camera.)
    void setLights(const glm::mat4 viewMatrix) override;

    // Render geometry for Shapes and Sceneview.
    void renderGeometry() override;

private:
    // Storage for private copies of the scene's light and material data. Note that these don't
    // need to be freed because they are VALUE types (not pointers) and the memory for them is
    // freed when the class itself is freed.

    CS123SceneMaterial   m_material;

    glm::vec4 m_lightDirection = glm::normalize(glm::vec4(-0.45f, -0.45f, -1.f, 0.f));

    // essentially an OpenGLShape from lab 1
    std::unique_ptr<Shape> m_shape;
    std::unique_ptr<Mesh> m_mesh;

    int m_width;
    int m_height;

    void initializeSceneMaterial();
    void initializeSceneLight();
    void setSceneMaterial(float r, float g, float b, float gloss, float roughness);
    void renderFilledPolygons();
    void renderNormals();
    void renderWireframe();
    void setSceneUniforms() override;
};

#endif // SHAPESSCENE_H
