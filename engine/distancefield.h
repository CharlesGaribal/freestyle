/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef DISTANCEFIELD_H
#define DISTANCEFIELD_H

#include "scenegraph.h"
#include "mesh.h"

namespace vortex {

/*  Pseudo distance field permettant de retrouver le voxel le plus proche d'un point
    et ensuite, le triangle le plus proche puis le point sur un maillage le plus proche */
class DistanceField
{
    // Boite englobante du distance field
    BBox mDistanceFieldBox;

    /* Les sommets du distance field : les pointeurs vers les sommets des maillages ajoutés */
    typedef Mesh::VertexData DFVertex; // TODO : transformer ça en pointeur pour gagner de la place et du temps si animations
    std::vector<DFVertex> mVertices;

    /* Les triangles du distance field : un tableau d'indice vers les sommets */
    std::vector<int> mIndices;

    /* TODO : ralation entre triangles et matériaux :
     *  Un matériau correspond à un intervalle d'indices de triangles.
     *  Faire une map <Intervalle, materiau> permettant de retrouver rapidement le matériau d'un triangle
     */

     /*
      *  Un voxel du distance field
      */
    class DFVoxel {
    public :
        // Est-ce vraiment utile
        int nbTriangles;
        std::vector<int> *triangles;

        // Information about nearest point on a surface
        float dist;
        glm::vec3 offset;
        int nearestTriangle;
        float barycentricCoords[3];

        // reference point in the voxel -- may be removed later
        glm::vec3 point;

        DFVoxel(): nbTriangles(0), triangles(NULL), dist(HUGE), offset(0.f), nearestTriangle(-1), point(0.f) {}
        DFVoxel(const glm::vec3 &p): nbTriangles(0), triangles(NULL), dist(HUGE), offset(0.f), nearestTriangle(-1), point(p) {}

        // un triangle est défini par l'indice dans le tableau mTriangles de son premier sommet
        // les autres paramètres servent à calculer les propriétés du voxel
        void addTriangle(int t, Mesh::VertexData *triangleVertices[3], const glm::vec4 &trianglePlane, const glm::vec3 triangleEdges[2]);
    };
    typedef DFVoxel *ptrVoxel;

    /* Les voxels */
    ptrVoxel *mVoxelGrid;
    glm::ivec3 mGridSize;
    float mGridStep;

    bool inVoxel(int i, int j, int k, const glm::vec4 &triangle);

    // far drawing (debug)
    // OpenGL stuffs
    GLuint mVertexArrayObject;
    enum {VBO_VERTICES, VBO_INDICES};
    GLuint mVertexBufferObjects[2];

    int mNumDrawVertices;
    std::vector<Mesh::VertexData> mDrawVertices;
    int mNumDrawIndices;
    std::vector<int> mDrawIndices;
    void init();

public:

    DistanceField();

    /* precision : un DF est constitué de voxels cubique.
        On donne la taille d'un voxel et le nombre de subdivision sur chaque axe
        sera calculé à partir de la boite englobante : nbsubdiv_axe = (int)(precision/lg_axe) + 1
    */
    void build(float precision);

    /* Add the mesh to the distance field structure */
    void addMesh(Mesh *theMesh, Material *theMaterial, const glm::mat4 &matrix);

    Mesh::VertexData findNearest(const glm::vec3 &which);


    void draw();
};


/* Visitor operation for adding an object to a distance field */
class DistanceFieldAdder : public SceneGraph::VisitorOperation {
public:
    DistanceFieldAdder(DistanceField *distanceField);
    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
private :
    DistanceFieldAdder(){}
    DistanceField  *mDistanceField;
};


}
#endif // DISTANCEFIELD_H
