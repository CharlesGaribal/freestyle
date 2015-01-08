/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef FBO_H
#define FBO_H

#include <map>

#include "opengl.h"

#include "texture.h"
#include "rbo.h"

namespace vortex {

/**
   basic abstraction for a FBO.
   all the callers must ensure that the currently bound FBO is the one
   that is handled by the FBO object.
*/

/**
 * OpenGL FrameBuffer Object representation class
 *
 */
class FBO {
public:

    /**
     * FrameBuffer Object type enumeration : Specify which attachment type can be call for attachment
     *
     */
    typedef enum {
        COLOR = 1 << 0,
        DEPTH = 1 << 1,
        STENCIL = 1 << 2,
        ALL = COLOR | DEPTH | STENCIL
    } Components;

    /**
     * Constructor : Opengl creation of the FBO and initialize FBO attributes.
     *
     * @param comp FBO type
     * @param width FBO width
     * @param height FBO height
     */
    FBO(Components comp , int width, int height);

    /**
     * Destructor : responsible for FBO and RBO OpenGL deletion and attached RBO deletion
     *
     */
    ~FBO();

    /**
     * Specify the OpenGL API this FBO will be used as render FrameBuffer Object
     *
     */
    void bind();

    /**
     * Bind the FBO, check its completeness and set his viewport
     *
     * @param width New FBO width
     * @param height New FBO height
     */
    void useAsTarget(int width, int height);

    /**
     * Specify the OpenGL API to use default render FrameBuffer Object
     *
     */
    void unbind();


    /**
      * Bind screen !
      *
      */
    static void bindDefault(){
        if(mCurrentlyBound){
            mCurrentlyBound->unbind();
        }
        glAssert(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    /**
     * Clear FBO attached buffers defined by "comp"
     *
     * @param comp Type of buffers to be cleared
     */
    void clear(Components comp);

    /**
     * Check the FBO completeness
     *
     */
    bool check() const;

    /**
     * Attach the Texture "texture" to the "attachment" FBO buffer
     *
     * @param attachment Specify the attachment point to which the texture must be attached. Accepted values are the same as for of glFrameBufferTexture*
     * @param texture The Texture to be attached
     * @param level The mipmap level of the texture to be attached
     *
     * If a Texture was already attached to "attachment", "texture" will replace it after calling this method.
     * The FBO must be bound before calling this method. The Texture must have been initialized by calling "Texture::initGL()" before calling this method.
     */
    void attachTexture(GLenum attachment, Texture *texture, GLuint level = 0);

    /**
     * Detach the Texture "texture" to the "attachment" FBO buffer
     *
     * @param attachment Specify the attachment point to which the texture must be attached. Accepted values are the same as for glFrameBufferTexture*
     * @param type Specify the type of the texture attached to "attachment".
     *
     * The FBO must be bound before calling this method.
     */
    void detachTexture(GLenum attachment, Texture::TexType type = Texture::TEX_2D);

    /**
     * Create and attach a RBO to "attachment"
     *
     * @param attachment Specify the attachment point to which the texture must be attached. Accepted values are the same as for glFrameBufferTexture*
     *
     * The FBO must be bound before calling this method.
     */
    void attachRBO(GLenum attachment);

    /**
     * Attach a RBO to "attachment"
     *
     * @param attachment Specify the attachment point to which the texture must be attached. Accepted values are the same as for glFrameBufferTexture*
     * @param rbo The RBO to be attached.
     *
     * The FBO must be bound before calling this method.
     */
    void attachRBO(GLenum attachment, RBO *rbo);

    /**
     * Detach a RBO from "attachment"
     *
     * @param attachment Specify the attachment point to which the texture must be attached. Accepted values are the same as for glFrameBufferTexture*
     *
     * The FBO must be bound before calling this method.
     */
    void detachRBO(GLenum attachment);

    /**
     * Set FBO dimensions
     *
     * @param width New width of the FBO. "width" value must be > 0.
     * @param height New height of the FBO. "height" value must be > 0.
     *
     * If there are RBOs attached to the FBO, they are resized according to "width" and "height" if the values don't exceed the maximum RenderBuffer Object supported size.
     */
    void setSize(int width, int height);

    /**
     * FBO Facory method : Create a FBO with Color and Depth components and RBOs attached to those components.
     *
     * @param width Width of the FBO. "width" value must be > 0.
     * @param height Height of the FBO. "height" value must be > 0.
     *
     * It "with" or "height" value exceeds the maximum RenderBuffer Object supported size, RBOs will be (1, 1) sized.
     */
    static FBO *createColorAndDepthFBO(int width, int height);

    /**
     * @return The components of the FBO
     *
     */
    Components getComponents() {
        return mComponents;
    }

private:
    /**
     * OpenGL deletion of the FBO.
     *
     */
    void deleteGL();

    GLuint mId;
    Components mComponents;
    int mWidth, mHeight;

    std::map< GLenum, RBO * > mRbos;

    bool mIsBound;

    static FBO *mCurrentlyBound;

};

}
#endif



