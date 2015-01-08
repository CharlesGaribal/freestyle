/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "camera.h"

namespace vortex {
using namespace util;

void Camera::updateNFplanes(){

    glm::vec3 corner;
    glm::vec3 direction = viewDirection();
    float d;
    corner = min_;
    d = glm::dot( direction, corner-position());
    zNear_ = d;
    zFar_ = d;

    corner = glm::vec3(min_[0], min_[1], max_[2]);
    d = glm::dot( direction, corner-position());
    zNear_ = glm::min(glm::max(d, 0.1f), zNear_);
    zFar_ = glm::max(d, zFar_); // beware if all corners are behind the camera

    corner = glm::vec3(min_[0], max_[1], min_[2]);
    d = glm::dot( direction, corner-position());
    zNear_ = glm::min(glm::max(d, 0.1f), zNear_);
    zFar_ = glm::max(d, zFar_); // beware if all corners are behind the camera

    corner = glm::vec3(min_[0], max_[1], max_[2]);
    d = glm::dot( direction, corner-position());
    zNear_ = glm::min(glm::max(d, 0.1f), zNear_);
    zFar_ = glm::max(d, zFar_); // beware if all corners are behind the camera

    corner = max_;
    d = glm::dot( direction, corner-position());
    zNear_ = glm::min(glm::max(d, 0.1f), zNear_);
    zFar_ = glm::max(d, zFar_); // beware if all corners are behind the camera

    corner = glm::vec3(max_[0], max_[1], min_[2]);
    d = glm::dot( direction, corner-position());
    zNear_ = glm::min(glm::max(d, 0.1f), zNear_);
    zFar_ = glm::max(d, zFar_); // beware if all corners are behind the camera

    corner = glm::vec3(max_[0], min_[1], max_[2]);
    d = glm::dot( direction, corner-position());
    zNear_ = glm::min(glm::max(d, 0.1f), zNear_);
    zFar_ = glm::max(d, zFar_); // beware if all corners are behind the camera

    corner = glm::vec3(max_[0], min_[1], min_[2]);
    d = glm::dot( direction, corner-position());
    zNear_ = glm::min(glm::max(d, 0.1f), zNear_);
    zFar_ = glm::max(d, zFar_); // beware if all corners are behind the camera

    zNear_ = std::max(0.1f, zNear_-0.01f);
    zFar_ += 0.01;
}

Camera::Camera(){
    /// @todo manage zNear/zFar automagically
    zNear_ = 0.1;
    zFar_ = 100.f;
    fieldOfView_ = 45.*(M_PI/180.);
    frame_.setTranslation(glm::vec3(0.f,0.f,10.f));
    frame_.setOrientation(glm::angleAxis(0.f, glm::vec3(1.f,0.f,0.f)));
    revolveAroundPoint_ = glm::vec3(0.f);
}

Camera::Camera(const Camera &other){
    zNear_ = other.zNear_;
    zFar_ = other.zFar_;
    fieldOfView_ = other.fieldOfView_;
    frame_.setTranslation(other.frame_.translation());
    frame_.setOrientation(other.frame_.orientation());
    revolveAroundPoint_ = other.revolveAroundPoint();
    std::cerr << "Camera : " << translation() << " -- " << orientation() << std::endl;
}


Camera::Camera(aiCamera *camera){
    aiMatrix4x4 aiMatrix;
    camera->GetCameraMatrix (aiMatrix);
    glm::mat4x4 matrix(
        aiMatrix[0][0], aiMatrix[1][0], aiMatrix[2][0], aiMatrix[3][0],
        aiMatrix[0][1], aiMatrix[1][1], aiMatrix[2][1], aiMatrix[3][1],
        aiMatrix[0][2], aiMatrix[1][2], aiMatrix[2][2], aiMatrix[3][2],
        aiMatrix[0][3], aiMatrix[1][3], aiMatrix[2][3], aiMatrix[3][3]
    );
    glm::quat orient;
    orient = glm::quat_cast(matrix);
    glm::vec3 pos(camera->mPosition[0], camera->mPosition[1], camera->mPosition[2]);
    frame_.setTranslation(pos);
    frame_.setOrientation(orient);
    zNear_ = camera->mClipPlaneNear;
    zFar_ = camera->mClipPlaneFar;
    fieldOfView_=camera->mAspect*camera->mHorizontalFOV;
    revolveAroundPoint_ = glm::vec3(0.f);
}

void Camera::computeModelViewMatrix(){
    //orientation is from frame to world
    // modelViewMatrix is from world to frame, use inverse orientation to build it
    modelViewMatrix_ = glm::toMat4(glm::inverse(frame_.orientation()));
    // translate after rotation
    modelViewMatrix_ = glm::translate(modelViewMatrix_, -frame_.translation());

}

void Camera::showEntireScene() {
    float distance = 0.0f;
    const float yview = sceneRadius_/ (sin(fieldOfView()/2.0));
    const float xview = sceneRadius_/ (sin(fieldOfView()*aspectRatio()/2.0));
    distance = glm::max(xview,yview);
    glm::vec3 newPos(sceneCenter_ - distance * viewDirection());
    frame_.setTranslation(newPos);

    updateNFplanes();
}

void Camera::revolveAroundPointAxis(float d, glm::vec3 revolvePoint, glm::vec3 revolveAxis){
    frame_.setOrientation(glm::rotate(frame_.orientation(), d, revolveAxis));
    glm::vec3 trans = revolvePoint
            + glm::angleAxis(d, frame_.inverseTransformVec(revolveAxis)) * (frame_.translation()-revolvePoint)
            - frame_.translation();
    frame_.setTranslation(frame_.translation()+trans);
}
glm::vec3 Camera::revolveAroundPoint() const
{
    return revolveAroundPoint_;
}

void Camera::setRevolveAroundPoint(const glm::vec3 &revolveAroundPoint)
{
    revolveAroundPoint_ = revolveAroundPoint;
}
int Camera::width() const
{
    return width_;
}

void Camera::setWidth(int width)
{
    width_ = width;
}
int Camera::height() const
{
    return height_;
}

void Camera::setHeight(int height)
{
    height_ = height;
}
glm::vec3 Camera::sceneCenter() const
{
    return sceneCenter_;
}

void Camera::setSceneCenter(const glm::vec3 &sceneCenter)
{
    sceneCenter_ = sceneCenter;
}
float Camera::sceneRadius() const
{
    return sceneRadius_;
}

void Camera::setSceneRadius(float sceneRadius)
{
    sceneRadius_ = sceneRadius;
}



glm::vec3 Camera::project(glm::vec3 p){
    glm::vec4 q = projectionMatrix_*modelViewMatrix_*glm::vec4(p, 1.f);
    return glm::vec3(q)/q.w;
}


std::ostream & operator<<(std::ostream &os, const Camera &c){
    using vortex::util::operator<<;
    os << "Camera : (" << c.frame_.translation() << ") x (" << c.frame_.orientation() << ") // N+F : " << c.zNear() << " + "  << c.zFar() << " // min + max : (" << c.min_ << ") + (" << c.max_ << ") ";
    os << "// w x h ("<< c.width() << " x " << c.height() << ") " << std::endl;
    return os;
}
} // namespace vortex
