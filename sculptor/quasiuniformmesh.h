#ifndef QUASIUNIFORMMESH_H
#define QUASIUNIFORMMESH_H

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

struct QuasiUniformMeshTraits : public OpenMesh::DefaultTraits
{
    typedef OpenMesh::Vec3f Point;
    typedef OpenMesh::Vec3f Normal;
    typedef OpenMesh::Vec3f TexCoord3D;

    VertexAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
    FaceAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
    EdgeAttributes(OpenMesh::Attributes::Status);
    HalfedgeAttributes(OpenMesh::Attributes::Status| OpenMesh::Attributes::Normal);
};

typedef OpenMesh::TriMesh_ArrayKernelT<QuasiUniformMeshTraits> QuasiUniformMesh;

typedef OpenMesh::PolyMesh_ArrayKernelT<OpenMesh::DefaultTraits> DefaultPolyMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<OpenMesh::DefaultTraits> DefaultTriMesh;

class QuasiUniformMeshConverter {
public:
    static void convert(DefaultPolyMesh *in, QuasiUniformMesh *out);
    static void convert(QuasiUniformMesh *in, DefaultPolyMesh *out);
    static void convert(DefaultTriMesh *in, QuasiUniformMesh *out);
    static void convert(QuasiUniformMesh *in, DefaultTriMesh *out);
    static void makeUniform(QuasiUniformMesh *mesh, float edgeMin, float edgeMax);
    static void makeUniformField(QuasiUniformMesh *mesh, const std::vector<OpenMesh::VertexHandle> &region, float edgeMin, float edgeMax);
};

#endif // QUASIUNIFORMMESH_H



