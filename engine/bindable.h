/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef _BINDABLE_H
#define _BINDABLE_H

#include <iostream>

#include "opengl.h"

namespace vortex {

class Bindable;
/**
  * Abstract class to define parameters associated with a bindable.
  * Mainly useful for shaders
  */
class GlobalParameter {
public:
    virtual ~GlobalParameter() {}
    /**
      * Set the parameters for the Bindable object.
      */
    virtual void set(const Bindable *object) const =0;
protected:
    GlobalParameter() {}
};



/**
 * Abstract class used for bindable objects that needs eventually some parameters
 *
 */
class Bindable {
public:
    /**
      * Bind the object to the rendering pipeline.
      * Must be defined in the subclass
      */
    virtual void bind() const = 0;
    /**
      * Bind the object to the rendering pipeline and set perameters.
      * Could be redefined in the subclass to specialize the binding and parameters positionning.
      * @todo : resolve polymorphism problem when using map<ShaderProgram, ...>
      */
    virtual void bind(const GlobalParameter& parameters) const{
        bind();
        parameters.set(this);
    };

    virtual ~Bindable(){}
protected:
    Bindable() {};
};


}
#endif
