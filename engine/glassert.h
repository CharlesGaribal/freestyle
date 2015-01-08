/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */


#ifndef GL_ASSERT_H
#define GL_ASSERT_H

/**
        @author Mathias Paulin <Mathias.Paulin@irit.fr>
 *
 * OpenGL error management class.
 *
 */

//#undef NDEBUG

#ifndef NDEBUG


#include <iostream>
#include <cassert>

#ifdef __APPLE__
#define glAssert(code) \
    code; \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            std::cerr<<"erreur OpenGL ("<<__FILE__<<":"<<__LINE__<<", "<<__STRING(code)<<") :"<<\
                ( (err == GL_INVALID_ENUM) ? " Invalid enum" : \
                ( (err == GL_INVALID_VALUE) ? " Invalid value" : \
                ( (err == GL_INVALID_OPERATION) ? " Invalid operation" : "unknown error") \
                ) \
                )\
                << " ("<<err<<")"<<std::endl; \
            /*assert(false);*/ \
        } \
    }

#define glCheckError() \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            std::cerr<<"erreur OpenGL ("<<__FILE__<<":"<<__LINE__<<") :"<<\
    ( (err == GL_INVALID_ENUM) ? " Invalid enum" : \
    ( (err == GL_INVALID_VALUE) ? " Invalid value" : \
    ( (err == GL_INVALID_OPERATION) ? " Invalid operation" : "unknown error") \
    ) \
    )\
    << " ("<<err<<")"<<std::endl; \
            /*assert(false);*/ \
        } \
    }
#else
#define glAssert(code) \
    code; \
{\
    GLuint err = glGetError(); \
    if (err != GL_NO_ERROR) { \
    std::cerr<<"erreur OpenGL ("<<__FILE__<<":"<<__LINE__<<", "<<__STRING(code)<<") : "<<(const char*)gluErrorString (err)<<"("<<err<<")"<<std::endl; \
    } \
    }

#define glCheckError() \
    {\
        GLuint err = glGetError(); \
        if (err != GL_NO_ERROR) { \
    std::cerr<<"erreur OpenGL ("<<__FILE__<<":"<<__LINE__<<", "<<__STRING()<<") : "<<(const char*)gluErrorString (err)<<"("<<err<<")"<<std::endl; \
          } \
    }
#endif

#else
#define glAssert(code) \
    code;

#define glCheckError()
#endif

// #undef NDEBUG

#endif
