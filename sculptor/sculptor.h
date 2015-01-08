#ifndef SCULPTOR_H
#define SCULPTOR_H

#include "quasiuniformmesh.h"
#include "sculptorparameters.h"
#include "operator.h"
#include "topologicalhandler.h"

class Sculptor
{
public:
    Sculptor(const SculptorParameters &params);

    void operator()(OpenMesh::VertexHandle vcenter, float radius);

    void setMesh(QuasiUniformMesh *mesh) {
        if (params.valid())
            QuasiUniformMeshConverter::convert(mesh, params.getMinEdgeLength(), params.getMaxEdgeLength());
    }

private:
    TopologicalHandler topHandler;

    SculptorParameters params;

    std::vector<Operator> ops;
    int currentOp;

    QuasiUniformMesh *qum;

    // Informations about current deformation
    std::vector<OpenMesh::VertexHandle> field;
    OpenMesh::VertexHandle vcenter;
    float radius;

    void buildField();
};

#endif // SCULPTOR_H
