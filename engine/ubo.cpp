/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "ubo.h"

namespace vortex {

UBO::UBO()
{
    glAssert(glGenBuffers(1, &uboId));
//  std::cerr << "UBO created : " << uboId << std::endl;
}

UBO::~UBO()
{
    glDeleteBuffers(1, &uboId);
//  std::cerr << "UBO deleted : " << uboId << std::endl;
}

void UBO::init(GLuint progId, const GLchar * uniformBlockName)
{
 /*
    mProgramHandle = progId;

    glAssert(glBindBuffer(GL_UNIFORM_BUFFER, uboId));
    //blockIdx contains the uniformBlockName info in the shader
    glAssert(blockIdx = glGetUniformBlockIndex(progId, uniformBlockName));
    //We need to get the uniform block's size in order to back it with the
    //appropriate buffer
    glAssert(glGetActiveUniformBlockiv(progId, blockIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize));

    glAssert(glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, NULL, GL_STREAM_DRAW));
    // Attach the buffer to UBO binding point 0...
    glAssert(glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboId));

    glAssert(glUniformBlockBinding(progId, blockIdx, 0));
*/
    //  glError();
}

void UBO::bind(GLvoid * uniforms, GLint size)
{
/*
    // Test if both data structures have the same size
//  if( size == uniformBlockSize ){
    // Bind Buffer
    glAssert(glBindBuffer(GL_UNIFORM_BUFFER, uboId));

    // And pass data to UBO
    //   Two possibilities :
    //   1) use of glBufferData
    glAssert(glBufferData(GL_UNIFORM_BUFFER, size, uniforms, GL_STREAM_DRAW));

//      //   2) use of glMapBuffer
//      glAssert ( glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STREAM_DRAW) );
//      GLvoid* PositionBuffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
//      memcpy(PositionBuffer, uniforms, size);
//      glAssert ( glUnmapBuffer(GL_UNIFORM_BUFFER) );

    //   Issues : Second method isn't so performant because of driver

    // BUG on botero normally this line doesn't participate in binding but apparently for proper working of binding it must be added, dunno why :/
//      glAssert( glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboId ) );
//  } else {
    // ERROR
//  }
*/

}

} //namespace vortex
