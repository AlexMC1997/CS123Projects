/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include "glm/gtc/quaternion.hpp"
#include <Settings.h>

CamtransCamera::CamtransCamera() :
    m_aspectRatio(1),
    m_near(1),
    m_far(30),
    m_thetaH(M_PI / 3),
    m_thetaW(M_PI / 3)

{
    orientLook(glm::vec4{2, 2, 2, 1}, glm::vec4{-2, -2, -2, 0}, glm::vec4{0, 1, 0, 0});
}

void CamtransCamera::setAspectRatio(float a)
{
    // @TODO: [CAMTRANS] Fill this in...
    m_aspectRatio = a;
    m_thetaW = 2 * glm::atan(m_aspectRatio * glm::tan(m_thetaH / 2));
    updateProjectionMatrix();
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_perspectiveTransformation * m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_rotationMatrix * m_translationMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_perspectiveTransformation;
}

glm::vec4 CamtransCamera::getPosition() const {
    // @TODO: [CAMTRANS] Fill this in...
    return glm::vec4();
}

glm::vec4 CamtransCamera::getLook() const {
    // @TODO: [CAMTRANS] Fill this in...
    return -m_w;
}

glm::vec4 CamtransCamera::getUp() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_up;
}

glm::vec4 CamtransCamera::getU() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_u;
}

glm::vec4 CamtransCamera::getV() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_v;
}

glm::vec4 CamtransCamera::getW() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_w;
}

float CamtransCamera::getAspectRatio() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    // @TODO: [CAMTRANS] Fill this in...
    return glm::degrees(m_thetaH);
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    // @TODO: [CAMTRANS] Fill this in...
    m_eye = eye;
    m_up = glm::normalize(up);
    m_w = glm::normalize(-look);
    m_v = glm::normalize(m_up - (glm::dot(m_up, m_w) * m_w));
    m_u = glm::normalize(glm::vec4{glm::cross(glm::vec3{m_v}, glm::vec3{m_w}), 0});
    updateViewMatrix();
    updateProjectionMatrix();

}

void CamtransCamera::setHeightAngle(float h) {
    // @TODO: [CAMTRANS] Fill this in...
    m_thetaH = glm::radians(h);
    m_thetaW = 2 * glm::atan(m_aspectRatio * glm::tan(m_thetaH / 2));
    updateProjectionMatrix();
}

void CamtransCamera::translate(const glm::vec4 &v) {
    // @TODO: [CAMTRANS] Fill this in...
    m_eye += v;
    updateViewMatrix();
}

void CamtransCamera::rotateU(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    auto q_rot = glm::rotate(glm::quat{0, glm::vec3{m_w}}, glm::radians(degrees), glm::vec3{m_u});
    auto v_w = glm::normalize(glm::vec4{q_rot.x, q_rot.y, q_rot.z, 0});
    q_rot = glm::rotate(glm::quat{0, glm::vec3{m_up}}, glm::radians(degrees), glm::vec3{m_u});
    auto v_up = glm::normalize(glm::vec4{q_rot.x, q_rot.y, q_rot.z, 0});
    orientLook(m_eye, -v_w, v_up);

}

void CamtransCamera::rotateV(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    auto q_rot = glm::rotate(glm::quat{0, glm::vec3{m_w}}, glm::radians(degrees), glm::vec3{m_v});
    auto v_w = glm::normalize(glm::vec4{q_rot.x, q_rot.y, q_rot.z, 0});
    q_rot = glm::rotate(glm::quat{0, glm::vec3{m_up}}, glm::radians(degrees), glm::vec3{m_v});
    auto v_up = glm::normalize(glm::vec4{q_rot.x, q_rot.y, q_rot.z, 0});
    orientLook(m_eye, -v_w, v_up);

}

void CamtransCamera::rotateW(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    auto q_rot = glm::rotate(glm::quat{0, glm::vec3{m_up}}, glm::radians(degrees), glm::vec3{m_w});
    auto v_up = glm::normalize(glm::vec4{q_rot.x, q_rot.y, q_rot.z, 0});
    orientLook(m_eye, -m_w, v_up);

}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    // @TODO: [CAMTRANS] Fill this in...
    m_near = nearPlane;
    m_far = farPlane;
    updateProjectionMatrix();
}


void CamtransCamera::updateProjectionMatrix() {
    updateScaleMatrix();
    updatePerspectiveMatrix();

}

void CamtransCamera::updatePerspectiveMatrix() {
    float c = -m_near / m_far;
    m_perspectiveTransformation = glm::transpose(glm::mat4{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1 / (1 + c), -c / (1 + c),
        0, 0, -1, 0
    });

}

void CamtransCamera::updateScaleMatrix() {
    m_scaleMatrix = glm::transpose(glm::mat4{
        1.0f / (m_far * glm::tan(m_thetaW / 2)), 0, 0, 0,
        0, 1.0f / (m_far * glm::tan(m_thetaH / 2)), 0, 0,
        0, 0, 1.0f / m_far, 0,
        0, 0, 0, 1
    });
}

void CamtransCamera::updateViewMatrix() {
    updateTranslationMatrix();
    updateRotationMatrix();
}

void CamtransCamera::updateRotationMatrix() {
    m_rotationMatrix = glm::transpose(glm::mat4 {
        m_u.x, m_u.y, m_u.z, 0,
        m_v.x, m_v.y, m_v.z, 0,
        m_w.x, m_w.y, m_w.z, 0,
        0,         0,     0, 1
    });
}

void CamtransCamera::updateTranslationMatrix() {
    m_translationMatrix = glm::transpose(glm::mat4 {
            1, 0, 0, -m_eye.x,
            0, 1, 0, -m_eye.y,
            0, 0, 1, -m_eye.z,
            0, 0, 0,        1
    });
}

