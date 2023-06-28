#ifndef OPENGLSCENE_H
#define OPENGLSCENE_H

#include "Scene.h"
#include <memory>
#include "gl/shaders/CS123Shader.h"

#include "gl/datatype/FBO.h"
#include "Settings.h"
#include "mesh/Mesh.h"
#include "shapes/PolySoup.h"
#include "shapes/Skybox.h"
#include "SupportCanvas3D.h"
#include "Camera.h"
#include "gl/textures/TextureArray.h"

namespace CS123 { namespace GL {

    enum shaders {
        SHADER_FLAT,
        SHADER_GOURAUD,
        SHADER_PHONG
    };
}}


// Maximum number of lights, as defined in shader.
const int MAX_NUM_LIGHTS = 10;

using namespace CS123::GL;
using std::string;

/**
 * @class  OpenGLScene
 *
 * Basic Scene abstract class that supports OpenGL. Students will extend this class in ShapesScene
 * and SceneviewScene.
 */
class OpenGLScene : public Scene {
public:
    virtual ~OpenGLScene();
    virtual void settingsChanged() override;
    void initOpenGLScene();
    void render(SupportCanvas3D *context);

protected:
    std::shared_ptr<CS123::GL::CS123Shader> m_activeShader;
    std::shared_ptr<CS123::GL::CS123Shader> m_phongShader;
    std::shared_ptr<CS123::GL::CS123Shader> m_gouraudShader;
    std::shared_ptr<CS123::GL::CS123Shader> m_flatShader;
    std::shared_ptr<CS123::GL::CS123Shader> m_skyShader;
    std::shared_ptr<CS123::GL::CS123Shader> m_shadowShader;
    std::unique_ptr<CS123::GL::Shader> m_wireframeShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsArrowShader;
    std::unique_ptr<CS123::GL::Shader> m_fsqShader;

    std::unique_ptr<Skybox> m_skyBox;
    std::unique_ptr<std::vector<std::unique_ptr<FBO>>> m_shadowMaps;
    std::unique_ptr<TextureArray> m_shadowArray;

    virtual void renderGeometry() = 0;
    virtual void setSceneUniforms() = 0;
    virtual void setLights(const glm::mat4 viewMatrix);

    void clearLights();
    void setClearColor();

    void setActiveShader(int shader);

    void loadFlatShader();
    void loadGouraudShader();
    void loadPhongShader();
    void loadSkyShader();
    void loadWireframeShader();
    void loadNormalsShader();
    void loadNormalsArrowShader();
    void loadShadowShader();

    void generateShadowMaps(SupportCanvas3D *context);
    void renderGeometryAsFilledPolygons();
    void renderWireframePass(SupportCanvas3D *context);
    void renderShadowMap(SupportCanvas3D *context, FBO* shadowBuffer);
    void renderGeometryAsWireframe();
    void renderNormalsPass(SupportCanvas3D *context);
    void renderEnvPass(SupportCanvas3D *context);
    void renderPass(SupportCanvas3D *context);

    void setMatrixUniforms(CS123::GL::Shader *shader, SupportCanvas3D *context);
};

#endif // OPENGLSCENE_H
