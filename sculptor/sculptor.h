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

        QuasiUniformMeshConverter::convert(mesh, *qum, scaleFactor);

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

    template<typename OpenMesh_type>
    inline void getMesh(OpenMesh_type &m)
    {
        ScaleFactor s;
        s.x = 1./scaleFactor.x;
        s.y = 1./scaleFactor.y;
        s.z = 1./scaleFactor.z;
        QuasiUniformMeshConverter::convert(*qum, m, s);
    }

    inline void setScaleFactor(float x, float y, float z){ scaleFactor.x = x; scaleFactor.y = y; scaleFactor.z = z; }
    inline void getScaleFactor(float &x, float &y, float &z){ x = scaleFactor.x; y = scaleFactor.y; z = scaleFactor.z; }

private:
    TopologicalHandler topHandler;

    SculptorParameters params;

    std::vector<Operator> ops;
    int currentOp;

    QuasiUniformMesh *qum;

    ScaleFactor scaleFactor;

    // Informations about current deformation
    std::vector<QuasiUniformMesh::VertexHandle> field_vertices;
    std::vector<QuasiUniformMesh::EdgeHandle> field_edges;
    QuasiUniformMesh::VertexHandle vcenter;
    QuasiUniformMesh::Point vcenter_pos;
    float radius;

    void buildField();
    inline float calcDist(QuasiUniformMesh::Point &p1, QuasiUniformMesh::Point &p2){ return sqrt(pow(p1[0]-p2[0], 2) + pow(p1[1]-p2[1], 2) + pow(p1[2]-p2[2], 2)); }
    void getMinMaxAvgEdgeLength(float &min, float &max, float &avg);
};

#endif // SCULPTOR_H
