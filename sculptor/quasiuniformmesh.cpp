#include "quasiuniformmesh.h"

void QuasiUniformMeshConverter::convert(DefaultPolyMesh *in, QuasiUniformMesh *out)
{
    out->request_halfedge_normals();

    OpenMesh::FPropHandleT<std::vector<QuasiUniformMesh::VertexHandle>> face_vhs;
    in->add_property(face_vhs);

    in->triangulate();

    // Create all the vertices in the new QuasiUniformMesh
    for(DefaultPolyMesh::VertexIter v_it = in->vertices_sbegin(); v_it != in->vertices_end(); ++v_it)
    {
        // Add vertex to QuasiUniformMesh
        QuasiUniformMesh::VertexHandle vh = out->add_vertex(in->point(*v_it));

        // Add the new vertex handle to each faces that the old vertex is attached to
        for(DefaultPolyMesh::VertexFaceIter vf_it = in->vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
        {
            in->property(face_vhs, *vf_it).push_back(vh);
        }
    }

    // Create all the faces (and automatically edges and halfedges) in the new QuasiUniformMesh
    for(DefaultPolyMesh::FaceIter f_it = in->faces_sbegin(); f_it != in->faces_end(); ++f_it)
    {
        DefaultPolyMesh::HalfedgeHandle heh_f1 = in->halfedge_handle(*f_it);
        DefaultPolyMesh::HalfedgeHandle heh_f2 = in->next_halfedge_handle(heh_f1);
        DefaultPolyMesh::HalfedgeHandle heh_f3 = in->next_halfedge_handle(heh_f2);
        DefaultPolyMesh::VertexHandle old_vh1 = in->to_vertex_handle(heh_f1);
        DefaultPolyMesh::VertexHandle old_vh2 = in->to_vertex_handle(heh_f2);
        DefaultPolyMesh::VertexHandle old_vh3 = in->to_vertex_handle(heh_f3);
        DefaultPolyMesh::Point p1 = in->point(old_vh1);
        DefaultPolyMesh::Point p2 = in->point(old_vh2);
        DefaultPolyMesh::Point p3 = in->point(old_vh3);

        std::vector<QuasiUniformMesh::VertexHandle> vertices = in->property(face_vhs, *f_it);

        QuasiUniformMesh::VertexHandle new_vh1, new_vh2, new_vh3;

        QuasiUniformMesh::Point p;

        for(int i = 0; i < vertices.size(); i++)
        {
            p = out->point(vertices[i]);

            if(p[0] == p1[0] && p[1] == p1[1] && p[2] == p1[2])
                new_vh1 = vertices[i];
            else if(p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2])
                new_vh2 = vertices[i];
            else if(p[0] == p3[0] && p[1] == p3[1] && p[2] == p3[2])
                new_vh3 = vertices[i];
        }

        out->add_face(new_vh1, new_vh2, new_vh3);
    }
}

void QuasiUniformMeshConverter::convert(QuasiUniformMesh *in, DefaultPolyMesh *out)
{
    out->request_halfedge_normals();

    OpenMesh::FPropHandleT<std::vector<DefaultPolyMesh::VertexHandle>> face_vhs;
    in->add_property(face_vhs);

    // Create all the vertices in the new DefaultPolyMesh
    for(QuasiUniformMesh::VertexIter v_it = in->vertices_sbegin(); v_it != in->vertices_end(); ++v_it)
    {
        // Add vertex to DefaultPolyMesh
        DefaultPolyMesh::VertexHandle vh = out->add_vertex(in->point(*v_it));

        // Add the new vertex handle to each faces that the old vertex is attached to
        for(QuasiUniformMesh::VertexFaceIter vf_it = in->vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
        {
            in->property(face_vhs, *vf_it).push_back(vh);
        }
    }

    // Create all the faces (and automatically edges and halfedges) in the new DefaultPolyMesh
    for(QuasiUniformMesh::FaceIter f_it = in->faces_sbegin(); f_it != in->faces_end(); ++f_it)
    {
        QuasiUniformMesh::HalfedgeHandle heh_f1 = in->halfedge_handle(*f_it);
        QuasiUniformMesh::HalfedgeHandle heh_f2 = in->next_halfedge_handle(heh_f1);
        QuasiUniformMesh::HalfedgeHandle heh_f3 = in->next_halfedge_handle(heh_f2);
        QuasiUniformMesh::VertexHandle old_vh1 = in->to_vertex_handle(heh_f1);
        QuasiUniformMesh::VertexHandle old_vh2 = in->to_vertex_handle(heh_f2);
        QuasiUniformMesh::VertexHandle old_vh3 = in->to_vertex_handle(heh_f3);
        QuasiUniformMesh::Point p1 = in->point(old_vh1);
        QuasiUniformMesh::Point p2 = in->point(old_vh2);
        QuasiUniformMesh::Point p3 = in->point(old_vh3);

        std::vector<DefaultPolyMesh::VertexHandle> vertices = in->property(face_vhs, *f_it);

        QuasiUniformMesh::VertexHandle new_vh1, new_vh2, new_vh3;

        QuasiUniformMesh::Point p;

        for(int i = 0; i < vertices.size(); i++)
        {
            p = out->point(vertices[i]);

            if(p[0] == p1[0] && p[1] == p1[1] && p[2] == p1[2])
                new_vh1 = vertices[i];
            else if(p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2])
                new_vh2 = vertices[i];
            else if(p[0] == p3[0] && p[1] == p3[1] && p[2] == p3[2])
                new_vh3 = vertices[i];
        }

        out->add_face(new_vh1, new_vh2, new_vh3);
    }
}

