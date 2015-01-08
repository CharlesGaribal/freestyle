/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "renderstate.h"
#include "assetmanager.h"

namespace vortex {

/**
 * @todo optimize this using a uniform class ... read Uniform Buffer Object specificatio
 */
void RenderState::bind(const glm::mat4x4 &modelviewmatrix, const glm::mat4x4 &projectionmatrix, const glm::mat4x4 &modelviewprojectionmatrix, const glm::mat4x4 &normalMatrix)
{
    glAssert(glUseProgram(mShaderProgram->id()));
    GLint loc;
    glAssert(loc = glGetUniformLocation(mShaderProgram->id(), "modelViewMatrix"));
    glAssert(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(modelviewmatrix)));

    glAssert(glUniformMatrix4fv(glGetUniformLocation(mShaderProgram->id(), "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionmatrix)));
    glAssert(glUniformMatrix4fv(glGetUniformLocation(mShaderProgram->id(), "MVP"), 1, GL_FALSE, glm::value_ptr(modelviewprojectionmatrix)));
    glAssert(glUniformMatrix4fv(glGetUniformLocation(mShaderProgram->id(), "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix)));

    if (mMaterial) {
        Texture * tex = mMaterial->getTexture(Material::TEXTURE_DIFFUSE);
        if (tex) {
            tex->bind(GL_TEXTURE0);
            glAssert(glUniform1i(glGetUniformLocation(mShaderProgram->id(), "map_diffuse"), 0));
        }
    }
}

/**
 * @todo do we realy need to do something here ?
 */
void RenderState::unbind()
{
}


}
