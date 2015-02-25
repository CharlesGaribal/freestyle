#ifndef SUBDIVIDER_H
#define SUBDIVIDER_H

#include "opengl.h"
#include "../sculptor/quasiuniformmesh.h"

class Subdivider{
public:
  Subdivider(){}  
  static void subdivide(QuasiUniformMesh & omesh);
};
#endif // SUBDIVIDER_H
