#include "OpenGLScene.h"

#include <GL/glew.h>

#include "Settings.h"

#include "ResourceLoader.h"

#include <iostream>
#include <sstream>
#include "glm/gtc/matrix_transform.hpp"

OpenGLScene::~OpenGLScene()
{
}

void OpenGLScene::initOpenGLScene()
{
    std::vector<float>* arr = new std::vector<float>();
    for (int i = 0 ; i <  256 * 256; arr->push_back(1.0f), i++);
    m_shadowArray = std::make_unique<TextureArray>(256, 256, MAX_NUM_LIGHTS, GL_FLOAT, GL_DEPTH_COMPONENT, 0, nullptr);
    m_shadowMaps = std::make_unique<std::vector<std::unique_ptr<FBO>>>();

    m_shadowArray->bind();
    loadPhongShader();
    loadGouraudShader();
    loadFlatShader();
    loadSkyShader();
    loadShadowShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();
    setActiveShader(SHADER_PHONG);

    m_phongShader->setTexture("shadowmaps", *m_shadowArray);
    m_gouraudShader->setTexture("shadowmaps", *m_shadowArray);
    m_flatShader->setTexture("shadowmaps", *m_shadowArray);
}

void OpenGLScene::setClearColor() {
    if (settings.drawWireframe || settings.drawNormals) {
        glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    } else {
        glClearColor(0, 0, 0, 0);
    }
}

void OpenGLScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.frag");
    m_phongShader = std::make_shared<CS123Shader>(vertexSource, fragmentSource);
}

void OpenGLScene::loadSkyShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/sky.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/sky.frag");
    m_skyShader = std::make_shared<CS123Shader>(vertexSource, fragmentSource);
}

void OpenGLScene::loadGouraudShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_gouraudShader = std::make_shared<CS123Shader>(vertexSource, fragmentSource);
}

void OpenGLScene::loadFlatShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/flat.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/flat.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/flat.frag");
    m_flatShader = std::make_shared<CS123Shader>(vertexSource, geometrySource, fragmentSource);
}

void OpenGLScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void OpenGLScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void OpenGLScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void OpenGLScene::loadShadowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shadow.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shadow.frag");
    m_shadowShader = std::make_shared<CS123Shader>(vertexSource, fragmentSource);
}

void OpenGLScene::render(SupportCanvas3D *context) {
    // Clear the screen in preparation for the next frame. (Use a gray background instead of a
    // black one for drawing wireframe or normals so they will show up against the background.)
    setClearColor();

    if (false) {
        generateShadowMaps(context);
    }

    renderPass(context);

    if (true) {
        renderEnvPass(context);
    }

    if (settings.drawWireframe) {
        renderWireframePass(context);
    }

    if (settings.drawNormals) {
        renderNormalsPass(context);
    }

}

void OpenGLScene::renderPass(SupportCanvas3D *context) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_activeShader->bind();

    clearLights();
    setLights(context->getCamera()->getViewMatrix());
    setSceneUniforms();
    setMatrixUniforms(m_activeShader.get(), context);
    renderGeometryAsFilledPolygons();

    m_activeShader->unbind();
}

void OpenGLScene::renderGeometryAsFilledPolygons() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderGeometry();
}

void OpenGLScene::renderWireframePass(SupportCanvas3D *context) {
    m_wireframeShader->bind();
    setMatrixUniforms(m_wireframeShader.get(), context);
    renderGeometryAsWireframe();
    m_wireframeShader->unbind();
}

void OpenGLScene::renderGeometryAsWireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    renderGeometry();
}

void OpenGLScene::renderNormalsPass (SupportCanvas3D *context) {
    // Render the lines.
    m_normalsShader->bind();
    setMatrixUniforms(m_normalsShader.get(), context);
    renderGeometryAsWireframe();
    m_normalsShader->unbind();

    // Render the arrows.
    m_normalsArrowShader->bind();
    setMatrixUniforms(m_normalsArrowShader.get(), context);
    renderGeometryAsFilledPolygons();
    m_normalsArrowShader->unbind();
}

