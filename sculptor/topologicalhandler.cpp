#include "sculptorparameters.h"
#include "topologicalhandler.h"
#include "sculptor.h"


TopologicalHandler::TopologicalHandler(Sculptor *sculptor) :
    sculptor(sculptor)
{}

void TopologicalHandler::handleJoinVertex(OpenMesh::VertexHandle &v1, OpenMesh::VertexHandle &v2)
{
    std::vector<QuasiUniformMesh::VertexHandle> verticesARing;
    std::vector<QuasiUniformMesh::VertexHandle> verticesBRing;

    std::vector<QuasiUniformMesh::EdgeHandle> edgesARing;
    std::vector<QuasiUniformMesh::EdgeHandle> edgesBRing;

    //Stockage des sommets des deux anneaux à fusionner
    for (QuasiUniformMesh::VertexVertexIter vv_it = sculptor->getQUM()->vv_iter(v1); vv_it.is_valid(); ++vv_it)
    {
        verticesARing.push_back(*vv_it);
    }

    for (QuasiUniformMesh::VertexVertexIter vv_it = sculptor->getQUM()->vv_iter(v2); vv_it.is_valid(); ++vv_it)
    {
        verticesBRing.push_back(*vv_it);
    }

    for (QuasiUniformMesh::VertexEdgeIter vv_it = sculptor->getQUM()->ve_iter(v1); vv_it.is_valid(); ++vv_it)
    {
        edgesARing.push_back(*vv_it);
    }

    for (QuasiUniformMesh::VertexEdgeIter vv_it = sculptor->getQUM()->ve_iter(v2); vv_it.is_valid(); ++vv_it)
    {
        edgesBRing.push_back(*vv_it);
    }

    int valenceA = sculptor->getQUM()->valence(v1), valenceB = sculptor->getQUM()->valence(v2);

    // Flip until valences are equals
    if(valenceA > valenceB)
    {
        int diff = valenceA - valenceB;
        int nbFlip = 0;
        bool unSurDeux = true;

        for(int i = 0; i < edgesARing.size(); ++i)
        {
            if(unSurDeux)
            {
                sculptor->getQUM()->flip(edgesARing[i]);

                nbFlip++;

                if(nbFlip == diff)
                    break;
            }

            unSurDeux = !unSurDeux;
        }
    }
    else if(valenceB > valenceA)
    {
        int diff = valenceB - valenceA;
        int nbFlip = 0;
        bool unSurDeux = true;

        for(int i = 0; i < edgesBRing.size(); ++i)
        {
            if(unSurDeux)
            {
                sculptor->getQUM()->flip(edgesBRing[i]);

                nbFlip++;

                if(nbFlip == diff)
                    break;
            }

            unSurDeux = !unSurDeux;
        }
    }

    valenceA = sculptor->getQUM()->valence(v1);
    valenceB = sculptor->getQUM()->valence(v2);

    edgesARing.clear();
    edgesBRing.clear();

    for (QuasiUniformMesh::VertexEdgeIter vv_it = sculptor->getQUM()->ve_iter(v1); vv_it.is_valid(); ++vv_it)
    {
        edgesARing.push_back(*vv_it);
    }

    for (QuasiUniformMesh::VertexEdgeIter vv_it = sculptor->getQUM()->ve_iter(v2); vv_it.is_valid(); ++vv_it)
    {
        edgesBRing.push_back(*vv_it);
    }

    verticesARing.clear();
    verticesBRing.clear();

    for (QuasiUniformMesh::VertexVertexIter vv_it = sculptor->getQUM()->vv_iter(v1); vv_it.is_valid(); ++vv_it)
    {
        verticesARing.push_back(*vv_it);
    }

    for (QuasiUniformMesh::VertexVertexIter vv_it = sculptor->getQUM()->vv_iter(v2); vv_it.is_valid(); ++vv_it)
    {
        verticesBRing.push_back(*vv_it);
    }

    //Destruction des faces inutiles pour la fusion
    std::vector<QuasiUniformMesh::FaceHandle> v;

    for (QuasiUniformMesh::VertexFaceIter vf_it = sculptor->getQUM()->vf_iter(v1); vf_it.is_valid(); ++vf_it) {
        v.push_back(*vf_it);
    }

    for (int i = 0; i < v.size();i++) {
        sculptor->getQUM()->delete_face(v[i], false);
    }

    v.clear();

    for (QuasiUniformMesh::VertexFaceIter vf_it = sculptor->getQUM()->vf_iter(v2); vf_it.is_valid(); ++vf_it) {
        v.push_back(*vf_it);
    }

    for (int i = 0; i < v.size();i++) {
        sculptor->getQUM()->delete_face(v[i], false);
    }

    v.clear();

    sculptor->getQUM()->delete_vertex(v1, false);
    sculptor->getQUM()->delete_vertex(v2, false);

    std::reverse(verticesBRing.begin(), verticesBRing.end());

    QuasiUniformMesh::Point a0 = sculptor->getQUM()->point(verticesARing[0]);
    int k = 0, i = 0, j = 0;
    float minDist = FLT_MAX;

    for(int i = 0; i < verticesBRing.size(); ++i)
    {
        float dist = sculptor->calcDist(a0, sculptor->getQUM()->point(verticesBRing[i]));

        if(dist < minDist)
        {
            minDist = dist;
            j = i;
        }
    }

    while(k < valenceA)
    {
        sculptor->getQUM()->add_face(verticesARing[i], verticesBRing[j], verticesBRing[(j+1)%valenceB]);

        sculptor->addToConnectingEdges(sculptor->getQUM()->edge_handle(sculptor->getQUM()->find_halfedge(verticesARing[i], verticesBRing[j])));
        sculptor->addToConnectingEdges(sculptor->getQUM()->edge_handle(sculptor->getQUM()->find_halfedge(verticesBRing[(j+1)%valenceB], verticesARing[i])));

        sculptor->getQUM()->add_face(verticesARing[i], verticesBRing[(j+1)%valenceB], verticesARing[(i+1)%valenceA]);

        sculptor->addToConnectingEdges(sculptor->getQUM()->edge_handle(sculptor->getQUM()->find_halfedge(verticesARing[i], verticesBRing[(j+1)%valenceB])));
        sculptor->addToConnectingEdges(sculptor->getQUM()->edge_handle(sculptor->getQUM()->find_halfedge(verticesBRing[(j+1)%valenceB], verticesARing[(i+1)%valenceA])));

        i = (i+1)%valenceA;
        j = (j+1)%valenceB;

        k++;
    }
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
                break;
            }
        }
        if (hasAnEdgeSame) {
            break;
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
                break;
            }
        }
        if (hasSameVertices) {
            break;
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

