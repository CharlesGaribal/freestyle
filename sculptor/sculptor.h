#ifndef SCULPTOR_H
#define SCULPTOR_H

#include "quasiuniformmesh.h"
#include "sculptorparameters.h"
#include "operator.h"
#include "topologicalhandler.h"

class Sculptor
{
public:
    Sculptor();

    void operator()(QuasiUniformMesh::VertexHandle vcenter, float radius);

    template<typename OpenMesh_type>
    void setMesh(OpenMesh_type &mesh)
    {
        field_edges.clear();
        field_vertices.clear();

        qum = new QuasiUniformMesh();

        QuasiUniformMeshConverter::convert(mesh, *qum);

        float min, max, avg;
        getMinMaxAvgEdgeLength(min, max, avg);

        std::cout << "min: " << min << "  max: " << max << "  avg: " << avg << std::endl;

        max = avg;
        min = max / 2.;
        params.setMinEdgeLength(min);
        params.setMaxEdgeLength(max);
        float dthickness = (max)/sqrt(3.f) + 0.3f;
        float dmove = (dthickness - ((max)/sqrt(3.f))) / 2.f - 0.1f;
        params.setDThickness(dthickness);
        params.setDMove(dmove);

        std::cout << "param min: " << min << "  max: " << max << "  dmove: " << dmove << "  dthickness: " << dthickness << std::endl;

        if (params.valid())
            QuasiUniformMeshConverter::makeUniform(*qum, params.getMinEdgeLength(), params.getMaxEdgeLength());
        else
            std::cout << "Erreur: paramètre quasi-uniforme non valide" << std::endl;
    }

    QuasiUniformMesh* getQUM() {return this->qum;}

    template<typename OpenMesh_type>
    inline void getMesh(OpenMesh_type &m) { QuasiUniformMeshConverter::convert(*qum, m); }
    inline float calcDist(QuasiUniformMesh::Point &p1, QuasiUniformMesh::Point &p2){ return sqrt(pow(p1[0]-p2[0], 2) + pow(p1[1]-p2[1], 2) + pow(p1[2]-p2[2], 2)); }

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

    void getMinMaxAvgEdgeLength(float &min, float &max, float &avg);
};

#endif // SCULPTOR_H
