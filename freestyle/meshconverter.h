#ifndef MESHCONVERTER_H
#define MESHCONVERTER_H

#include <../engine/mesh.h>
#include "../sculptor/quasiuniformmesh.h"

class MeshConverter{
public:
    static void convert(QuasiUniformMesh *in, vortex::Mesh *out);
    static void convert(vortex::Mesh *in, QuasiUniformMesh *out);
};


#endif // MESHCONVERTER_H
