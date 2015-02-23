#include "sculptorparameters.h"
#include "topologicalhandler.h"
#include "sculptor.h"


TopologicalHandler::TopologicalHandler(Sculptor *sculptor) :
    sculptor(sculptor)
{}

void TopologicalHandler::handleJoinVertex(OpenMesh::VertexHandle &v1, OpenMesh::VertexHandle &v2)
{
    QuasiUniformMesh::Point v1Courant;
    QuasiUniformMesh::Point v2Courant;
    int valence = 0;
    float plusProche = FLT_MAX;
    float secondPlusProche = FLT_MAX;
    int idCpt = 0;

    //Sommet courant du second 1-ring
    struct VertexSecondRing
    {
        //Distance du sommet par rapport au somment courant du premier 1-ring
        float dist;
        QuasiUniformMesh::VertexHandle v;
        //Nombre de fois où le sommet est lié à un sommet du premier 1-ring
        int nbUsing = 0;
    };

    //Calcul de la valence du second 1-ring
    valence = sculptor->getQUM()->valence(v2);

    VertexSecondRing *vsr = new VertexSecondRing[valence];

    //parcour des vertex du premier 1-ring
    for (QuasiUniformMesh::VertexVertexIter vv_it = sculptor->getQUM()->vv_iter(v1); vv_it.is_valid(); ++vv_it)
    {
        v1Courant = sculptor->getQUM()->point(*vv_it);
        float dist;

        //Tableau de VertexSecondRing pour stocker les distances des sommets du second 1-ring avec le sommet courant vv_it
        //Calcul des deux distances les plus proches du point courant
        for (QuasiUniformMesh::VertexVertexIter vv_it2 = sculptor->getQUM()->vv_iter(v2); vv_it2.is_valid(); ++vv_it2)
        {
            v2Courant = sculptor->getQUM()->point(*vv_it2);
            dist = sculptor->calcDist(v1Courant, v2Courant);
            vsr[idCpt].dist = dist;
            vsr[idCpt].v = *vv_it2;
            idCpt++;
        }

        //Calcul du min1 et min2
        //Construction de la nouvelle face entre les deux anneaux
        QuasiUniformMesh::VertexHandle vhandle[3];
        vhandle[0] = *vv_it;
        int idPlusProche;
        int idSecondPlusProche;
        for (int i = 0; i < valence; ++i) {
            if (vsr[i].dist < plusProche && vsr[i].nbUsing < 2) {
                secondPlusProche = plusProche;
                plusProche = vsr[i].dist;
                vhandle[1] = vsr[i].v;
                idPlusProche = i;
            }

            if(vsr[i].dist < secondPlusProche && vsr[i].dist > plusProche && vsr[i].nbUsing < 2)
            {
                secondPlusProche = vsr[i].dist;
                vhandle[2] = vsr[i].v;
                idSecondPlusProche = i;
            }
        }


        std::vector<QuasiUniformMesh::VertexHandle> face_vhandles;
        face_vhandles.push_back(vhandle[0]);
        QuasiUniformMesh::VertexHandle vCourant;
        for(QuasiUniformMesh::VertexOHalfedgeIter voh_it = sculptor->getQUM()->voh_begin(vhandle[1]); sculptor->getQUM()->voh_end(vhandle[1]); ++voh_it)
        {
            //Test si le next halfedge pointe vers le sommet du premier 1-ring
            vCourant = sculptor->getQUM()->to_vertex_handle(sculptor->getQUM()->next_halfedge_handle(voh_it));
            if (isSameVertexHandle(vCourant, vhandle[1])) {
                face_vhandles.push_back(vhandle[2]);
                vsr[idSecondPlusProche].nbUsing++;
                face_vhandles.push_back(vhandle[1]);
                vsr[idPlusProche].nbUsing++;
                break;
            }
        }

        for(QuasiUniformMesh::VertexOHalfedgeIter voh_it = sculptor->getQUM()->voh_begin(vhandle[1]); sculptor->getQUM()->voh_end(vhandle[1]); ++voh_it)
        {
            //Test si le next halfedge pointe vers le sommet du premier 1-ring
            vCourant = sculptor->getQUM()->to_vertex_handle(sculptor->getQUM()->next_halfedge_handle(voh_it));
            if (isSameVertexHandle(vCourant, vhandle[2])) {
                face_vhandles.push_back(vhandle[1]);
                vsr[idPlusProche].nbUsing++;
                face_vhandles.push_back(vhandle[2]);
                vsr[idSecondPlusProche].nbUsing++;
                break;
            }
        }
        QuasiUniformMesh::FaceHandle new_f = sculptor->getQUM()->add_face(face_vhandles);
        face_vhandles.clear();

        for(QuasiUniformMesh::FaceEdgeIter fe_it = sculptor->getQUM()->fe_iter(new_f); fe_it.is_valid(); ++fe_it)
        {
            sculptor->getConnectingEdges().push_back(*fe_it);
        }
        plusProche = FLT_MAX;
        secondPlusProche = FLT_MAX;
        idCpt = 0;
        valence = 0;
    }

    sculptor->getQUM()->garbage_collection();
}

