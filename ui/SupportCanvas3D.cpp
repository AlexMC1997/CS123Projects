#include "SupportCanvas3D.h"

#include <QFileDialog>
#include <QMouseEvent>
#include <QMessageBox>
#include <QApplication>

#include "RGBA.h"
#include "CamtransCamera.h"
#include "OrbitingCamera.h"
#include "SceneviewScene.h"
#include "Settings.h"
#include "ShapesScene.h"

#include <iostream>
#include "gl/GLDebug.h"
#include "CS123XmlSceneParser.h"

SupportCanvas3D::SupportCanvas3D(QGLFormat format, QWidget *parent) : QGLWidget(format, parent),
    m_isDragging(false),
    m_isDraggingL(false),
    m_settingsDirty(true),
    m_defaultPerspectiveCamera(new CamtransCamera()),
    m_defaultOrbitingCamera(new OrbitingCamera()),
    m_currentScene(nullptr)
{
}

SupportCanvas3D::~SupportCanvas3D()
{
}

Camera *SupportCanvas3D::getCamera() {
    switch(settings.getCameraMode()) {
        case CAMERAMODE_CAMTRANS:
            return m_defaultPerspectiveCamera.get();

        case CAMERAMODE_ORBIT:
            return m_defaultOrbitingCamera.get();

        default:
            return nullptr;
    }
}

OrbitingCamera *SupportCanvas3D::getOrbitingCamera() {
    return m_defaultOrbitingCamera.get();
}


CamtransCamera *SupportCanvas3D::getCamtransCamera() {
    return m_defaultPerspectiveCamera.get();
}

void SupportCanvas3D::initializeGL() {
    // Track the camera settings so we can generate deltas
    m_oldPosX = settings.cameraPosX;
    m_oldPosY = settings.cameraPosY;
    m_oldPosZ = settings.cameraPosZ;
    m_oldRotU = settings.cameraRotU;
    m_oldRotV = settings.cameraRotV;
    m_oldRotN = settings.cameraRotN;

    initializeGlew();

    initializeOpenGLSettings();
    initializeScenes();
    setSceneFromSettings();

    settingsChanged();

}

void SupportCanvas3D::initializeGlew() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    glGetError(); // Clear errors after call to glewInit
    if (GLEW_OK != err) {
      // Problem: glewInit failed, something is seriously wrong.
      fprintf(stderr, "Error initializing glew: %s\n", glewGetErrorString(err));
    }
}

void SupportCanvas3D::initializeOpenGLSettings() {
    // Enable depth testing, so that objects are occluded based on depth instead of drawing order.
    glEnable(GL_DEPTH_TEST);

    // Move the polygons back a bit so lines are still drawn even though they are coplanar with the
    // polygons they came from, which will be drawn before them.
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1, -1);

    // Enable back-face culling, meaning only the front side of every face is rendered.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Specify that the front face is represented by vertices in counterclockwise order (this is
    // the default).
    glFrontFace(GL_CCW);

    // Calculate the orbiting camera matrices.
    getOrbitingCamera()->updateMatrices();

    glEnable(GL_MULTISAMPLE);
}

void SupportCanvas3D::initializeScenes() {
    m_sceneviewScene = std::make_unique<SceneviewScene>();
    m_shapesScene = std::make_unique<ShapesScene>(width(), height());
}

void SupportCanvas3D::paintGL() {
    if (m_settingsDirty) {
        setSceneFromSettings();
    }

    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    glViewport(0, 0, width() * ratio, height() * ratio);
    getCamera()->setAspectRatio(static_cast<float>(width()) / static_cast<float>(height()));
    glEnable(GL_MULTISAMPLE);
    m_currentScene->settingsChanged();
    m_currentScene->render(this);
}

void SupportCanvas3D::resetViewport() {
    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    glViewport(0, 0, width() * ratio, height() * ratio);
}

void SupportCanvas3D::settingsChanged() {
    m_settingsDirty = true;
    if (m_currentScene != nullptr) {
        // Just calling this function so that the scene is always updated.
        setSceneFromSettings();
        m_currentScene->settingsChanged();
    }
    update(); /* repaint the scene */
}

void SupportCanvas3D::setSceneFromSettings() {
    switch(settings.getSceneMode()) {
        case SCENEMODE_SHAPES:
            setSceneToShapes();
            break;
        case SCENEMODE_SCENEVIEW:
            setSceneToSceneview();
            break;
    }
    m_settingsDirty = false;
}

void SupportCanvas3D::loadSceneviewSceneFromParser(CS123XmlSceneParser &parser) {
    m_sceneviewScene = std::make_unique<SceneviewScene>();
    Scene::parse(m_sceneviewScene.get(), &parser);
    m_settingsDirty = true;
}

void SupportCanvas3D::loadMeshToShapesScene(QString mFile) {
    QFile f(mFile);
    Mesh* mesh = new Mesh(f);
    m_shapesScene->setMesh(mesh);
}

void SupportCanvas3D::subdivideShapesMesh() {
    m_shapesScene->subdivideMesh();
}

