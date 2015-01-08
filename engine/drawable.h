/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "opengl.h"

#include "renderstate.h"

namespace vortex {

/**
 * Abstract class used for drawables objects. Usefull for polymorphism in rendere loops
 *
 */
class Drawable {
public :
    virtual ~Drawable(){}
    virtual void draw() = 0;

protected:
    Drawable() {}
};

} // vortex

#endif // DRAWABLE_H
