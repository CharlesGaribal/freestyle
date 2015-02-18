#ifndef TOPOLOGICALHANDLER_H
#define TOPOLOGICALHANDLER_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

class Sculptor;


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
    Sculptor *sculptor;
};

#endif // TOPOLOGICALHANDLER_H
