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

    for (int i = 0; i < (int) field.size(); i++) {
        Vertex v = field[i].first;
        float x = field[i].second / radius;
        QuasiUniformMesh::Normal normal = mesh->normal(v);
        float b = direction * ((n-1)*pow(x,n) - n*pow(x,n-1) + 1) * dmove;
        QuasiUniformMesh::Point vPos = mesh->point(v);
        QuasiUniformMesh::Point vNewPos(vPos[0] + normal[0]*b, vPos[1] + normal[1]*b , vPos[2] + normal[2]*b);
        mesh->set_point(v, vNewPos);
    }
}

TwistOperator::TwistOperator(int _direction, int _smoothParam) :
    direction(_direction),
    smoothParam(_smoothParam) {

}

void TwistOperator::setDirection(int _direction) {
    assert(_direction == CLOCKWISE || _direction == ANTICLOCKWISE);
    direction = _direction;
}

void TwistOperator::setSmoothParam(int _smoothParam) {
    assert(_smoothParam >= 2);
    smoothParam = _smoothParam;
}

Operator::ETopologicalChange TwistOperator::getTopologicalChange() {
    return ETopologicalChange::GENUS;
}

void TwistOperator::applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float radius, float dmove) {
    float a0 = M_PI/180. * 10.;
    int n = smoothParam;

    for (int i = 0; i < (int) field.size(); i++) {
        Vertex v = field[i].first;
        float r = field[i].second / radius;
        QuasiUniformMesh::Point vP = mesh->point(v);
        QuasiUniformMesh::Normal N = mesh->normal(v);
        QuasiUniformMesh::Point vC = mesh->point(vcenter);
        QuasiUniformMesh::Point vecR(vP[0]-vC[0], vP[1]-vC[1], vP[2]-vC[2]);

        float a = a0 * ((n-1)*pow(r,n) - n*pow(r,n-1) + 1);

        QuasiUniformMesh::Point vecD(0,0,0);
        N.normalize();
        float ps = prodScal(vecR,N);
        QuasiUniformMesh::Point pv = prodVec(N,vecR);
        float cosA = cos(a);
        float sinA = sin(a);
        vecD[0] = (vecR[0] - (ps*N[0])) * (1-cosA) + pv[0]*sinA;
        vecD[1] = (vecR[1] - (ps*N[1])) * (1-cosA) + pv[1]*sinA;
        vecD[2] = (vecR[2] - (ps*N[2])) * (1-cosA) + pv[2]*sinA;

        QuasiUniformMesh::Point vNewPos(vP[0] + vecD[0], vP[1] + vecD[1] , vP[2] + vecD[2]);
        mesh->set_point(v, vNewPos);
    }
}
