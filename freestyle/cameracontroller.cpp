#include "cameracontroller.h"
#include <math.h>

using namespace vortex::ui;

static float projectOnBall(float x, float y)
{
  const float size       = 1.0f;
  const float size2      = size*size;
  const float size_limit = size2*0.5;

  const float d = x*x + y*y;
  return d < size_limit ? sqrt(size2 - d) : size_limit/sqrt(d);
}


void CameraController::mouseMoveEvent(QMouseEvent *e){
    float dx = e->x()-mouseX_;
    float dy = e->y()-mouseY_;

    dx *= sensitivity_;
    dy *= sensitivity_;

    if(isClick_){
        if(action_ == TRANSLATE){
            camera_->moveRight(-dx);
            camera_->moveUp(dy);
        }
        if(action_ == FORWARD){
            camera_->moveForward(dy);
        }
        if(action_ == ROTATE){
            camera_->rotateAroundUp(-dx * rotationSensitivity() / (sensitivity_ *(float)(camera_->width())));
            camera_->rotateAroundRight(-dy * rotationSensitivity() / (sensitivity_ *(float)(camera_->height())));
        }
        if(action_ == REVOLVE){
            glm::vec3 projPoint = camera_->project(camera_->revolveAroundPoint());
            projPoint = (projPoint * .5f + .5f) * glm::vec3(camera_->width(), camera_->height(), 0.f);
            float cx = projPoint.x;
            float cy = projPoint.y;

            // Points on the deformed ball
            float px = rotationSensitivity() * (float)(mouseX_  - cx) / (float)(camera_->width());
            float py = rotationSensitivity() * (float)(cy - mouseY_)  / (float)(camera_->height());
            float dx = rotationSensitivity() * (float)(e->x() - cx)	    / (float)(camera_->width());
            float dy = rotationSensitivity() * (float)(cy - e->y())	    / (float)(camera_->height());

            const glm::vec3  p1(px, py, projectOnBall(px, py));
            const glm::vec3  p2(dx, dy, projectOnBall(dx, dy));
            const glm::vec3  axis = glm::cross(p2,p1);
            if (glm::dot(axis, axis) != 0.){
                // BUG POTENTIEL : D'ou vient-il ? MACOS ? QT 5.3.2 ?
                // difficile à reproduire mais si p1 == p2 ==> nan dans la position/direction de la caméra
                const float angle = 2.0 * std::asin(std::sqrt(glm::dot(axis, axis) / (glm::dot(p1, p1) * glm::dot(p2, p2))));
                camera_->revolveAroundPointAxis(angle,  camera_->revolveAroundPoint(), glm::normalize(axis));
            }
        }
    }
    mouseX_ = e->x();
    mouseY_ = e->y();

}

void CameraController::mousePressEvent(QMouseEvent *e){
    if ( e->button() & Qt::LeftButton ){
        if (e->modifiers() & Qt::ALT)
            action_ = ROTATE;
        else
            action_ = REVOLVE;
    }

    else if ( e->button() & Qt::MiddleButton ){
        action_ = FORWARD;
    }
    else if ( e->button() & Qt::RightButton ){
        action_ = TRANSLATE;
    } else {
        std::cerr << "Camera : unknown button" << std::endl;
    }

    mouseX_ = e->x();
    mouseY_ = e->y();
    isClick_ = true;
}

void CameraController::mouseReleaseEvent(QMouseEvent *e){
    isClick_ = false;
}

void CameraController::wheelEvent(QWheelEvent *e)
{
    camera_->moveForward(e->angleDelta().y()/120.f);
}
float CameraController::rotationSensitivity() const
{
    return rotationSensitivity_;
}

void CameraController::setRotationSensitivity(float rotationSensitivity)
{
    rotationSensitivity_ = rotationSensitivity;
}
