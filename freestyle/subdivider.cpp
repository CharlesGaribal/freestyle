#include "subdivider.h"

#define PI 3.14159265359

void Subdivider::subdivide(QuasiUniformMesh &omesh)
{
    //Vertex Property pour la position après déplacement des sommets originaux
    OpenMesh::VPropHandleT<QuasiUniformMesh::Point> pts_v;

    //Edge Property pour le handle des sommets inséré
    OpenMesh::EPropHandleT<QuasiUniformMesh::VertexHandle> pts_e;

    //Face Property pour savoir si c'est une des faces originales ou créées
    OpenMesh::FPropHandleT<bool> new_face;

    //Ajout des diverses Property
    omesh.add_property(pts_v);
    omesh.add_property(pts_e);
    omesh.add_property(new_face);
    int valence;

    // Déplacement (virtuel, stockage position dans property)
    for (QuasiUniformMesh::VertexIter v_it = omesh.vertices_begin(); v_it != omesh.vertices_end(); ++v_it)
    {
        QuasiUniformMesh::Point p = omesh.point(*v_it);
        QuasiUniformMesh::Point new_p;

        if(omesh.is_boundary(*v_it))
        {
            new_p = 0.75f*p;

            //Si le sommet est boundary, le halfedge_handle retourné est celui qui est au bord
            QuasiUniformMesh::HalfedgeHandle heh = omesh.halfedge_handle(*v_it);

            new_p += 0.125f*omesh.point(omesh.to_vertex_handle(heh));
            new_p += 0.125f*omesh.point(omesh.from_vertex_handle(omesh.prev_halfedge_handle(heh)));
        }
        else
        {
            valence = omesh.valence(*v_it);

            // Valeur du poid des points adjacents au point courant
            float beta = (1.f/valence)*(0.625 - pow((0.375 + 0.25*cos(2*PI/valence)), 2));

            new_p = (1-valence*beta)*p;

            // Calcul des positions avec les poids.
            for (QuasiUniformMesh::VertexVertexIter vv_it = omesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
            {
                new_p += beta*omesh.point(*vv_it);
            }
        }

        //Ajout à la Property du sommet
        omesh.property(pts_v, *v_it) = new_p;
    }

    //Insertion (création des sommets et stockage du handle dans une property du edge)
    for (QuasiUniformMesh::EdgeIter e_it = omesh.edges_begin(); e_it != omesh.edges_end(); ++e_it)
    {
        //Récupération des sommets influent pour le calcul du nouveau sommet
        QuasiUniformMesh::HalfedgeHandle heh1 = omesh.halfedge_handle(*e_it, 0);
        QuasiUniformMesh::HalfedgeHandle heh2 = omesh.halfedge_handle(*e_it, 1);
        QuasiUniformMesh::VertexHandle vh1 = omesh.to_vertex_handle(heh1);
        QuasiUniformMesh::VertexHandle vh2 = omesh.to_vertex_handle(heh2);
        QuasiUniformMesh::VertexHandle vh3 = omesh.to_vertex_handle(omesh.next_halfedge_handle(heh1));
        QuasiUniformMesh::VertexHandle vh4 = omesh.to_vertex_handle(omesh.next_halfedge_handle(heh2));
        QuasiUniformMesh::Point p1 = omesh.point(vh1);
        QuasiUniformMesh::Point p2 = omesh.point(vh2);
        QuasiUniformMesh::Point p3 = omesh.point(vh3);
        QuasiUniformMesh::Point p4 = omesh.point(vh4);
        QuasiUniformMesh::Point new_p;

        //Calcul des coordonnées du nouveau sommet
        if(omesh.is_boundary(*e_it))
            new_p = 0.5f*p1 + 0.5f*p2;
        else
            new_p = 0.375f*p1 + 0.375f*p2 + 0.125f*p3 + 0.125f*p4;

        //Ajout du sommet au maillage sans le connecter
        QuasiUniformMesh::VertexHandle new_vh = omesh.new_vertex(new_p);

        //Pour pas qu'il ne bouge au moment du déplacement des anciens sommets
        omesh.property(pts_v, new_vh) = new_p;

        //Pour pouvoir accéder plus tard au nouveau sommet depuis l'arête
        omesh.property(pts_e, *e_it) = new_vh;
    }

    //Insertion (création des faces)
    for (QuasiUniformMesh::FaceIter f_it = omesh.faces_sbegin(); f_it != omesh.faces_end(); ++f_it)
    {
        //Si c'est une des faces originales
        if(omesh.property(new_face, *f_it) == NULL)
        {
            //Récupération des handle des sommets de la face et des nouveaux sommets
            QuasiUniformMesh::HalfedgeHandle heh_f1 = omesh.halfedge_handle(*f_it);
            QuasiUniformMesh::HalfedgeHandle heh_f2 = omesh.next_halfedge_handle(heh_f1);
            QuasiUniformMesh::HalfedgeHandle heh_f3 = omesh.next_halfedge_handle(heh_f2);
            QuasiUniformMesh::VertexHandle vh1 = omesh.to_vertex_handle(heh_f1);
            QuasiUniformMesh::VertexHandle vh2 = omesh.to_vertex_handle(heh_f2);
            QuasiUniformMesh::VertexHandle vh3 = omesh.to_vertex_handle(heh_f3);
            QuasiUniformMesh::VertexHandle vh4 = omesh.property(pts_e, omesh.edge_handle(heh_f2));
            QuasiUniformMesh::VertexHandle vh5 = omesh.property(pts_e, omesh.edge_handle(heh_f3));
            QuasiUniformMesh::VertexHandle vh6 = omesh.property(pts_e, omesh.edge_handle(heh_f1));

            //Suppression de la face sans supprimer les éventuels sommets isolés car ils vont être réutilisés
            omesh.delete_face(*f_it, false);

            //Création des nouvelles faces et indication que ce sont des nouvelles faces
            omesh.property(new_face, omesh.add_face(vh1, vh4, vh6)) = true;
            omesh.property(new_face, omesh.add_face(vh2, vh5, vh4)) = true;
            omesh.property(new_face, omesh.add_face(vh3, vh6, vh5)) = true;
            omesh.property(new_face, omesh.add_face(vh4, vh5, vh6)) = true;
        }
    }

    //Déplacement réel des anciens sommets
    for (QuasiUniformMesh::VertexIter v_it = omesh.vertices_begin(); v_it != omesh.vertices_end(); ++v_it)
    {
        omesh.set_point(*v_it, omesh.property(pts_v, *v_it));
    }

    omesh.garbage_collection();
}


