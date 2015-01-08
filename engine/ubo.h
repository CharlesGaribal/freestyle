/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef UBO_H
#define UBO_H

#include <iostream>
#include <string.h>

#include "opengl.h"

namespace vortex {

/**
 * Uniform Buffer Object
 * cf http://www.opengl.org/registry/specs/ARB/uniform_buffer_object.txt
 */

class UBO {
public :

    UBO();
    ~UBO();

    /**
     * Perform all the required initialisation bindings between UBO and Shader Uniform Block
     *
     * @param progId Program identificator
     * @param uniformBlockName Name of the block that will be binded
     */
    void init(GLuint progId, const GLchar * uniformBlockName);

    /**
     * Bind the uniform block data to the UBO
     *
     * @param uniforms Uniform block data to bind
     * @param size Size of the Uniform block data
     */
    void bind(GLvoid * uniforms, GLint size);

    GLuint idProgram() {
        return mProgramHandle;
    }

    GLuint id() {
        return uboId;
    }

private :
    GLuint uboId;
    GLuint mProgramHandle;
    // just for test if both structures have the same size
    GLint uniformBlockSize;
};

}

#endif
