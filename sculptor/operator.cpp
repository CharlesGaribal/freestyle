#include "operator.h"
#include "sculptor.h"

InfDefOperator::InfDefOperator(int _direction, int _smoothParam) :
    direction(_direction),
    smoothParam(_smoothParam) {

}

void InfDefOperator::setDirection(int _direction) {
    assert(_direction == INFLATE || _direction == DEFLATE);
    direction = _direction;
}

void InfDefOperator::setSmoothParam(int _smoothParam) {
    assert(_smoothParam >= 2);
    smoothParam = _smoothParam;
}

Operator::ETopologicalChange InfDefOperator::getTopologicalChange() {
    return ETopologicalChange::GENUS;
}

void InfDefOperator::applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float radius, float dmove) {
    int n = smoothParam;

    for (int i = 0; i < field.size(); i++) {
        Vertex v = field[i].first;
        float x = field[i].second / radius;
        QuasiUniformMesh::Normal normal = mesh->normal(v);
        float b = direction * ((n-1)*pow(x,n) - n*pow(x,n-1) + 1) * dmove;
        QuasiUniformMesh::Point vPos = mesh->point(v);
        QuasiUniformMesh::Point vNewPos(vPos[0] + normal[0]*b, vPos[1] + normal[1]*b , vPos[2] + normal[2]*b);
        mesh->set_point(v, vNewPos);
    }
}
