/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef _LIGHT_H
#define _LIGHT_H

#include <iostream>
#include <string>

#include "opengl.h"

#include "bindable.h"
#include "assimp/light.h"

namespace vortex {

/**
 * Phong directionnal/sun light representation class
 *
 */
class Light {
public:
    /**
     * ASSIMP light transformation constructor.
     *
     * @param light The ASSIMP light to be transform to Light.
     */
    Light(aiLight *light);

    /**
     * Constructor : initialize Phong attributes
     *
     * @param name Light Object name.
     * @param position For sun lights indicates the position.
     * @param direction For directionnal lights indicates the direction of emission.
     * @param ambient Phong ambient component of the light.
     * @param diffuse Phong diffuse component of the light.
     * @param specular Phong specular component of the light.
     *
     * Nul vector as "position" value indicates the light is a directionnal one.
     * Nul vector as "direction" value indicates the light is a sun one.
     */
    Light(std::string name, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

    /**
     * Copy constructor : copy light attributes, transforming its position and direction using "transform"
     *
     * @param light The Light to be copied.
     * @param transform The transformation matrix to be applied to "light" position and direction.
     */
    Light(const Light &light, glm::mat4x4 transform);

    /**
     * Default constructor : initialize all Light attributes to 0.
     *
     */
    Light();
    ~Light();


    void bind(GLuint shaderProgramId) const;

    void printDebug(){
        using vortex::util::operator <<;
        std::cerr << "light " << mName << " pos " << mPosition << " dir " << mDirection << " diff " << mDiffuse;
        std::cerr << " Outer " << mAngleOuterCone << " Inner " << mAngleInnerCone << std::endl;
    }

    bool mIsSun;

    std::string mName;
    glm::vec3 mPosition;
    glm::vec3 mDirection;
    glm::vec3 mAmbient;
    glm::vec3 mDiffuse;
    glm::vec3 mSpecular;
    glm::mat4x4 mTransform;
    glm::mat4x4 mShadowMatrix;

    float mAngleOuterCone;
    float mAngleInnerCone;
    float mAttenuationConstant;
    float mAttenuationLinear;
    float mAttenuationQuadratic;
};

}


#endif
