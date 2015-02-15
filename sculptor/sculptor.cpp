#include "sculptor.h"

Sculptor::Sculptor(const SculptorParameters &params) :
    topHandler(this),
    params(params),
    currentOp(-1)
{}

void Sculptor::operator()(QuasiUniformMesh::VertexHandle vcenter, float radius, const Operator::AdditionnalParameters &opParams)
{
    if (!params.valid() || ops.empty())
        return;

    this->vcenter = vcenter;
    this->radius = radius;
    buildField();

    Operator &op = ops[currentOp];
    op(opParams);

    QuasiUniformMeshConverter::makeUniformField(*qum, field_edges, params.getMinEdgeLength(), params.getMaxEdgeLength());

    switch(op.getTopologicalChange()) {
        case Operator::NONE:
            break;
        case Operator::GENUS:
            // Stuff with Topological handler
            break;
    }
}

void Sculptor::buildField()
{
    for(QuasiUniformMesh::VertexIter v_it = qum->vertices_sbegin(); v_it != qum->vertices_end(); ++v_it)
    {
        if(calcDist(qum->point(*v_it), vcenter_pos) < radius)
            field_vertices.push_back(*v_it);
    }

    for(QuasiUniformMesh::EdgeIter e_it = qum->edges_sbegin(); e_it != qum->edges_end(); ++e_it)
    {
        QuasiUniformMesh::HalfedgeHandle heh0 = qum->halfedge_handle(*e_it, 0);
        QuasiUniformMesh::VertexHandle vh1 = qum->to_vertex_handle(heh0);
        QuasiUniformMesh::VertexHandle vh2 = qum->from_vertex_handle(heh0);
        bool v1 = false, v2 = false;
        int i = 0;

        while (i < field_vertices.size() && !v1 && !v2)
        {
            OpenMesh::VertexHandle vh = field_vertices[i++];

            if(vh1 == vh)
                v1 = true;
            else if(vh2 == vh)
                v2 = true;
        }

        if(v1 && v2)
            field_edges.push_back(*e_it);
    }
}
