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

    void operator()(QuasiUniformMesh::VertexHandle vcenter, float radius, const Operator::AdditionnalParameters &opParams);

    template<typename OpenMesh_type>
    void setMesh(OpenMesh_type &mesh)
    {
        field_edges.clear();
        field_vertices.clear();

        qum = new QuasiUniformMesh();

        QuasiUniformMeshConverter::convert(mesh, *qum);

        if (params.valid())
            QuasiUniformMeshConverter::makeUniform(*qum, params.getMinEdgeLength(), params.getMaxEdgeLength());
        else
            std::cout << "Erreur: paramètre quasi-uniforme non valide" << std::endl;
    }

    template<typename OpenMesh_type>
    inline void getMesh(OpenMesh_type &m) { QuasiUniformMeshConverter::convert(*qum, m); }

private:
    TopologicalHandler topHandler;

    SculptorParameters params;

    std::vector<Operator> ops;
    int currentOp;

    QuasiUniformMesh *qum;

    // Informations about current deformation
    std::vector<QuasiUniformMesh::VertexHandle> field_vertices;
    std::vector<QuasiUniformMesh::EdgeHandle> field_edges;
    QuasiUniformMesh::VertexHandle vcenter;
    QuasiUniformMesh::Point vcenter_pos;
    float radius;

    void buildField();
    inline float calcDist(QuasiUniformMesh::Point &p1, QuasiUniformMesh::Point &p2){ return sqrt(pow(p1[0]-p2[0], 2) + pow(p1[1]-p2[1], 2) + pow(p1[2]-p2[2], 2)); }
};

#endif // SCULPTOR_H
