/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#if 0
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "opengl.h"

#include "quaternion.h"
#include "camera.h"

namespace vortex{

template <class value_type> class Controller {
public:
    typedef std::vector<value_type> ValueKeyType;
    typedef std::vector<double> TimeKeyType;

    Controller(){ mValue = &mFakeControlled; }
    virtual ~Controller(){  }
    void addKey(double time, value_type value){
        mValueKeys.push_back(value);
        mTimeKeys.push_back(time);
    }

    virtual value_type computeTangent(const value_type &prev, const value_type &current, const value_type &next){
        return .5f*(next-prev);
    }

    void updateTangent(){
        mTangentValueKeys.clear();
        size_t size = mValueKeys.size();
        if(1== size) return;

        mTangentValueKeys.push_back(computeTangent(mValueKeys[0], mValueKeys[0], mValueKeys[1]));
        for(size_t i=1; i<size-1; ++i){
            mTangentValueKeys.push_back(computeTangent(mValueKeys[i-1], mValueKeys[i], mValueKeys[i+1]));
        }
        mTangentValueKeys.push_back(computeTangent(mValueKeys[size-2], mValueKeys[size-2], mValueKeys[size-1]));
        assert(size == mTangentValueKeys.size());
    }

    value_type value() { return *mValue; }


    void updateCatmullRom(double time){
        updateTangent();
        if(time <= mTimeKeys.front()){ *mValue = mValueKeys.front(); return; }

        if(time >= mTimeKeys.back()){  *mValue = mValueKeys.back();  return; }

        TimeKeyType::iterator timeKeyItr = mTimeKeys.begin(), prevTimeKeyItr;
        typename ValueKeyType::iterator valueKeyItr = mValueKeys.begin(), prevValueKeyItr;
        typename ValueKeyType::iterator tangentValueKeyItr = mTangentValueKeys.begin(), prevTangentValueKeyItr;
        while(time > *timeKeyItr){
            prevTimeKeyItr = timeKeyItr;   timeKeyItr++;
            prevValueKeyItr = valueKeyItr; valueKeyItr++;
            prevTangentValueKeyItr = tangentValueKeyItr; tangentValueKeyItr++;
        }

        float dt = (*timeKeyItr-*prevTimeKeyItr);
        float t = (time-*prevTimeKeyItr)/dt;

        *mValue = (2.f*t*t*t-3.f*t*t+1.f)* *prevValueKeyItr + (t*t*t-2.f*t*t+t)* *prevTangentValueKeyItr
                +(-2.f*t*t*t+3.f*t*t)* *valueKeyItr + (t*t*t-t*t)* *tangentValueKeyItr;
    }

    void updateLinear(double time){
        if(time <= mTimeKeys.front()){ *mValue = mValueKeys.front(); return; }

        if(time >= mTimeKeys.back()){  *mValue = mValueKeys.back();  return; }

        TimeKeyType::iterator timeKeyItr = mTimeKeys.begin(), prevTimeKeyItr;
        typename ValueKeyType::iterator valueKeyItr = mValueKeys.begin(), prevValueKeyItr;
        while(time > *timeKeyItr){
            prevTimeKeyItr = timeKeyItr;   timeKeyItr++;
            prevValueKeyItr = valueKeyItr; valueKeyItr++;
        }

        float t = (time-*prevTimeKeyItr)/(*timeKeyItr-*prevTimeKeyItr);
        *mValue = t*(*prevValueKeyItr) + (1.0f-t)*(*valueKeyItr);
    }

    void update(double time){
        updateCatmullRom(time);
     //     updateLinear(time);
    }


protected:
    value_type *mValue;
    value_type mFakeControlled;
    ValueKeyType mValueKeys;
    ValueKeyType mTangentValueKeys;
    TimeKeyType mTimeKeys;

};

typedef Controller<glm::vec3> Vec3Controller;
typedef Controller<Quaternion> QuaternionController;

template<> Quaternion QuaternionController::computeTangent(const Quaternion &prev, const Quaternion &current, const Quaternion &next);
template<> void QuaternionController::updateCatmullRom(double time);
template<> void QuaternionController::updateLinear(double time);

//template <> Quaternion  Controller<Quaternion>::computeTangent(const Quaternion &prev, const Quaternion &current, const Quaternion &next){}

class CameraController{


public:
    CameraController(Camera *cam):mCam(cam){
    }

    void addKey(double time, glm::vec3 pos, Quaternion orientation){
        mPositionController.addKey(time, pos);
        mOrientationController.addKey(time, orientation);
    }

    void update(double time){
        mPositionController.update(time);
        mOrientationController.update(time);
        mCam->setTranslation(mPositionController.value());
        mCam->setOrientation(glm::quat(mOrientationController.value()[0], mOrientationController.value()[1], mOrientationController.value()[2], mOrientationController.value()[3]));
    }

protected:
    Camera *mCam;
    Vec3Controller mPositionController;
    QuaternionController mOrientationController;
};

}
#endif // CONTROLLER_H
#endif