void TopologicalHandler::cleanup(OpenMesh::VertexHandle &v1)
{
    QuasiUniformMesh *mesh = sculptor->getQUM();
    QuasiUniformMesh::FaceHandle fh1;
    QuasiUniformMesh::FaceHandle fh2;

    //Destruction des faces voisines ayant les mêmes sommets
    for(QuasiUniformMesh::VertexFaceIter vf_it = mesh->vf_iter(v1); vf_it.is_valid(); ++vf_it)
    {
        fh1 = *vf_it;
        for(QuasiUniformMesh::VertexFaceIter vf_it2 = mesh->vf_iter(v1); vf_it.is_valid(); ++vf_it2)
        {
            fh2 = *vf_it2;
            if (hasOneEdgeSame(fh1, fh2)) {
                if (hasSameVertices(fh1, fh2)) {
                    mesh->delete_face(fh1, false);
                    mesh->delete_face(fh2, false);
                }
            }
        }
    }

    //Destruction des faces non voisines ayant les mêmes sommets
    for(QuasiUniformMesh::VertexFaceIter vf_it = mesh->vf_iter(v1); vf_it.is_valid(); ++vf_it)
    {
        fh1 = *vf_it;
        for(QuasiUniformMesh::VertexFaceIter vf_it2 = mesh->vf_iter(v1); vf_it.is_valid(); ++vf_it2)
        {
            fh2 = *vf_it2;
            if (!hasOneEdgeSame(fh1, fh2)) {
                if (hasSameVertices(fh1, fh2)) {
                    mesh->delete_face(fh1, false);
                    mesh->delete_face(fh2, false);
                }
            }
        }
    }

    QuasiUniformMesh::EdgeHandle eh1;
    QuasiUniformMesh::EdgeHandle eh2;
    //Destruction des edges qui ont leur sommets en commun
    for(QuasiUniformMesh::VertexEdgeIter ve_it = mesh->ve_iter(v1); ve_it.is_valid(); ++ve_it)
    {
        eh1 = *ve_it;
        for(QuasiUniformMesh::VertexEdgeIter ve_it2 = mesh->ve_iter(v1); ve_it2.is_valid(); ++ve_it2)
        {
            eh2 = *ve_it2;
            if (hasSameVertices(eh1, eh2)) {
                mesh->delete_edge(eh1);
                mesh->delete_edge(eh2);
            }
        }
    }

    mesh->garbage_collection();
}