void SupportCanvas3D::setSceneToSceneview() {
    assert(m_sceneviewScene.get());
    m_currentScene = m_sceneviewScene.get();
}

void SupportCanvas3D::setSceneToShapes() {
    assert(m_shapesScene.get());
    m_currentScene = m_shapesScene.get();
}

void SupportCanvas3D::copyPixels(int width, int height, RGBA *data) {
    glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, data);
    std::cout << "copied " << width << "x" << height << std::endl;

    // Flip the image and since OpenGL uses an origin in the lower left and we an origin in the
    // upper left.
    for (int y = 0; y < (height + 1) / 2; y++)
        for (int x = 0; x < width; x++)
            std::swap(data[x + y * width], data[x + (height - y - 1) * width]);
}

void SupportCanvas3D::resetUpVector() {
    // Reset the up vector to the y axis
    glm::vec4 up = glm::vec4(0.f, 1.f, 0.f, 0.f);
    if (fabs(glm::length(m_defaultPerspectiveCamera->getUp() - up)) > 0.0001f) {
        m_defaultPerspectiveCamera->orientLook(
                    m_defaultPerspectiveCamera->getPosition(),
                    m_defaultPerspectiveCamera->getLook(),
                    up);
        update();
    }
}


void SupportCanvas3D::setCameraAxisX() {
    m_defaultPerspectiveCamera->orientLook(
                glm::vec4(2.f, 0.f, 0.f, 1.f),
                glm::vec4(-1.f, 0.f, 0.f, 0.f),
                glm::vec4(0.f, 1.f, 0.f, 0.f));
    update();
}

void SupportCanvas3D::setCameraAxisY() {
    m_defaultPerspectiveCamera->orientLook(
                glm::vec4(0.f, 2.f, 0.f, 1.f),
                glm::vec4(0.f, -1.f, 0.f, 0.f),
                glm::vec4(0.f, 0.f, 1.f, 0.f));
    update();
}

void SupportCanvas3D::setCameraAxisZ() {
    m_defaultPerspectiveCamera->orientLook(
                glm::vec4(0.f, 0.f, 2.f, 1.f),
                glm::vec4(0.f, 0.f, -1.f, 0.f),
                glm::vec4(0.f, 1.f, 0.f, 0.f));
    update();
}

void SupportCanvas3D::setCameraAxonometric() {
    m_defaultPerspectiveCamera->orientLook(
                glm::vec4(2.f, 2.f, 2.f, 1.f),
                glm::vec4(-1.f, -1.f, -1.f, 0.f),
                glm::vec4(0.f, 1.f, 0.f, 0.f));
    update();
}

void SupportCanvas3D::updateCameraHeightAngle() {
    // The height angle is half the overall field of view of the camera
    m_defaultPerspectiveCamera->setHeightAngle(settings.cameraFov);
}

void SupportCanvas3D::updateCameraTranslation() {
    m_defaultPerspectiveCamera->translate(
            glm::vec4(
                settings.cameraPosX - m_oldPosX,
                settings.cameraPosY - m_oldPosY,
                settings.cameraPosZ - m_oldPosZ,
                0));

    m_oldPosX = settings.cameraPosX;
    m_oldPosY = settings.cameraPosY;
    m_oldPosZ = settings.cameraPosZ;
}

void SupportCanvas3D::updateCameraRotationU() {
    m_defaultPerspectiveCamera->rotateU(settings.cameraRotU - m_oldRotU);
    m_oldRotU = settings.cameraRotU;
}

void SupportCanvas3D::updateCameraRotationV() {
    m_defaultPerspectiveCamera->rotateV(settings.cameraRotV - m_oldRotV);
    m_oldRotV = settings.cameraRotV;
}

void SupportCanvas3D::updateCameraRotationN() {
    m_defaultPerspectiveCamera->rotateW(settings.cameraRotN - m_oldRotN);
    m_oldRotN = settings.cameraRotN;
}

void SupportCanvas3D::updateCameraClip() {
    m_defaultPerspectiveCamera->setClip(settings.cameraNear, settings.cameraFar);
}


void SupportCanvas3D::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        getCamera()->mouseDown(event->x(), event->y());
        m_isDragging = true;
        update();
    } if (event->button() == Qt::LeftButton) {
        getCamera()->mouseDown(event->x(), event->y());
        m_isDraggingL = true;
        update();
    }
}

void SupportCanvas3D::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        getCamera()->mouseDragged(event->x(), event->y());
        update();
    } else if (m_isDraggingL) {
        getCamera()->mousePan(event->x(), event->y());
        update();
    }
}

void SupportCanvas3D::mouseReleaseEvent(QMouseEvent *event) {
    if (m_isDragging && event->button() == Qt::RightButton) {
        getCamera()->mouseUp(event->x(), event->y());
        m_isDragging = false;
        update();
    }
}

void SupportCanvas3D::wheelEvent(QWheelEvent *event) {
    getCamera()->mouseScrolled(event->delta());
    update();
}

void SupportCanvas3D::resizeEvent(QResizeEvent *event) {
    emit aspectRatioChanged();
}