void OpenGLScene::renderEnvPass(SupportCanvas3D *context) {
    m_skyShader->bind();

    glDepthFunc(GL_LEQUAL);
    setMatrixUniforms(m_skyShader.get(), context);
    if (m_skyBox)
        m_skyBox->draw();
    glDepthFunc(GL_LESS);

    m_skyShader->unbind();
}

void OpenGLScene::generateShadowMaps(SupportCanvas3D* context) {
    glm::vec3 lPos;
    glm::mat4 lProj;
    glm::mat4 lView;
    m_shadowMaps->clear();

    for (std::size_t i = 0; i < m_lights.size(); i++) {
        switch (m_lights[i]->type) {
            case LightType::LIGHT_DIRECTIONAL:
                lPos = -20.0f * glm::normalize(glm::vec3{m_lights[i]->dir});
                lProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 80.0f);
                lView = glm::lookAt(lPos, glm::vec3{0}, glm::vec3{0, 1, 0});
                break;
            default:
                continue;
    }

//        auto lTrans = lProj * lView;
//        m_shadowMaps->push_back(std::make_unique<FBO>(0, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, 256, 256));
//        m_shadowArray->bind();
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//        m_shadowArray->linkFBO(i, 0, *m_shadowMaps->back(), GL_DEPTH_ATTACHMENT);
//        m_shadowArray->unbind();
//        m_shadowMaps->back()->status();
//        m_shadowShader->bind();
//        m_shadowShader->setUniform("lTransform", lTrans);
//        m_shadowShader->setUniform("lColor", m_lights[i]->color);
//        m_activeShader->bind();
//        m_activeShader->setUniformArrayByIndex("lightTransforms", lTrans, i);
//        m_activeShader->unbind();
//        renderShadowMap(context, m_shadowMaps->back().get());
    }
}

void OpenGLScene::renderShadowMap(SupportCanvas3D* context, FBO* shadowBuffer) {
    auto tmp = m_activeShader;
    m_activeShader = m_shadowShader;
    m_activeShader->bind();
//    uint buf;
//    std::vector<float>* arr = new std::vector<float>(256 * 256);
//    glGenFramebuffers(1, &buf);
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, buf);
//    glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_shadowArray->id(), 0, 0);
//    glReadBuffer(GL_COLOR_ATTACHMENT0);
//    glReadPixels(0, 0, 256, 256, GL_RED, GL_FLOAT, arr->data());
    shadowBuffer->bind();
    glEnable(GL_DEPTH);
    glClear(GL_DEPTH_BUFFER_BIT);
//    renderGeometryAsFilledPolygons();
    glFinish();
    shadowBuffer->unbind();
    m_activeShader->unbind();
    m_activeShader = tmp;
    context->resetViewport();
}

void OpenGLScene::setActiveShader(int shader) {
    switch (shader) {
        case SHADER_FLAT:
            m_activeShader = m_flatShader;
            break;
        case SHADER_GOURAUD:
            m_activeShader = m_gouraudShader;
            break;
        case SHADER_PHONG:
            m_activeShader = m_phongShader;
            break;
    }
}

void OpenGLScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    auto iw = glm::inverse(context->getCamera()->getViewMatrix());
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
    shader->setUniform("iw", iw);
    shader->setUniform("iv", glm::inverse(context->getCamera()->getViewMatrix()));
    shader->setUniform("m", glm::mat4(1.0f));
}

void OpenGLScene::setLights(const glm::mat4 viewMatrix)
{
    clearLights();
    for (std::size_t i = 0; i < m_lights.size(); i++) {
        auto l = *m_lights[i];
        m_activeShader->setLight(l);
    }
}

void OpenGLScene::clearLights() {
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        m_activeShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void OpenGLScene::settingsChanged() {

}
