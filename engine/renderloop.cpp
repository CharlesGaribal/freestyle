/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "renderloop.h"

namespace vortex {


//-------------------------------------------------------------------------------
// MaterialState
//-------------------------------------------------------------------------------


MaterialState:: MaterialState(Material *mat, ShaderProgram *shaderId) : Bindable(), mMaterial(mat), mShader(shaderId) {}


void MaterialState::bind() const{ // to be completed
    if (mMaterial) {
        bind(mMaterial, mShader);
    }

}

void MaterialState::bind(Material *mat, ShaderProgram *shader) {

    glAssert(glUniform1f(glGetUniformLocation(shader->id() , "opacityLevel"), (-1.f)));

    Texture * tex = mat->getTexture(Material::TEXTURE_DIFFUSE);
    if (tex) {
        shader->setUniformTexture("map_diffuse", tex);
    }
    tex = mat->getTexture(Material::TEXTURE_HEIGHT);
    if (tex) {
        shader->setUniformTexture("map_normals", tex);
    }
    tex = mat->getTexture(Material::TEXTURE_NORMALS);
    if (tex) {
        shader->setUniformTexture("map_normals", tex);
    }
    tex = mat->getTexture(Material::TEXTURE_SPECULAR);
    if (tex) {
        shader->setUniformTexture("map_specular", tex);
    }
    tex = mat->getTexture(Material::TEXTURE_AMBIENT);
    if (tex) {
        shader->setUniformTexture("map_ambient", tex);
    }
    tex = mat->getTexture(Material::TEXTURE_OPACITY);
    if (tex) {
        shader->setUniform("opacityLevel", 0.01f);
        shader->setUniformTexture("map_opacity", tex);
    }

    shader->setUniform("Kd", mat->getDiffuseColor());
    shader->setUniform("Ka", mat->getAmbientColor());
    shader->setUniform("Ks", mat->getSpecularColor());
    shader->setUniform("Ns", mat->getShininess());


}

bool MaterialState::operator< (const MaterialState &other) const {
    // todo find a better comparison
    return (mMaterial->getName() < other.mMaterial->getName());
}



//-------------------------------------------------------------------------------
// TransformState
//-------------------------------------------------------------------------------

TransformState::TransformState(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix, ShaderProgram *prog) : Bindable(),
        mModelView(modelviewMatrix), mProjection(projectionMatrix), mProgram(prog) {
        mMVP = mProjection * mModelView;
        mNormal = glm::transpose(glm::inverse(mModelView));
    }

void TransformState::bind() const {

    /**********     METHOD WITH UBO     **********/
    mProgram->setTransform(mModelView, mProjection, mMVP, mNormal);
    /*********************************************/

}

void TransformState::activate(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix) const {

    glm::mat4x4 mv = modelviewMatrix*mModelView;
    glm::mat4x4 mvp = projectionMatrix * mv;
    glm::mat4x4 mnormal = glm::transpose(glm::inverse(mv));
    /**********     METHOD WITH UBO     **********/
    mProgram->setTransform(mv, projectionMatrix, mvp, mnormal);
    /*********************************************/

}

bool TransformState::operator< (const TransformState &other) const {
    // todo find a better comparison
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (mModelView[i][j] < other.mModelView[i][j]) {
                return true;
            } else if (mModelView[i][j] > other.mModelView[i][j]) {
                return false;
            }
        }
    }
    return false;
}

// Added for lazy transform

template<>
void DrawableMap<TransformState, MaterialLoop>::draw(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix) {
    for (typename DrawableMap< TransformState, MaterialLoop >::iterator it = this->begin(); it != this->end(); ++it) {
        it->first.activate(modelviewMatrix, projectionMatrix);
        it->second.draw();
    }
}

template<>
void DrawableMap<TransformState, DrawableVector<Mesh::MeshPtr> >::draw(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix) {
    for (typename DrawableMap< TransformState, DrawableVector<Mesh::MeshPtr> >::iterator it = this->begin(); it != this->end(); ++it) {
        glCheckError();
        it->first.activate(modelviewMatrix, projectionMatrix);
        it->second.draw();
    }
}


} // vortex