void QuasiUniformMeshConverter::convert(DefaultTriMesh *in, QuasiUniformMesh *out)
{
    out->request_halfedge_normals();

    OpenMesh::FPropHandleT<std::vector<QuasiUniformMesh::VertexHandle>> face_vhs;
    in->add_property(face_vhs);

    // Create all the vertices in the new QuasiUniformMesh
    for(DefaultTriMesh::VertexIter v_it = in->vertices_sbegin(); v_it != in->vertices_end(); ++v_it)
    {
        // Add vertex to QuasiUniformMesh
        QuasiUniformMesh::VertexHandle vh = out->add_vertex(in->point(*v_it));

        // Add the new vertex handle to each faces that the old vertex is attached to
        for(DefaultTriMesh::VertexFaceIter vf_it = in->vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
        {
            in->property(face_vhs, *vf_it).push_back(vh);
        }
    }

    // Create all the faces (and automatically edges and halfedges) in the new QuasiUniformMesh
    for(DefaultTriMesh::FaceIter f_it = in->faces_sbegin(); f_it != in->faces_end(); ++f_it)
    {
        DefaultTriMesh::HalfedgeHandle heh_f1 = in->halfedge_handle(*f_it);
        DefaultTriMesh::HalfedgeHandle heh_f2 = in->next_halfedge_handle(heh_f1);
        DefaultTriMesh::HalfedgeHandle heh_f3 = in->next_halfedge_handle(heh_f2);
        DefaultTriMesh::VertexHandle old_vh1 = in->to_vertex_handle(heh_f1);
        DefaultTriMesh::VertexHandle old_vh2 = in->to_vertex_handle(heh_f2);
        DefaultTriMesh::VertexHandle old_vh3 = in->to_vertex_handle(heh_f3);
        DefaultTriMesh::Point p1 = in->point(old_vh1);
        DefaultTriMesh::Point p2 = in->point(old_vh2);
        DefaultTriMesh::Point p3 = in->point(old_vh3);

        std::vector<QuasiUniformMesh::VertexHandle> vertices = in->property(face_vhs, *f_it);

        QuasiUniformMesh::VertexHandle new_vh1, new_vh2, new_vh3;

        QuasiUniformMesh::Point p;

        for(int i = 0; i < vertices.size(); i++)
        {
            p = out->point(vertices[i]);

            if(p[0] == p1[0] && p[1] == p1[1] && p[2] == p1[2])
                new_vh1 = vertices[i];
            else if(p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2])
                new_vh2 = vertices[i];
            else if(p[0] == p3[0] && p[1] == p3[1] && p[2] == p3[2])
                new_vh3 = vertices[i];
        }

        out->add_face(new_vh1, new_vh2, new_vh3);
    }
}

