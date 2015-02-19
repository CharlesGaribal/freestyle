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

void InfDefOperator::applyDeformation(Mesh *mesh, Vertex vcenter, Field &field, float dmove) {

}
