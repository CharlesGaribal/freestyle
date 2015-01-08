/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef BBOX_H
#define BBOX_H
#include <iostream>
#include <vector>

#include "opengl.h"

namespace vortex {

/**
 * Bounding Box representation class
 *
 */
class BBox {
public:
    /**
     * Default constructor : initialize BBox dimensions
     *
     */
    BBox() : mMin(HUGE, HUGE, HUGE), mMax(-HUGE, -HUGE, -HUGE), empty(true) {
    }

    /**
     * Copy constructor : copy "from" dimensions
     *
     * @param from Const reference to the BBox to be copied
     */
    BBox(const BBox &from) : mMin(from.mMin), mMax(from.mMax), empty(from.empty) {
    }

    /**
     * Update Bbox dimensions in order to include "pt"
     *
     * @param pt 3D point position to be included in the BBox
     */
    BBox & operator += (glm::vec3 pt) {
        empty=false;
        mMin = glm::min(mMin, pt);
        mMax = glm::max(mMax, pt);
        return *this;
    }

    /**
     * Update BBox dimensions in order to include "box"
     *
     * @param box BBox to be included in this one
     */
    BBox & operator += (const BBox box) {
        empty = empty && box.empty;
        mMin = glm::min(mMin, box.mMin);
        mMax = glm::max(mMax, box.mMax);
        return *this;
    }

    /**
     * Update BBox dimensions in order to have them transformed by "matrix"
     *
     * @param matrix Transformation matrix to apply to BBox dimensions
     */
    BBox operator *(const glm::mat4x4 &matrix) const {
        BBox r(*this);
        glm::vec4 minTransformed;
        glm::vec4 maxTransformed;

        r.mMax = glm::vec3(matrix * glm::vec4(mMax, 1.0));
        r.mMin = glm::vec3(matrix * glm::vec4(mMin, 1.0));
        r.empty = empty;
        return r;
    }

    /**
     * Define how to print BBox data
     *
     */
    friend std::ostream & operator << (std::ostream &out, const BBox &box) {
        out << "min(" << box.mMin.x << ", " << box.mMin.y << ", " << box.mMin.z << ") -- MAX(" << box.mMax.x << ", " << box.mMax.y << ", " << box.mMax.z << ") ";
        return out;
    }

    /**
     * Return BBox minimal coordinates on x, y and z axes
     *
     */
    glm::vec3 getMin() const {
        return mMin;
    }

    /**
     * Return BBox maximal coordinates on x, y and z axes
     *
     */
    glm::vec3 getMax() const{
        return mMax;
    }

    glm::vec3 getExtend() const {
        return mMax-mMin;
    }

    bool isEmpty() const {
        return empty;
    }

    /**
        * Update BBox dimensions in order to have them transformed by "matrix"
        * this version is conservative since it computes a new AABB containing
        * the rotated old BBox.
        * @param matrix Transformation matrix to apply to BBox dimensions
        */
       BBox getTransformedBBox(const glm::mat4x4 &matrix) const {
           BBox r;

           if(!this->empty){
               std::vector<glm::vec3> corners;
               this->getCorners(corners);
               //Transform the corners and update the new bbox
               for(std::vector<glm::vec3>::iterator it = corners.begin();
                   it != corners.end(); ++it){
                   (*it) = glm::vec3(matrix * glm::vec4((*it),1.0f));
                   r += (*it);
               }
           }

           return r;
       }

      void getCorners(glm::vec3& mx_my_mz,glm::vec3& mx_my_Mz,
                       glm::vec3& mx_My_mz,glm::vec3& Mx_my_mz,
                       glm::vec3& Mx_My_mz,glm::vec3& Mx_my_Mz,
                       glm::vec3& mx_My_Mz,glm::vec3& Mx_My_Mz) const{
           mx_my_mz = glm::vec3(mMin.x,mMin.y,mMin.z);
           mx_my_Mz = glm::vec3(mMin.x,mMin.y,mMax.z);
           mx_My_mz = glm::vec3(mMin.x,mMax.y,mMin.z);
           Mx_my_mz = glm::vec3(mMax.x,mMin.y,mMin.z);
           Mx_My_mz = glm::vec3(mMax.x,mMax.y,mMin.z);
           Mx_my_Mz = glm::vec3(mMax.x,mMin.y,mMax.z);
           mx_My_Mz = glm::vec3(mMin.x,mMax.y,mMax.z);
           Mx_My_Mz = glm::vec3(mMax.x,mMax.y,mMax.z);
       }

       void getCorners(std::vector<glm::vec3>& theCorners) const{
           theCorners.push_back(glm::vec3(mMin.x,mMin.y,mMin.z));
           theCorners.push_back(glm::vec3(mMin.x,mMin.y,mMax.z));
           theCorners.push_back(glm::vec3(mMin.x,mMax.y,mMin.z));
           theCorners.push_back(glm::vec3(mMax.x,mMin.y,mMin.z));
           theCorners.push_back(glm::vec3(mMax.x,mMax.y,mMin.z));
           theCorners.push_back(glm::vec3(mMax.x,mMin.y,mMax.z));
           theCorners.push_back(glm::vec3(mMin.x,mMax.y,mMax.z));
           theCorners.push_back(glm::vec3(mMax.x,mMax.y,mMax.z));
       }

private:
    glm::vec3 mMin;
    glm::vec3 mMax;
    bool empty;
};

} // namespace vortex
#endif // BBOX_H
