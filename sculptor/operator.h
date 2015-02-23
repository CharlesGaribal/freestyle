#ifndef OPERATOR_H
#define OPERATOR_H

#include "quasiuniformmesh.h"
class Sculptor;



class Operator
{
protected:
    typedef QuasiUniformMesh Mesh;
    typedef Mesh::VertexHandle Vertex;
    typedef std::pair<Vertex, float> FieldValue;
    typedef std::vector<FieldValue> Field;

public:
    enum ETopologicalChange {NONE, GENUS};

    Operator() {}
    virtual ~Operator() {}

    virtual void applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float radius, float dmove) = 0;
    virtual ETopologicalChange getTopologicalChange() = 0;
};

class SweepOperator : public Operator
{
public:
    SweepOperator() {}

    void applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float radius, float dmove) {}
    ETopologicalChange getTopologicalChange() {
        return ETopologicalChange::NONE;
    }
};


class InfDefOperator : public Operator
{
public:
    static const int INFLATE = 1;
    static const int DEFLATE = -1;

    InfDefOperator(int _direction = INFLATE, int _smoothParam = 2);

    void setDirection(int _direction);
    void setSmoothParam(int _smoothParam);

    void applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float radius, float dmove);
    ETopologicalChange getTopologicalChange();

private:
    int direction;
    int smoothParam;
};


class TwistOperator : public Operator
{
public:
    static const int CLOCKWISE = 1;
    static const int ANTICLOCKWISE = -1;


    TwistOperator(int _direction = CLOCKWISE, int _smoothParam = 2);

    void setDirection(int _direction);
    void setSmoothParam(int _smoothParam);

    void applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float radius, float dmove);
    ETopologicalChange getTopologicalChange();

private:
    Mesh::Point prodVec(Mesh::Point u, Mesh::Point v) {
        Mesh::Point r;
        r[0] = u[1]*v[2] - u[2]*v[1];
        r[1] = u[2]*v[0] - u[0]*v[2];
        r[2] = u[0]*v[1] - u[1]*v[0];
        return r;
    }

    float prodScal(Mesh::Point u, Mesh::Point v) {
        return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
    }

    int direction;
    int smoothParam;
};

#endif // OPERATOR_H
