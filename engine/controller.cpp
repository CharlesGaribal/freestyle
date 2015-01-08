/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#if 0
#include "controller.h"

namespace vortex{

template<> Quaternion QuaternionController::computeTangent(const Quaternion &prev, const Quaternion &current, const Quaternion &next){
    return Quaternion::squadTangent(prev, current, next);
}

template<> void QuaternionController::updateCatmullRom(double time){
    updateTangent();
    if(time <= mTimeKeys.front()){ *mValue = mValueKeys.front(); return; }

    if(time >= mTimeKeys.back()){  *mValue = mValueKeys.back();  return; }

    TimeKeyType::iterator timeKeyItr = mTimeKeys.begin(), prevTimeKeyItr;
    /*typename*/ ValueKeyType::iterator valueKeyItr = mValueKeys.begin(), prevValueKeyItr;
    /*typename*/ ValueKeyType::iterator tangentValueKeyItr = mTangentValueKeys.begin(), prevTangentValueKeyItr;
    while(time > *timeKeyItr){
        prevTimeKeyItr = timeKeyItr;   timeKeyItr++;
        prevValueKeyItr = valueKeyItr; valueKeyItr++;
        prevTangentValueKeyItr = tangentValueKeyItr; tangentValueKeyItr++;
    }

    float dt = (*timeKeyItr-*prevTimeKeyItr);
    float t = (time-*prevTimeKeyItr)/dt;

    *mValue = Quaternion::squad(*prevValueKeyItr, *prevTangentValueKeyItr, *tangentValueKeyItr, *valueKeyItr, t);
}

template<> void QuaternionController::updateLinear(double time){
        if(time <= mTimeKeys.front()){ *mValue = mValueKeys.front(); return; }
        if(time >= mTimeKeys.back()){  *mValue = mValueKeys.back();  return; }
        TimeKeyType::iterator timeKeyItr = mTimeKeys.begin(), prevTimeKeyItr;
        /*typename*/ ValueKeyType::iterator valueKeyItr = mValueKeys.begin(), prevValueKeyItr;
        while(time > *timeKeyItr){
            prevTimeKeyItr = timeKeyItr;   timeKeyItr++;
            prevValueKeyItr = valueKeyItr; valueKeyItr++;
        }

        float t = (*timeKeyItr-time)/(*timeKeyItr-*prevTimeKeyItr);
        *mValue = Quaternion::slerp( (*prevValueKeyItr), (*valueKeyItr), 1.0-t);
}


}
#endif
