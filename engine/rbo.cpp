/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "rbo.h"

namespace vortex {

RBO::RBO(GLenum format) : mFormat(format)
{
    glAssert(glGenRenderbuffers(1, &mId));
    glAssert(glBindRenderbuffer(GL_RENDERBUFFER, mId));
    glAssert(glRenderbufferStorage(GL_RENDERBUFFER, mFormat, 1, 1));
}

void RBO::setSize(int width, int height)
{
    // verification des dimensions :
    int MAX_SIZE;
#ifdef __APPLE__
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &MAX_SIZE);
#else
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &MAX_SIZE);
#endif
    if ((width > 0) && (width <= MAX_SIZE) && (height > 0) && (height <= MAX_SIZE)) {
        glAssert(glBindRenderbuffer(GL_RENDERBUFFER, mId));
        glAssert(glRenderbufferStorage(GL_RENDERBUFFER, mFormat, width, height));
    } else {
        std::cerr << "Error : invalid size (" << width << "," << height << "): maximum size supported = " << MAX_SIZE << " (RBO not resized)" << std::endl;
    }
}

void RBO::bind()
{
    glAssert(glBindRenderbuffer(GL_RENDERBUFFER, mId));
}

void RBO::deleteGL()
{
    glAssert(glDeleteRenderbuffers(1, &mId));
}

} // vortex
