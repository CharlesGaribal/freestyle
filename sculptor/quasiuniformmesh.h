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
    static void makeUniform(QuasiUniformMesh &mesh, float edgeMin, float edgeMax);
    static void makeUniformField(QuasiUniformMesh &mesh, const std::vector<OpenMesh::EdgeHandle> &field, float edgeMin, float edgeMax);

    template<typename T_in, typename T_out>
    static void convert(T_in &in, T_out &out)
    {
        out.request_halfedge_normals();

        OpenMesh::FPropHandleT<std::vector<typename T_out::VertexHandle>> face_vhs;
        in.add_property(face_vhs);

        in.triangulate();

        // Create all the vertices in the new T_out
        for(typename T_in::VertexIter v_it = in.vertices_sbegin(); v_it != in.vertices_end(); ++v_it)
        {
            // Add vertex to T_out
            typename T_out::VertexHandle vh = out.add_vertex(in.point(*v_it));

            // Add the new vertex handle to each faces that the old vertex is attached to
            for(typename T_in::VertexFaceIter vf_it = in.vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
            {
                in.property(face_vhs, *vf_it).push_back(vh);
            }
        }

        // Create all the faces (and automatically edges and halfedges) in the new T_out
        for(typename T_in::FaceIter f_it = in.faces_sbegin(); f_it != in.faces_end(); ++f_it)
        {
            typename T_in::HalfedgeHandle heh_f1 = in.halfedge_handle(*f_it);
            typename T_in::HalfedgeHandle heh_f2 = in.next_halfedge_handle(heh_f1);
            typename T_in::HalfedgeHandle heh_f3 = in.next_halfedge_handle(heh_f2);
            typename T_in::VertexHandle old_vh1 = in.to_vertex_handle(heh_f1);
            typename T_in::VertexHandle old_vh2 = in.to_vertex_handle(heh_f2);
            typename T_in::VertexHandle old_vh3 = in.to_vertex_handle(heh_f3);
            typename T_in::Point p1 = in.point(old_vh1);
            typename T_in::Point p2 = in.point(old_vh2);
            typename T_in::Point p3 = in.point(old_vh3);

            std::vector<typename T_out::VertexHandle> vertices = in.property(face_vhs, *f_it);

            typename T_out::VertexHandle new_vh1, new_vh2, new_vh3;

            typename T_out::Point p;

            for(int i = 0; i < vertices.size(); i++)
            {
                p = out.point(vertices[i]);

                if(p[0] == p1[0] && p[1] == p1[1] && p[2] == p1[2])
                    new_vh1 = vertices[i];
                else if(p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2])
                    new_vh2 = vertices[i];
                else if(p[0] == p3[0] && p[1] == p3[1] && p[2] == p3[2])
                    new_vh3 = vertices[i];
            }

            out.add_face(new_vh1, new_vh2, new_vh3);
        }
    }
};

#endif // QUASIUNIFORMMESH_H



