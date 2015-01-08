/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>

#include "opengl.h"

#include "bbox.h"

#include <glm/gtx/quaternion.hpp>
#include "assimp/camera.h"

namespace vortex {
using namespace util;

class Camera{
    void updateNFplanes();

public:
    Camera();
    Camera(aiCamera *camera);
    Camera(const Camera &other);

    void setScreenWidthAndHeight(int w, int h){width_ = w; height_ = h;}
    void computeModelViewMatrix();

    void computeProjectionMatrix() {
        updateNFplanes();
        projectionMatrix_ = glm::perspective(fieldOfView(), aspectRatio(), zNear(), zFar());
    }

    glm::mat4 getModelViewMatrix() const {return modelViewMatrix_;}
    glm::mat4 getProjectionMatrix() const {return projectionMatrix_;}

    void setSceneBoundingBox(glm::vec3 min, glm::vec3 max){
        min_ = min;
        max_ = max;
        setSceneCenter(.5f*(min_+max_));
        setSceneRadius(glm::length(.5f*(max_-min_)));
    }

    void showEntireScene();

    glm::vec3 viewDirection() const {
        return frame_.inverseTransformVec(glm::vec3(0,0,-1));
    }
    glm::vec3 upVector() const {
        return frame_.inverseTransformVec(glm::vec3(0,1,0));
    }

    glm::vec3 rightVector() const {
        return frame_.inverseTransformVec(glm::vec3(1,0,0));
    }

    const glm::vec3 & position() const {
        return frame_.translation();
    }

    void moveRight(float d){
        frame_.setTranslation(frame_.translation()+d*rightVector());
    }

    void moveUp(float d){
        frame_.setTranslation(frame_.translation()+d*upVector());
    }

    void moveForward(float d){
        frame_.setTranslation(frame_.translation()+d*viewDirection());
    }

    void rotateAroundRight(float d){
        ///@todo corrigé la dérive du Right ... est-ce vraiment autour de 1 0 0 qu'il faut tourner ?
        frame_.setOrientation(glm::rotate(frame_.orientation(), d, glm::vec3(1.f,0.f,0.f)));
    }

    void rotateAroundUp(float d){
        ///@todo corrigé la dérive du up ... est-ce vraiment autour de 0 1 0 qu'il faut tourner ?
        frame_.setOrientation(glm::rotate(frame_.orientation(), d, glm::vec3(0.f,1.f,0.f)));
    }

    void rotateAroundView(float d){
        frame_.setOrientation(glm::rotate(frame_.orientation(), d, glm::vec3(0.f,0.f,1.f)));
    }

    void revolveAroundPointAxis(float d, glm::vec3 revolvePoint, glm::vec3 revolveAxis);

    float zNear() const {return zNear_;}
    float zFar() const {return zFar_; }
    float fieldOfView() const {return fieldOfView_;}
    float aspectRatio(){return float(width_)/float(height_);}
    glm::vec3 project(glm::vec3 p);
    glm::vec3 revolveAroundPoint() const;
    void setRevolveAroundPoint(const glm::vec3 &revolveAroundPoint);

    const glm::vec3 & translation() const {
        return frame_.translation();
    }
    void setTranslation(const glm::vec3 & t){
        frame_.setTranslation(t);
    }

    const glm::quat & orientation() const {return frame_.orientation();}
    void setOrientation(const glm::quat &o){ frame_.setOrientation(glm::normalize(o));}

    friend std::ostream & operator<<(std::ostream &os, const Camera &c);

    class NewFrame{
    public:
       NewFrame(){
            translation_ = glm::vec3(0.f);
            orientation_ = glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f));
        }

        // world to frame
        glm::vec3 transform(glm::vec3 p) const {
            return glm::inverse(orientation()) * (p - translation());
        }

        // frame to world
        glm::vec3 inverseTransform(glm::vec3 p) const {

            return orientation()*p + translation();
        }

        glm::vec3 transformVec(glm::vec3 p) const {
            return glm::inverse(orientation())*p;
        }

        glm::vec3 inverseTransformVec(glm::vec3 p) const {
            return orientation()*p;
        }

        const glm::vec3 & translation() const {
            return translation_;
        }
        void setTranslation(const glm::vec3 & t){
            translation_ = t;
        }

        const glm::quat & orientation() const {return orientation_;}
        void setOrientation(const glm::quat &o){ orientation_ = glm::normalize(o);}

    protected:

        //! world pos of the frame
        glm::vec3 translation_;

        //! from frame to world
        glm::quat orientation_;
    };
    int width() const;
    void setWidth(int width);

    int height() const;
    void setHeight(int height);

    glm::vec3 sceneCenter() const;
    void setSceneCenter(const glm::vec3 &sceneCenter);

    float sceneRadius() const;
    void setSceneRadius(float sceneRadius);

protected:
    NewFrame frame_;
    int width_;
    int height_;
    float zNear_;
    float zFar_;
    float fieldOfView_;
    glm::mat4 projectionMatrix_;
    glm::mat4 modelViewMatrix_;
    glm::vec3 min_;
    glm::vec3 max_;
    glm::vec3 sceneCenter_;
    float sceneRadius_;
    glm::vec3 revolveAroundPoint_;

};


} // namespace vortex

#endif // CAMERA_H
