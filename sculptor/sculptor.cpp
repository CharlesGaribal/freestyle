#include "sculptor.h"

Sculptor::Sculptor(const SculptorParameters &params) :
    topHandler(this),
    params(params),
    currentOp(-1)
{}

void Sculptor::operator()(OpenMesh::VertexHandle vcenter, float radius) {
    if (!params.valid() || ops.empty())
        return;

    this->vcenter = vcenter;
    this->radius = radius;
    buildField();

    Operator &op = ops[currentOp];
    op();

    QuasiUniformMeshConverter::convertRegion(qum, field, params.getMinEdgeLength(), params.getMaxEdgeLength());

    switch(op.getTopologicalChange()) {
        case Operator::NONE:
            break;
        case Operator::GENUS:
            // Stuff with Topological handler
            break;
    }
}

void Sculptor::buildField() {

}

