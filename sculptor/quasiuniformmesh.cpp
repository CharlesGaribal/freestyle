#include "quasiuniformmesh.h"
#include "sculptor.h"

void QuasiUniformMeshConverter::makeUniform(QuasiUniformMesh &mesh, float edgeMin, float edgeMax)
{
    // Compliance to edgeMin
    for (QuasiUniformMesh::EdgeIter e_it = mesh.edges_sbegin(); e_it != mesh.edges_end(); ++e_it)
    {
        if(mesh.calc_edge_length(*e_it) < edgeMin)
        {
            QuasiUniformMesh::HalfedgeHandle heh = mesh.halfedge_handle(*e_it, 0);
            QuasiUniformMesh::VertexHandle vh1 = mesh.to_vertex_handle(heh);
            QuasiUniformMesh::VertexHandle vh2 = mesh.from_vertex_handle(heh);
            QuasiUniformMesh::Point p1 = mesh.point(vh1);
            QuasiUniformMesh::Point p2 = mesh.point(vh2);
            QuasiUniformMesh::Point new_p = (p1 + p2)/2;
            if(mesh.is_collapse_ok(heh))
            {
                mesh.collapse(heh);
                mesh.set_point(vh1, new_p);
            }
        }
    }

    // edgeMax tight mesh
    for (QuasiUniformMesh::EdgeIter e_it = mesh.edges_sbegin(); e_it != mesh.edges_end(); ++e_it)
    {
        if(mesh.calc_edge_length(*e_it) > edgeMax)
        {
            QuasiUniformMesh::HalfedgeHandle heh0 = mesh.halfedge_handle(*e_it, 0);
            QuasiUniformMesh::HalfedgeHandle heh1 = mesh.halfedge_handle(*e_it, 1);
            QuasiUniformMesh::VertexHandle vh1 = mesh.to_vertex_handle(heh0);
            QuasiUniformMesh::VertexHandle vh2 = mesh.from_vertex_handle(heh0);
            QuasiUniformMesh::Point p1 = mesh.point(vh1);
            QuasiUniformMesh::Point p2 = mesh.point(vh2);
            QuasiUniformMesh::Point new_p = (p1 + p2)/2;
            QuasiUniformMesh::VertexHandle new_vh = mesh.add_vertex(new_p);
            if(mesh.is_boundary(*e_it))
            {
                QuasiUniformMesh::VertexHandle vh3;
                mesh.delete_edge(*e_it, false);

                if(mesh.is_boundary(heh0))
                {
                    vh3 = mesh.to_vertex_handle(mesh.next_halfedge_handle(heh0));
                    mesh.add_face(vh2, new_vh, vh3);
                    mesh.add_face(new_vh, vh1, vh3);
                }
                else
                {
                    vh3 = mesh.to_vertex_handle(mesh.next_halfedge_handle(heh1));
                    mesh.add_face(vh1, new_vh, vh3);
                    mesh.add_face(new_vh, vh2, vh3);
                }
            }
            else
            {
                QuasiUniformMesh::FaceHandle fh = mesh.remove_edge(*e_it);
                mesh.split(fh, new_vh);
            }
        }
    }

    mesh.garbage_collection();
}

void QuasiUniformMeshConverter::makeUniformField(QuasiUniformMesh &mesh, const std::vector<OpenMesh::EdgeHandle> &field, float edgeMin, float edgeMax)
{
    // Compliance to edgeMin
    for (int i = 0; i < (int) field.size(); ++i)
    {
        OpenMesh::EdgeHandle eh = field[i];

        if(mesh.is_valid_handle(eh) && mesh.calc_edge_length(eh) < edgeMin)
        {
            QuasiUniformMesh::HalfedgeHandle heh = mesh.halfedge_handle(eh, 0);
            QuasiUniformMesh::VertexHandle vh1 = mesh.to_vertex_handle(heh);
            QuasiUniformMesh::VertexHandle vh2 = mesh.from_vertex_handle(heh);
            QuasiUniformMesh::Point p1 = mesh.point(vh1);
            QuasiUniformMesh::Point p2 = mesh.point(vh2);
            QuasiUniformMesh::Point new_p = (p1 + p2)/2;

            if(mesh.is_collapse_ok(heh))
            {
                mesh.collapse(heh);
                mesh.set_point(vh1, new_p);
            }
        }
    }

    // edgeMax tight mesh
    for (int i = 0; i < (int) field.size(); ++i)
    {
        OpenMesh::EdgeHandle eh = field[i];

        if(mesh.is_valid_handle(eh) && mesh.calc_edge_length(eh) > edgeMax)
        {
            QuasiUniformMesh::HalfedgeHandle heh0 = mesh.halfedge_handle(eh, 0);
            QuasiUniformMesh::HalfedgeHandle heh1 = mesh.halfedge_handle(eh, 1);
            QuasiUniformMesh::VertexHandle vh1 = mesh.to_vertex_handle(heh0);
            QuasiUniformMesh::VertexHandle vh2 = mesh.from_vertex_handle(heh0);
            QuasiUniformMesh::Point p1 = mesh.point(vh1);
            QuasiUniformMesh::Point p2 = mesh.point(vh2);
            QuasiUniformMesh::Point new_p = (p1 + p2)/2;
            QuasiUniformMesh::VertexHandle new_vh = mesh.add_vertex(new_p);
            if(mesh.is_boundary(eh))
            {
                QuasiUniformMesh::VertexHandle vh3;
                mesh.delete_edge(eh, false);

                if(mesh.is_boundary(heh0))
                {
                    vh3 = mesh.to_vertex_handle(mesh.next_halfedge_handle(heh0));
                    mesh.add_face(vh2, new_vh, vh3);
                    mesh.add_face(new_vh, vh1, vh3);
                }
                else
                {
                    vh3 = mesh.to_vertex_handle(mesh.next_halfedge_handle(heh1));
                    mesh.add_face(vh1, new_vh, vh3);
                    mesh.add_face(new_vh, vh2, vh3);
                }
            }
            else
            {
                bool a = mesh.status(eh).deleted();
                bool b = mesh.is_simple_link(eh);
                QuasiUniformMesh::FaceHandle fh = mesh.remove_edge(eh);
                mesh.split(fh, new_vh);
            }
        }
    }

    mesh.garbage_collection();
}
