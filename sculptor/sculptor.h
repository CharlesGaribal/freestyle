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
    ~Sculptor();

    void loop(QuasiUniformMesh::Point vCenterPos);

    void setMesh(QuasiUniformMesh &mesh)
    {
        field_edges.clear();
        field_vertices.clear();
        qum = &mesh;

        float min, max, avg;
        getMinMaxAvgEdgeLength(min, max, avg);

        std::cout << "min: " << min << "  max: " << max << "  avg: " << avg << std::endl;

        // grand edges minoritaires
        if(avg < min + 0.25f*(max-min))
        {
            max = (max-min)/2.f;

            if(max/min < 2)
                min = max/2.01f;
        }
        else if(avg > min + 0.75f*(max-min))  // petit edges minoritaires
        {
            min = (max-min)/2.f;

            if(max/min < 2)
                max = min*2.01f;
        }
        else if(avg < min + 0.5f*(max-min))
        {
            if(max/min < 2)
                min = max/2.01f;
        }
        else if(avg > min + 0.5f*(max-min))
        {
            if(max/min < 2)
                max = min*2.01f;
        }

        float dthickness = 0.01 + sqrt(0.01f*0.01f*4.f + (max*max)/3.f);
        float dmove = sqrt((dthickness*dthickness - (max*max)/3.f)/4.f);

        params = SculptorParameters(min, max, dmove, dthickness);

        std::cout << "param min: " << min << "  max: " << max << "  dmove: " << dmove << "  dthickness: " << dthickness << std::endl;
        std::cout << "param min: " << params.getMinEdgeLength() << "  max: " << params.getMaxEdgeLength() << "  dmove: " << params.getDMove() << "  dthickness: " << params.getDThickness() << std::endl;

        if (params.valid())
            QuasiUniformMeshConverter::makeUniform(*qum, params.getMinEdgeLength(), params.getMaxEdgeLength());
        else
            std::cout << "Erreur: paramètre quasi-uniforme non valide" << std::endl;
    }

    QuasiUniformMesh* getQUM() {return this->qum;}
    SculptorParameters getParams() {return this->params;}
    std::vector<QuasiUniformMesh::EdgeHandle> getConnectingEdges() {return this->connecting_edges;}

    inline void getMesh(QuasiUniformMesh &m) { m = *qum; }

    inline float calcDist(QuasiUniformMesh::Point &p1, QuasiUniformMesh::Point &p2){ return sqrt(pow(p1[0]-p2[0], 2) + pow(p1[1]-p2[1], 2) + pow(p1[2]-p2[2], 2)); }

    int addOperator(Operator *op) {
        ops.push_back(op);
        return ops.size()-1;
    }

    Operator *getOperator(int index) {
        assert(index >= 0 && index < (int) ops.size());
        return ops[index];
    }

    void setCurrentOperator(int index) {
        assert(index >= -1 && index < (int) ops.size());
        currentOp = index;
    }

    int getCurrentIndexOp() {
        return currentOp;
    }

    SculptorParameters getParameters() {
        return params;
    }

    float getRadius() const;
    void setRadius(float value);

private:
    TopologicalHandler topHandler;

    SculptorParameters params;

    std::vector<Operator*> ops;
    int currentOp;

    QuasiUniformMesh *qum;

    // Informations about current deformation
    std::vector<std::pair<QuasiUniformMesh::VertexHandle, float>> field_vertices;
    std::vector<QuasiUniformMesh::EdgeHandle> field_edges;
    std::vector<QuasiUniformMesh::EdgeHandle> connecting_edges;
    QuasiUniformMesh::VertexHandle vcenter;

    float radius;

    void buildField(QuasiUniformMesh::Point vCenterPos);

    void getMinMaxAvgEdgeLength(float &min, float &max, float &avg);
};

#endif // SCULPTOR_H
