#include "sculptor.h"
#include "../engine/timer.h"

Sculptor::Sculptor() :
    topHandler(this),
    currentOp(-1)
{}

Sculptor::~Sculptor() {
    for (int i = 0; i < ops.size(); i++)
        delete ops[i];
}

void Sculptor::loop(QuasiUniformMesh::Point vCenterPos)
{
    vortex::Timer t;
    t.start();

    assert(params.valid());

    buildField(vCenterPos);

    Operator *op = getCurrentOperator();
    qum->update_normals();
    op->applyDeformation(qum, vcenter, field_vertices, radius, params.getDMove());


    QuasiUniformMeshConverter::makeUniformField(*qum, field_edges, params.getMinEdgeLength(), params.getMaxEdgeLength());

    switch(op->getTopologicalChange()) {
        case Operator::NONE:
            break;
        case Operator::GENUS:
            // Stuff with Topological handler
            break;
    }

    t.stop();
    std::cout << "Loop execution : " << t.value() << std::endl;
}

float Sculptor::getRadius() const
{
    return radius;
}

void Sculptor::setRadius(float value)
{
    radius = value;
}

void Sculptor::buildField(QuasiUniformMesh::Point vCenterPos)
{
    field_vertices.clear();
    field_edges.clear();
    float minDist = FLT_MAX;
    for(QuasiUniformMesh::VertexIter v_it = qum->vertices_sbegin(); v_it != qum->vertices_end(); ++v_it)
    {
        float dist = calcDist(qum->point(*v_it), vCenterPos);
        if(dist < radius) {
            field_vertices.push_back(std::pair<QuasiUniformMesh::VertexHandle, float>(*v_it, dist));

            if (dist < minDist) {
                vcenter = *v_it;
                minDist = dist;
            }
        }
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
            OpenMesh::VertexHandle vh = field_vertices[i++].first;

            if(vh1 == vh)
                v1 = true;
            else if(vh2 == vh)
                v2 = true;
        }

        if(v1 && v2)
            field_edges.push_back(*e_it);
    }
}

void Sculptor::getMinMaxAvgEdgeLength(float &min, float &max, float &avg)
{
    min = FLT_MAX;
    max = 0;
    avg = 0;

    for(QuasiUniformMesh::EdgeIter e_it = qum->edges_sbegin(); e_it != qum->edges_end(); ++e_it)
    {
        float length = qum->calc_edge_length(*e_it);

        if(length < min)
            min = length;
        else if(length > max)
            max = length;

        avg += length;
    }

    avg /= qum->n_edges();
}
