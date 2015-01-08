/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

/**
   OpenGL library and implementation management
 */

#ifndef OPENGL_H
#define OPENGL_H

#define GLM_FORCE_RADIANS

#ifdef __APPLE__

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#define __gl_h_

#else

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#endif

//#define GLM_MESSAGES
#include "glm/glm.hpp"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glassert.h"
#include "glsave.h"

#include <ostream>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace vortex {

/*! Projects the Vec on the axis of direction \p direction that passes through the origin.

\p direction does not need to be normalized (but must be non null). */
inline void projectOnAxis(glm::vec3 &v, const glm::vec3& direction) {
  v = glm::dot(v, direction) / glm::dot(direction, direction) * direction;
}

/*! Projects the Vec on the plane whose normal is \p normal that passes through the origin.

\p normal does not need to be normalized (but must be non null). */
inline void projectOnPlane(glm::vec3 &v, const glm::vec3& normal) {
  v -= glm::dot(v, normal) / glm::dot(normal, normal) * normal;
}

namespace util {

inline std::ostream &operator<<(std::ostream &stream, const glm::mat4x4 &m)
{
    stream << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << std::endl;
    stream << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << std::endl;
    stream << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << std::endl;
    stream << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << std::endl;
    return stream;
}


inline std::ostream &operator<<(std::ostream &stream, const glm::vec3 &v)
{
    stream << v[0] << " " << v[1] << " " << v[2];
    return stream;
}

inline std::ostream &operator<<(std::ostream &stream, const glm::vec4 &v)
{
    stream << v[0] << " " << v[1] << " " << v[2] << " " << v[3];
    return stream;
}

inline std::ostream &operator<<(std::ostream &stream, const glm::quat &q)
{
    stream << q[0] << " " << q[1] << " " << q[2] << " " << q[3];
    return stream;
}

inline std::ostream &operator<<(std::ostream &stream, const glm::ivec3 &v)
{
    stream << v[0] << " " << v[1] << " " << v[2];
    return stream;
}

inline void printMatrix(std::ostream &stream, const glm::mat4x4 &m)
{
    stream << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << std::endl;
    stream << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << std::endl;
    stream << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << std::endl;
    stream << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << std::endl;
}


// length


inline float sqlength
(
    const glm::vec3 & v
)
{

    float sqr = v.x * v.x + v.y * v.y + v.z * v.z;
    return sqr;
}

inline glm::vec3 perpendicular (const glm::vec3 & v) {
                static const float fSquareZero = 1e-06 * 1e-06;
                glm::vec3 perp = glm::cross(v, glm::vec3(1,0,0));

                if (sqlength(perp)< fSquareZero) {
                        /* This vector is the Y axis multiplied by a scalar, so we have
                           to use another axis.
                        */
                    perp = glm::cross(v, glm::vec3(0,1,0));
                }

                return perp;
}

// from h3
inline glm::mat4x4 inverse(const glm::mat4x4 direct)
{
    float data[16];

    float m00 = glm::value_ptr(direct) [0];
    float m01 = glm::value_ptr(direct) [1];
    float m02 = glm::value_ptr(direct) [2];
    float m03 = glm::value_ptr(direct) [3];
    float m10 = glm::value_ptr(direct) [4];
    float m11 = glm::value_ptr(direct) [5];
    float m12 = glm::value_ptr(direct) [6];
    float m13 = glm::value_ptr(direct) [7];
    float m20 = glm::value_ptr(direct) [8];
    float m21 = glm::value_ptr(direct) [9];
    float m22 = glm::value_ptr(direct) [10];
    float m23 = glm::value_ptr(direct) [11];
    float m30 = glm::value_ptr(direct) [12];
    float m31 = glm::value_ptr(direct) [13];
    float m32 = glm::value_ptr(direct) [14];
    float m33 = glm::value_ptr(direct) [15];

    float determinant =
    m03 * m12 * m21 * m30-m02 * m13 * m21 * m30-m03 * m11 * m22 * m30+m01 * m13    * m22 * m30+
    m02 * m11 * m23 * m30-m01 * m12 * m23 * m30-m03 * m12 * m20 * m31+m02 * m13    * m20 * m31+
    m03 * m10 * m22 * m31-m00 * m13 * m22 * m31-m02 * m10 * m23 * m31+m00 * m12    * m23 * m31+
    m03 * m11 * m20 * m32-m01 * m13 * m20 * m32-m03 * m10 * m21 * m32+m00 * m13    * m21 * m32+
    m01 * m10 * m23 * m32-m00 * m11 * m23 * m32-m02 * m11 * m20 * m33+m01 * m12    * m20 * m33+
    m02 * m10 * m21 * m33-m00 * m12 * m21 * m33-m01 * m10 * m22 * m33+m00 * m11    * m22 * m33;

    data[0] = (m12*m23*m31 - m13*m22*m31 + m13*m21*m32 - m11*m23*m32 - m12*m21*m33 + m11*m22*m33)/determinant;
    data[1] = (m03*m22*m31 - m02*m23*m31 - m03*m21*m32 + m01*m23*m32 + m02*m21*m33 - m01*m22*m33)/determinant;
    data[2] = (m02*m13*m31 - m03*m12*m31 + m03*m11*m32 - m01*m13*m32 - m02*m11*m33 + m01*m12*m33)/determinant;
    data[3] = (m03*m12*m21 - m02*m13*m21 - m03*m11*m22 + m01*m13*m22 + m02*m11*m23 - m01*m12*m23)/determinant;
    data[4] = (m13*m22*m30 - m12*m23*m30 - m13*m20*m32 + m10*m23*m32 + m12*m20*m33 - m10*m22*m33)/determinant;
    data[5] = (m02*m23*m30 - m03*m22*m30 + m03*m20*m32 - m00*m23*m32 - m02*m20*m33 + m00*m22*m33)/determinant;
    data[6] = (m03*m12*m30 - m02*m13*m30 - m03*m10*m32 + m00*m13*m32 + m02*m10*m33 - m00*m12*m33)/determinant;
    data[7] = (m02*m13*m20 - m03*m12*m20 + m03*m10*m22 - m00*m13*m22 - m02*m10*m23 + m00*m12*m23)/determinant;
    data[8] = (m11*m23*m30 - m13*m21*m30 + m13*m20*m31 - m10*m23*m31 - m11*m20*m33 + m10*m21*m33)/determinant;
    data[9] = (m03*m21*m30 - m01*m23*m30 - m03*m20*m31 + m00*m23*m31 + m01*m20*m33 - m00*m21*m33)/determinant;
    data[10] = (m01*m13*m30 - m03*m11*m30 + m03*m10*m31 - m00*m13*m31 - m01*m10*m33 + m00*m11*m33)/determinant;
    data[11] = (m03*m11*m20 - m01*m13*m20 - m03*m10*m21 + m00*m13*m21 + m01*m10*m23 - m00*m11*m23)/determinant;
    data[12] = (m12*m21*m30 - m11*m22*m30 - m12*m20*m31 + m10*m22*m31 + m11*m20*m32 - m10*m21*m32)/determinant;
    data[13] = (m01*m22*m30 - m02*m21*m30 + m02*m20*m31 - m00*m22*m31 - m01*m20*m32 + m00*m21*m32)/determinant;
    data[14] = (m02*m11*m30 - m01*m12*m30 - m02*m10*m31 + m00*m12*m31 + m01*m10*m32 - m00*m11*m32)/determinant;
    data[15] = (m01*m12*m20 - m02*m11*m20 + m02*m10*m21 - m00*m12*m21 - m01*m10*m22 + m00*m11*m22)/determinant;

    return glm::mat4x4(data[0], data[1], data[2], data[3],
                       data[4], data[5], data[6], data[7],
                       data[8], data[9], data[10], data[11],
                       data[12], data[13], data[14], data[15]);

}

}
}

#endif
