#ifndef OPERATOR_H
#define OPERATOR_H

#include "quasiuniformmesh.h"
class Sculptor;



class Operator
{
protected:
    typedef QuasiUniformMesh Mesh;
    typedef std::vector<Mesh::VertexHandle> Field;
    typedef Mesh::VertexHandle Vertex;

public:
    enum ETopologicalChange {NONE, GENUS};

    Operator() {}

    virtual void applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float dmove) = 0;
    virtual ETopologicalChange getTopologicalChange() = 0;
};

class InfDefOperator : Operator
{
public:
    static const int INFLATE = 1;
    static const int DEFLATE = -1;

    InfDefOperator(int _direction = INFLATE, int _smoothParam = 2);

    void setDirection(int _direction);
    void setSmoothParam(int _smoothParam);

    void applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float dmove);
    ETopologicalChange getTopologicalChange();

private:
    int direction;
    int smoothParam;
};

#endif // OPERATOR_H
