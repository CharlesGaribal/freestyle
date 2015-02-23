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
    bool hasOneEdgeSame(OpenMesh::FaceHandle &fh1, OpenMesh::FaceHandle &fh2);
    bool hasSameVertices(OpenMesh::FaceHandle &fh1, OpenMesh::FaceHandle &fh2);
    bool hasSameVertices(OpenMesh::EdgeHandle &eh1, OpenMesh::EdgeHandle &eh2);
    bool isSameVertexHandle(OpenMesh::VertexHandle &v1, OpenMesh::VertexHandle &v2);

    Sculptor *sculptor;
};

#endif // TOPOLOGICALHANDLER_H
