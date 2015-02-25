#ifndef TOPOLOGICALHANDLER_H
#define TOPOLOGICALHANDLER_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>
#include "quasiuniformmesh.h"

class Sculptor;

//Sommet courant du second 1-ring
struct VertexSecondRing
{
    //Distance du sommet par rapport au somment courant du premier 1-ring
    float dist;
    QuasiUniformMesh::VertexHandle v;
    //Nombre de fois où le sommet est lié à un sommet du premier 1-ring
    int nbUsing = 0;
};

class TopologicalHandler
{
public:
    TopologicalHandler(Sculptor *sculptor);

    void setSculptor(Sculptor *sculptor) {
        this->sculptor = sculptor;
    }

    void handleJoinVertex(OpenMesh::VertexHandle &v1, OpenMesh::VertexHandle &v2);
    void cleanup(OpenMesh::VertexHandle &v1);

private:
    bool hasOneEdgeSame(OpenMesh::FaceHandle &fh1, OpenMesh::FaceHandle &fh2);
    bool hasSameVertices(OpenMesh::FaceHandle &fh1, OpenMesh::FaceHandle &fh2);
    bool hasSameVertices(OpenMesh::EdgeHandle &eh1, OpenMesh::EdgeHandle &eh2);
    bool isSameVertexHandle(OpenMesh::VertexHandle &v1, OpenMesh::VertexHandle &v2);

    Sculptor *sculptor;
};

#endif // TOPOLOGICALHANDLER_H
