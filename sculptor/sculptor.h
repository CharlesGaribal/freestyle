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
        if(avg < min + 0.25*(max-min))
        {
            max = min + (max-min)/2.;

            if(max/min < 2)
                min = max/2.01;
        }
        else if(avg > min + 0.75*(max-min))  // petit edges minoritaires
        {
            min = min + (max-min)/2.;

            if(max/min < 2)
                max = min*2.01;
        }
        else if(avg < min + 0.5*(max-min))
        {
            if(max/min < 2)
                min = max/2.01;
        }
        else if(avg > min + 0.5*(max-min))
        {
            if(max/min < 2)
                max = min*2.01;
        }

        double dmove = min/2.;
        double dthickness = sqrt(4*dmove*dmove + (max*max)/3.) + 0.001;

        params = SculptorParameters(min, max, dmove, dthickness);

        std::cout << "eval   min: " << min << "  max: " << max << "  dmove: " << dmove << "  dthickness: " << dthickness << std::endl;
        std::cout << "eval   4*dmove*dmove: " << 4.*dmove*dmove << "  dthick*dthick - (dmax*dmax)/3: " << dthickness*dthickness - (max*max)/3. << std::endl;
        std::cout << "params min: " << params.getMinEdgeLength() << "  max: " << params.getMaxEdgeLength() << "  dmove: " << params.getDMove() << "  dthickness: " << params.getDThickness() << std::endl;

        assert(params.valid());

        QuasiUniformMeshConverter::makeUniform(*qum, params.getMinEdgeLength(), params.getMaxEdgeLength());
        getMinMaxAvgEdgeLength(min, max, avg);

        std::cout << "min: " << min << "  max: " << max << "  avg: " << avg << std::endl;
    }

    inline QuasiUniformMesh* getQUM() {return this->qum;}
    inline SculptorParameters getParams() {return this->params;}
    inline std::vector<QuasiUniformMesh::EdgeHandle> getConnectingEdges() {return this->connecting_edges;}

    inline void getMesh(QuasiUniformMesh &m) { m = *qum; }

    inline float calcDist(QuasiUniformMesh::Point &p1, QuasiUniformMesh::Point &p2){ return sqrt(pow(p1[0]-p2[0], 2) + pow(p1[1]-p2[1], 2) + pow(p1[2]-p2[2], 2)); }

    inline int addOperator(Operator *op) {
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
