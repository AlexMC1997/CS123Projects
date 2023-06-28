#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
using namespace CS123::GL;


SceneviewScene::SceneviewScene()
{
    initOpenGLScene();
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

}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::setSceneUniforms() {
    m_activeShader->setUniform("useArrowOffsets", false);
    m_activeShader->setUniform("lightingType", settings.lightingType);
    m_activeShader->setUniform("shadingType", settings.shadingType);
}

void SceneviewScene::renderGeometry() {
    for (std::size_t i = 0; i < m_objects.size(); i++) {
        m_activeShader->applyMaterial(m_objects[i]->getMaterial());
        m_activeShader->setUniform("m", m_objects[i]->world());
        m_activeShader->setUniform("iw", glm::inverse(m_objects[i]->world()));
        m_wireframeShader->setUniform("m", m_objects[i]->world());
        m_objects[i]->draw();
    }

    for (std::size_t i = 0; i < m_meshes.size(); i++) {
        m_activeShader->applyMaterial(m_meshes[i]->getMaterial());
        m_activeShader->setUniform("m", m_meshes[i]->world());
        m_activeShader->setUniform("iw", glm::inverse(m_meshes[i]->world()));
        m_wireframeShader->setUniform("m", m_meshes[i]->world());
        m_meshes[i]->draw();
    }

}

void SceneviewScene::settingsChanged() {
    setActiveShader(settings.shadingType);
}