void QuasiUniformMeshConverter::convert(QuasiUniformMesh *in, DefaultTriMesh *out)
{
    out->request_halfedge_normals();

    OpenMesh::FPropHandleT<std::vector<DefaultTriMesh::VertexHandle>> face_vhs;
    in->add_property(face_vhs);

    // Create all the vertices in the new DefaultTriMesh
    for(QuasiUniformMesh::VertexIter v_it = in->vertices_sbegin(); v_it != in->vertices_end(); ++v_it)
    {
        // Add vertex to DefaultTriMesh
        DefaultTriMesh::VertexHandle vh = out->add_vertex(in->point(*v_it));

        // Add the new vertex handle to each faces that the old vertex is attached to
        for(QuasiUniformMesh::VertexFaceIter vf_it = in->vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
        {
            in->property(face_vhs, *vf_it).push_back(vh);
        }
    }

    // Create all the faces (and automatically edges and halfedges) in the new DefaultTriMesh
    for(QuasiUniformMesh::FaceIter f_it = in->faces_sbegin(); f_it != in->faces_end(); ++f_it)
    {
        QuasiUniformMesh::HalfedgeHandle heh_f1 = in->halfedge_handle(*f_it);
        QuasiUniformMesh::HalfedgeHandle heh_f2 = in->next_halfedge_handle(heh_f1);
        QuasiUniformMesh::HalfedgeHandle heh_f3 = in->next_halfedge_handle(heh_f2);
        QuasiUniformMesh::VertexHandle old_vh1 = in->to_vertex_handle(heh_f1);
        QuasiUniformMesh::VertexHandle old_vh2 = in->to_vertex_handle(heh_f2);
        QuasiUniformMesh::VertexHandle old_vh3 = in->to_vertex_handle(heh_f3);
        QuasiUniformMesh::Point p1 = in->point(old_vh1);
        QuasiUniformMesh::Point p2 = in->point(old_vh2);
        QuasiUniformMesh::Point p3 = in->point(old_vh3);

        std::vector<DefaultTriMesh::VertexHandle> vertices = in->property(face_vhs, *f_it);

        QuasiUniformMesh::VertexHandle new_vh1, new_vh2, new_vh3;

        QuasiUniformMesh::Point p;

        for(int i = 0; i < vertices.size(); i++)
        {
            p = out->point(vertices[i]);

            if(p[0] == p1[0] && p[1] == p1[1] && p[2] == p1[2])
                new_vh1 = vertices[i];
            else if(p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2])
                new_vh2 = vertices[i];
            else if(p[0] == p3[0] && p[1] == p3[1] && p[2] == p3[2])
                new_vh3 = vertices[i];
        }

        out->add_face(new_vh1, new_vh2, new_vh3);
    }
}

void QuasiUniformMeshConverter::makeUniform(QuasiUniformMesh *mesh, float edgeMin, float edgeMax)
{
    float d = 0.2f;
    float ddetail = 0.5f;

    // Compliance to d
    for (QuasiUniformMesh::EdgeIter e_it = mesh->edges_sbegin(); e_it != mesh->edges_end(); ++e_it)
    {
        if(mesh->calc_edge_length(*e_it) < d)
        {
            QuasiUniformMesh::HalfedgeHandle heh = mesh->halfedge_handle(*e_it, 0);
            QuasiUniformMesh::VertexHandle vh1 = mesh->to_vertex_handle(heh);
            QuasiUniformMesh::VertexHandle vh2 = mesh->from_vertex_handle(heh);
            QuasiUniformMesh::Point p1 = mesh->point(vh1);
            QuasiUniformMesh::Point p2 = mesh->point(vh2);
            QuasiUniformMesh::Point new_p = (p1 + p2)/2;

            if(mesh->is_collapse_ok(heh))
            {
                mesh->collapse(heh);
                mesh->set_point(vh1, new_p);
            }
        }
    }

    // ddetail tight mesh
    for (QuasiUniformMesh::EdgeIter e_it = mesh->edges_sbegin(); e_it != mesh->edges_end(); ++e_it)
    {
        if(mesh->calc_edge_length(*e_it) > ddetail)
        {
            QuasiUniformMesh::HalfedgeHandle heh0 = mesh->halfedge_handle(*e_it, 0);
            QuasiUniformMesh::HalfedgeHandle heh1 = mesh->halfedge_handle(*e_it, 1);
            QuasiUniformMesh::VertexHandle vh1 = mesh->to_vertex_handle(heh0);
            QuasiUniformMesh::VertexHandle vh2 = mesh->from_vertex_handle(heh0);
            QuasiUniformMesh::Point p1 = mesh->point(vh1);
            QuasiUniformMesh::Point p2 = mesh->point(vh2);
            QuasiUniformMesh::Point new_p = (p1 + p2)/2;
            QuasiUniformMesh::VertexHandle new_vh = mesh->add_vertex(new_p);

            if(mesh->is_boundary(*e_it))
            {
                QuasiUniformMesh::VertexHandle vh3;
                mesh->delete_edge(*e_it, false);

                if(mesh->is_boundary(heh0))
                {
                    vh3 = mesh->to_vertex_handle(mesh->next_halfedge_handle(heh0));
                    mesh->add_face(vh2, new_vh, vh3);
                    mesh->add_face(new_vh, vh1, vh3);
                }
                else
                {
                    vh3 = mesh->to_vertex_handle(mesh->next_halfedge_handle(heh1));
                    mesh->add_face(vh1, new_vh, vh3);
                    mesh->add_face(new_vh, vh2, vh3);
                }
            }
            else
            {
                QuasiUniformMesh::FaceHandle fh = mesh->remove_edge(*e_it);
                mesh->split(fh, new_vh);
            }
        }
        /// problème : les longueurs ne sont pas les vraies (non scale)
        /// essayer de calculer la longueur manuellement en prenant les coordonnées des points
        //printf("length: %f\n", length);
    }

    mesh->garbage_collection();
}

void QuasiUniformMeshConverter::makeUniformField(QuasiUniformMesh *mesh, const std::vector<OpenMesh::VertexHandle> &region, float edgeMin, float edgeMax)
{
}
