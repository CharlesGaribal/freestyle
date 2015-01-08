/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef RBO_H
#define RBO_H

#include <iostream>
#include "opengl.h"

namespace vortex {

/**
 * RenderBuffer Object representation class
 *
 */
class RBO {
public:

    /**
     * Constructor : RenderBuffer Object OpenGL creation with an initial size of (1, 1).
     *
     * @param format Specify the internal format for the RenderBuffer Object.
     */
    RBO(GLenum format);

    ~RBO() {
        deleteGL();
    }

    /**
     * Bind the RBO.
     *
     */
    void bind();

    /**
     * RenderBuffer Object OpenGL deletion.
     *
     */
    void deleteGL();

    /**
     * Resize the RBO according to width and height.
     *
     * @param width New width of the RBO. Value must be between 1 and the maximum RenderBuffer Object supported size, otherwise the RBO is not resized.
     * @param width New height of the RBO. Value must be between 1 and the maximum RenderBuffer Object supported size, otherwise the RBO is not resized.
     */
    void setSize(int width, int height);

    /**
     * @return The OpenGL id of the RenderBuffer Object.
     *
     */
    GLuint getId() {
        return mId;
    }

private:
    GLuint mId;
    GLenum mFormat;
};

} // vortex

#endif // RBO_H
