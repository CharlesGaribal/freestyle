/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include <iostream>

#include "fbo.h"



namespace vortex {

FBO *FBO::mCurrentlyBound = NULL;

//--------------------------------------------------
FBO::FBO(Components comp, int width, int height) : mId(0), mComponents(comp), mWidth((width ? width : 1)), mHeight((height ? height : 1)), mIsBound(false)
{
    glAssert(glGenFramebuffers(1, &mId));
}

//--------------------------------------------------
FBO::~FBO()
{
    deleteGL();
    for (std::map< GLenum, RBO * >::iterator it = mRbos.begin(); it != mRbos.end(); ++it) {
        delete it->second;
    }

    if(this == mCurrentlyBound) mCurrentlyBound = NULL;
}

//--------------------------------------------------
void FBO::bind()
{
    if(mCurrentlyBound) mCurrentlyBound->unbind();
    glAssert(glBindFramebuffer(GL_FRAMEBUFFER, mId));
    mIsBound = true;
    mCurrentlyBound = this;
}

//--------------------------------------------------
void FBO::unbind()
{
    assert(mCurrentlyBound == this);
    if(this == mCurrentlyBound) mCurrentlyBound = NULL;
    mIsBound = false;
}

//--------------------------------------------------
void FBO::deleteGL()
{
    // need to check if bound ?
    glAssert(glDeleteFramebuffers(1, &mId));
}

//--------------------------------------------------
bool FBO::check() const
{
    if (!mIsBound) {
        std::cerr << "Error : trying to check an unbound FBO (FBO not checked)" << std::endl;
        return false;
    }
    GLenum err;
    glAssert(err = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (err != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FBO::check() : FBO " << mId << " not complete (error = " << err << ") : ";
        switch (err) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        default:
            std::cerr << "Unknown ERROR";
        }
        std::cerr << std::endl;
    }

    return (err == GL_FRAMEBUFFER_COMPLETE);
}

//--------------------------------------------------
void FBO::useAsTarget(int width, int height)
{
    if (!mIsBound)
        bind();
    assert(mCurrentlyBound == this);
    glAssert( glViewport(0, 0, width, height) );
}

//--------------------------------------------------
void FBO::attachTexture(GLenum attachment, Texture *texture, GLuint level)
{
    if (!mIsBound) {
        std::cerr << "Error : trying to attach a texture to an unbound FBO (texture always unattached)" << std::endl;
        return;
    }
    switch (texture->getType()) {
    case Texture::TEX_1D:
        glAssert(glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, texture->getTarget(), texture->getId(), level));
        break;
    case Texture::TEX_2D:
        glAssert(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture->getTarget(), texture->getId(), level));
        break;
    case Texture::TEX_3D:
        glAssert(glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, texture->getTarget(), texture->getId(), level, texture->getZOffset()));
        break;
    case Texture::TEX_CUBE:
        glAssert(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture->getTarget(), texture->getId(), level));
        break;
    default:
        std::cerr << "WTF (attach texture)-- " << texture->getName() << " // " << texture->getType() << std::endl;
    }
}

//--------------------------------------------------
void FBO::detachTexture(GLenum attachment, Texture::TexType type)
{
    if (!mIsBound) {
        std::cerr << "Error : trying to detach a texture from an unbound FBO (texture always attached)" << std::endl;
        return;
    }
    switch (type) {
    case Texture::TEX_1D:
        glAssert(glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, 0, 0, 0));
        break;
    case Texture::TEX_2D:
        glAssert(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, 0, 0, 0));
        break;
    case Texture::TEX_3D:
        glAssert(glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, 0, 0, 0, 0));
        break;
    case Texture::TEX_CUBE:
        glAssert(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, 0, 0, 0));
        break;
    default:
        std::cerr << "WTF (detach texture)-- " << std::endl;
    }
}

void FBO::attachRBO(GLenum attachment, RBO *rbo)
{
    if (!mIsBound) {
        std::cerr << "Error : trying to attach a RBO to an unbound FBO (RBO not attached)" << std::endl;
    }
    rbo->bind();
    glAssert(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo->getId()));

    delete mRbos[attachment];
    mRbos[attachment] = rbo;
}

void FBO::detachRBO(GLenum attachment)
{
    if (!mIsBound) {
        std::cerr << "Error : trying to detach  a RBO from an unbound FBO (RBO always attached)" << std::endl;
        return;
    }
    glAssert(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, 0));

    delete mRbos[attachment];
    mRbos.erase(attachment);
}

//--------------------------------------------------
void FBO::clear(Components components)
{
    if (!mIsBound) {
        std::cerr << "Error : trying to clear an uThe FBO must be bound before calling this method.nbound FBO (FBO not cleared)" << std::endl;
        return;
    }

    Components filtered_components = Components(mComponents & components);

    GLbitfield mask = 0;
    if ((filtered_components & COLOR)) {
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if ((filtered_components & DEPTH)) {
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    if ((filtered_components & STENCIL)) {
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    glAssert(glClear(mask));
}

//--------------------------------------------------
void FBO::setSize(int width, int height)
{
    if (width > 0 && height > 0) {
        mWidth = width;
        mHeight = height;
        for (std::map< GLenum, RBO * >::iterator i=mRbos.begin(); i != mRbos.end() ; ++i) {
            (*i).second->setSize(width, height);
        }
    } else {
        std::cerr << "Error : invalid size : FBO width & height must be > 0." << std::endl;
    }
}

//--------------------------------------------------
FBO *FBO::createColorAndDepthFBO(int width, int height)
{
    FBO *fbo = new FBO(Components(DEPTH | COLOR), width, height);
    fbo->bind();
    fbo->attachRBO(GL_DEPTH_ATTACHMENT);
    fbo->attachRBO(GL_COLOR_ATTACHMENT0);
    fbo->check();
    fbo->unbind();
    glAssert( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    return fbo;
}

void FBO::attachRBO(GLenum attachment)
{
    RBO *rbo;
    switch (attachment) {
    case GL_DEPTH_ATTACHMENT:
        rbo = new RBO(GL_DEPTH_COMPONENT);
        break;
    case GL_STENCIL_ATTACHMENT:
        rbo = new RBO(GL_DEPTH_STENCIL);
        break;
    default:// a color attachment
        rbo = new RBO(GL_RGBA);
        break;
    }
    rbo->setSize(mWidth, mHeight);
    attachRBO(attachment, rbo);
//  if ( mIsBound )
//      mRbos[attachment] = rbo;
}

}