//Détermine si deux faces ont un edge en commun
bool TopologicalHandler::hasOneEdgeSame(OpenMesh::FaceHandle &fh1, OpenMesh::FaceHandle &fh2)
{
    bool hasAnEdgeSame = false;
    QuasiUniformMesh *mesh = sculptor->getQUM();
    QuasiUniformMesh::EdgeHandle eh1;
    QuasiUniformMesh::EdgeHandle eh2;
    QuasiUniformMesh::Point p1_1;
    QuasiUniformMesh::Point p1_2;
    QuasiUniformMesh::Point p2_1;
    QuasiUniformMesh::Point p2_2;

    for (QuasiUniformMesh::FaceEdgeIter fe_it = mesh->fe_iter(fh1); fe_it.is_valid(); ++fe_it) {
        eh1 = *fe_it;
        for (QuasiUniformMesh::FaceEdgeIter fe_it2 = mesh->fe_iter(fh2); fe_it2.is_valid(); ++fe_it2) {
            eh2 = *fe_it2;
            QuasiUniformMesh::HalfedgeHandle heh1 = mesh->halfedge_handle(eh1, 0);
            QuasiUniformMesh::HalfedgeHandle heh2 = mesh->halfedge_handle(eh2, 0);

            p1_1 = mesh->point(mesh->to_vertex_handle(heh1));
            p1_2 = mesh->point(mesh->from_vertex_handle(heh1));
            p2_1 = mesh->point(mesh->to_vertex_handle(heh2));
            p2_2 = mesh->point(mesh->from_vertex_handle(heh2));

            hasAnEdgeSame = (p1_1[0] == p2_1[0] && p1_1[1] == p2_1[1] && p1_1[2] == p2_1[2] && p1_2[0] == p2_2[0] && p1_2[1] == p2_2[1] && p1_2[2] == p2_2[2]);
            if (hasAnEdgeSame) {
                continue;
            }
        }
        if (hasAnEdgeSame) {
            continue;
        }
    }

    return hasAnEdgeSame;
}

//Test si deux faces ont strictement les mêmes sommets
bool TopologicalHandler::hasSameVertices(OpenMesh::FaceHandle &fh1, OpenMesh::FaceHandle &fh2)
{
    bool hasSameVertices = true;
    QuasiUniformMesh *mesh = sculptor->getQUM();
    QuasiUniformMesh::VertexHandle vh1;
    QuasiUniformMesh::VertexHandle vh2;
    QuasiUniformMesh::Point p1;
    QuasiUniformMesh::Point p2;

    for (QuasiUniformMesh::FaceVertexIter fv_it = mesh->fv_iter(fh1); fv_it.is_valid(); ++fv_it) {
        vh1 = *fv_it;
        p1 = mesh->point(vh1);
        for (QuasiUniformMesh::FaceVertexIter fv_it2 = mesh->fv_iter(fh2); fv_it2.is_valid(); ++fv_it2) {
            vh2 = *fv_it2;
            p2 = mesh->point(vh2);
            hasSameVertices = (p1[0] != p2[0] || p1[1] != p2[1] || p1[2] != p2[2]);
            if (hasSameVertices) {
                continue;
            }
        }
        if (hasSameVertices) {
            continue;
        }
    }
    return hasSameVertices;
}

bool TopologicalHandler::hasSameVertices(OpenMesh::EdgeHandle &eh1, OpenMesh::EdgeHandle &eh2)
{
    QuasiUniformMesh *mesh = sculptor->getQUM();
    QuasiUniformMesh::HalfedgeHandle heh1 = mesh->halfedge_handle(eh1, 0);
    QuasiUniformMesh::HalfedgeHandle heh2 = mesh->halfedge_handle(eh2, 0);
    QuasiUniformMesh::Point p1_1 = mesh->point(mesh->to_vertex_handle(heh1));
    QuasiUniformMesh::Point p1_2 = mesh->point(mesh->from_vertex_handle(heh1));
    QuasiUniformMesh::Point p2_1 = mesh->point(mesh->to_vertex_handle(heh2));
    QuasiUniformMesh::Point p2_2 = mesh->point(mesh->from_vertex_handle(heh2));

    return (p1_1[0] == p2_1[0] && p1_1[1] == p2_1[1] && p1_1[2] == p2_1[2] && p1_2[0] == p2_2[0] && p1_2[1] == p2_2[1] && p1_2[2] == p2_2[2]);
}

bool TopologicalHandler::isSameVertexHandle(OpenMesh::VertexHandle &v1, OpenMesh::VertexHandle &v2)
{
    QuasiUniformMesh::Point p1 = sculptor->getQUM()->point(v1);
    QuasiUniformMesh::Point p2 = sculptor->getQUM()->point(v2);
    return (p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2]);
}

