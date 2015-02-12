#include "quasiuniformmesh.h"

QuasiUniformMesh QuasiUniformMeshConverter::convert(DefaultPolyMesh *mesh)
{
    QuasiUniformMesh *qum;

    OpenMesh::FPropHandleT<std::vector<QuasiUniformMesh::VertexHandle>> face_vhs;
    mesh->add_property(face_vhs);

    mesh->triangulate();

    // Create all the vertices in the new QuasiUniformMesh
    for(DefaultPolyMesh::VertexIter v_it = mesh->vertices_sbegin(); v_it != mesh->vertices_end(); ++v_it)
    {
        // Add vertex to QuasiUniformMesh
        QuasiUniformMesh::VertexHandle vh = qum->add_vertex(mesh->point(*v_it));

        // Add the new vertex handle to each faces that the old vertex is attached to
        for(DefaultPolyMesh::VertexFaceIter vf_it = mesh->vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
        {
            mesh->property(face_vhs, *vf_it).push_back(vh);
        }
    }

    // Create all the faces (and automatically edges and halfedges) in the new QuasiUniformMesh
    for(DefaultPolyMesh::FaceIter f_it = mesh->faces_sbegin(); f_it != mesh->faces_end(); ++f_it)
    {
        DefaultPolyMesh::HalfedgeHandle heh_f1 = mesh->halfedge_handle(*f_it);
        DefaultPolyMesh::HalfedgeHandle heh_f2 = mesh->next_halfedge_handle(heh_f1);
        DefaultPolyMesh::HalfedgeHandle heh_f3 = mesh->next_halfedge_handle(heh_f2);
        DefaultPolyMesh::VertexHandle old_vh1 = mesh->to_vertex_handle(heh_f1);
        DefaultPolyMesh::VertexHandle old_vh2 = mesh->to_vertex_handle(heh_f2);
        DefaultPolyMesh::VertexHandle old_vh3 = mesh->to_vertex_handle(heh_f3);
        DefaultPolyMesh::Point p1 = mesh->point(old_vh1);
        DefaultPolyMesh::Point p2 = mesh->point(old_vh2);
        DefaultPolyMesh::Point p3 = mesh->point(old_vh3);

        std::vector<QuasiUniformMesh::VertexHandle> vertices = mesh->property(face_vhs, *f_it);

        std::vector<QuasiUniformMesh::VertexHandle> sorted_vertices;
        sorted_vertices.reserve(3);

        for(int i = 0; i < vertices.size(); i++)
        {
            QuasiUniformMesh::Point p = qum->point(vertices[i]);

            if(p[0] == p1[0] && p[1] == p1[1] && p[2] == p1[2])
                sorted_vertices[0] = vertices[i];
            else if(p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2])
                sorted_vertices[1] = vertices[i];
            else if(p[0] == p3[0] && p[1] == p3[1] && p[2] == p3[2])
                sorted_vertices[2] = vertices[i];
        }

        qum->add_face(sorted_vertices);
    }

    return *qum;
}

QuasiUniformMesh QuasiUniformMeshConverter::convert(DefaultTriMesh *mesh)
{
    QuasiUniformMesh *qum;

    OpenMesh::FPropHandleT<std::vector<QuasiUniformMesh::VertexHandle>> face_vhs;
    mesh->add_property(face_vhs);

    // Create all the vertices in the new QuasiUniformMesh
    for(DefaultTriMesh::VertexIter v_it = mesh->vertices_sbegin(); v_it != mesh->vertices_end(); ++v_it)
    {
        // Add vertex to QuasiUniformMesh
        QuasiUniformMesh::VertexHandle vh = qum->add_vertex(mesh->point(*v_it));

        // Add the new vertex handle to each faces that the old vertex is attached to
        for(DefaultTriMesh::VertexFaceIter vf_it = mesh->vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
        {
            mesh->property(face_vhs, *vf_it).push_back(vh);
        }
    }

    // Create all the faces (and automatically edges and halfedges) in the new QuasiUniformMesh
    for(DefaultTriMesh::FaceIter f_it = mesh->faces_sbegin(); f_it != mesh->faces_end(); ++f_it)
    {
        DefaultTriMesh::HalfedgeHandle heh_f1 = mesh->halfedge_handle(*f_it);
        DefaultTriMesh::HalfedgeHandle heh_f2 = mesh->next_halfedge_handle(heh_f1);
        DefaultTriMesh::HalfedgeHandle heh_f3 = mesh->next_halfedge_handle(heh_f2);
        DefaultTriMesh::VertexHandle old_vh1 = mesh->to_vertex_handle(heh_f1);
        DefaultTriMesh::VertexHandle old_vh2 = mesh->to_vertex_handle(heh_f2);
        DefaultTriMesh::VertexHandle old_vh3 = mesh->to_vertex_handle(heh_f3);
        DefaultTriMesh::Point p1 = mesh->point(old_vh1);
        DefaultTriMesh::Point p2 = mesh->point(old_vh2);
        DefaultTriMesh::Point p3 = mesh->point(old_vh3);

        std::vector<QuasiUniformMesh::VertexHandle> vertices = mesh->property(face_vhs, *f_it);

        std::vector<QuasiUniformMesh::VertexHandle> sorted_vertices;
        sorted_vertices.reserve(3);

        for(int i = 0; i < vertices.size(); i++)
        {
            QuasiUniformMesh::Point p = qum->point(vertices[i]);

            if(p[0] == p1[0] && p[1] == p1[1] && p[2] == p1[2])
                sorted_vertices[0] = vertices[i];
            else if(p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2])
                sorted_vertices[1] = vertices[i];
            else if(p[0] == p3[0] && p[1] == p3[1] && p[2] == p3[2])
                sorted_vertices[2] = vertices[i];
        }

        qum->add_face(sorted_vertices);
    }

    return *qum;
}
void QuasiUniformMeshConverter::makeUniform(QuasiUniformMesh *mesh, float edgeMin, float edgeMax)
{
}

void QuasiUniformMeshConverter::makeUniformField(QuasiUniformMesh *mesh, const std::vector<OpenMesh::VertexHandle> &region, float edgeMin, float edgeMax)
{
}
