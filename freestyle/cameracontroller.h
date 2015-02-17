#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <../engine/camera.h>
#include <QMouseEvent>

namespace vortex{
namespace ui{
class CameraController{
public:
    CameraController(vortex::Camera *cam){
        sensitivity_ = 0.1f;
        rotationSensitivity_ = 1.f;
        camera_ = cam;
    }

    void mouseMoveEvent(QMouseEvent *e);

    void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

    void wheelEvent(QWheelEvent *e);

    float rotationSensitivity() const;
    void setRotationSensitivity(float rotationSensitivity);

protected:
    vortex::Camera *camera_;
    enum CameraAction{NONE, TRANSLATE, FORWARD, ROTATE, REVOLVE};
    CameraAction action_;
    bool isClick_;
    int mouseX_;
    int mouseY_;
    float sensitivity_;
    float rotationSensitivity_;
};
}
}

#endif
