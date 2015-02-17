#include "topologicalhandler.h"
#include "sculptor.h"

//Sommet courant du second 1-ring
struct VertexSecondRing
{

    //Distance du sommet par rapport au somment courant du premier 1-ring
    float dist;
    QuasiUniformMesh::VertexHandle v;
    //Nombre de fois où le sommet est lié à un sommet du premier 1-ring
    int nbUsing = 0;
};

TopologicalHandler::TopologicalHandler(Sculptor *sculptor) :
    sculptor(sculptor)
{}

void TopologicalHandler::handleJoinVertex(OpenMesh::VertexHandle &v1, OpenMesh::VertexHandle &v2)
{
    QuasiUniformMesh::Point v1Courant;
    QuasiUniformMesh::Point v2Courant;
    int valence = 0;
    float plusProche = 999999999999;
    float secondPlusProche = 99999999999;
    int idCpt = 0;

    //parcour des vertex du premier 1-ring
    for (QuasiUniformMesh::VertexVertexIter vv_it = sculptor->getQUM()->vv_iter(v1); vv_it.is_valid(); ++vv_it)
    {
        v1Courant = sculptor->getQUM()->point(*vv_it);
        float dist;
        //Calcul de la valence du second 1-ring
        for (QuasiUniformMesh::VertexVertexIter vv_it2 = sculptor->getQUM()->vv_iter(v2); vv_it2.is_valid(); ++vv_it2)
        {
            ++valence;
        }
        //Tableau de VertexSecondRing pour stocker les distances des sommets du second 1-ring avec le sommet courant vv_it
        VertexSecondRing *vsr = new VertexSecondRing[valence];

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
        face_vhandles.push_back(vhandle[2]);
        vsr[idSecondPlusProche].nbUsing++;
        face_vhandles.push_back(vhandle[1]);
        vsr[idPlusProche].nbUsing++;
        face_vhandles.push_back(vhandle[0]);
        sculptor->getQUM()->add_face(face_vhandles);

        plusProche = 999999999999;
        secondPlusProche = 99999999999;
        idCpt = 0;
        valence = 0;
    }
}
