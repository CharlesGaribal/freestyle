/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "light.h"

namespace vortex {

Light::Light(aiLight *light){
    mName = std::string(light->mName.data);
    mPosition = glm::vec3(light->mPosition[0], light->mPosition[1], light->mPosition[2]);
    mDirection = glm::vec3(light->mDirection[0], light->mDirection[1], light->mDirection[2]);
    mAmbient = glm::vec3(light->mColorAmbient[0], light->mColorAmbient[1], light->mColorAmbient[2]);
    mDiffuse = glm::vec3(light->mColorDiffuse[0], light->mColorDiffuse[1], light->mColorDiffuse[2]);
    mSpecular = glm::vec3(light->mColorSpecular[0], light->mColorSpecular[1], light->mColorSpecular[2]);
    mAngleInnerCone= light->mAngleInnerCone*0.5; // light->mAngleInnerCone is the total angle
    mAngleOuterCone = light->mAngleOuterCone*0.5;
}

Light::Light(std::string name, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular):
    mName(name), mPosition(position), mDirection(direction), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular)
{
    mIsSun = (mPosition == glm::vec3(0.));
    mAngleOuterCone = 2.*M_PI;
    mAngleInnerCone = 2.*M_PI;
}

Light::Light(const Light &light, glm::mat4x4 transform)
{
    mName = light.mName;
    mAmbient = light.mAmbient;
    mDiffuse = light.mDiffuse;
    mSpecular = light.mSpecular;
    glm::vec4 vec = transform * glm::vec4(light.mPosition, 1.);
    mPosition = glm::vec3(vec[0], vec[1], vec[2]);
    vec = vortex::util::inverse(glm::transpose(transform)) * glm::vec4(light.mDirection, 0.);
    mDirection = glm::vec3(vec[0], vec[1], vec[2]);
    mShadowMatrix = light.mShadowMatrix;
    mAngleOuterCone = light.mAngleOuterCone;
    mAngleInnerCone = light.mAngleInnerCone;
}

Light::Light()
{
    mName = "default";
    mPosition = glm::vec3(0.);
    mDirection = glm::vec3(0.,0., -1.);
    mAmbient = glm::vec3(1.);
    mDiffuse = glm::vec3(1.);
    mSpecular = glm::vec3(1.);
    mAngleOuterCone = 2.*M_PI;
    mAngleInnerCone = 2.*M_PI;
    mTransform = glm::mat4(1.0f);
    mShadowMatrix = mTransform;

}

Light::~Light()
{
}

void Light::bind(GLuint shaderProgramId) const
{
    glAssert(glUniform3f(glGetUniformLocation(shaderProgramId, "uniLightPosition"), mPosition[0], mPosition[1], mPosition[2]));
    glAssert(glUniform3f(glGetUniformLocation (shaderProgramId, "uniLightDirection"), mDirection[0], mDirection[1], mDirection[2] ));
    glAssert(glUniform3f(glGetUniformLocation(shaderProgramId, "uniLightAmbient"), mAmbient[0], mAmbient[1], mAmbient[2]));
    glAssert(glUniform3f(glGetUniformLocation(shaderProgramId, "uniLightDiffuse"), mDiffuse[0], mDiffuse[1], mDiffuse[2]));
    glAssert(glUniform3f(glGetUniformLocation(shaderProgramId, "uniLightSpecular"), mSpecular[0], mSpecular[1], mSpecular[2]));
    glAssert(glUniform1f(glGetUniformLocation(shaderProgramId, "uniLightAngleInnerCone"), mAngleInnerCone));
    glAssert(glUniform1f(glGetUniformLocation(shaderProgramId, "uniLightAngleOuterCone"), mAngleOuterCone));
}

}
