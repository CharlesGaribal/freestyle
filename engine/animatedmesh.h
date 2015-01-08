/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include "scenegraph.h"
#include "mesh.h"
#include "skeleton.h"

/**
 * @author Alexandre Bonhomme
 * @date 25/04/12
 */
namespace vortex {

/**
 * Animated mesh representation class.
 */

class AnimatedMesh : public Mesh {
public:
    /**
     * Weight structure representation
     */
    struct WeightData {
        // Id of the vertex
        unsigned int mVertexId;

        // Normalize weight
        float mWeight;
    };

    /**
     * Bone structure representation
     */
    struct BoneData {
        // Bone name
        std::string mName;

        // @TODO
        // "Matrix that transforms from mesh space to bone space in bind pose." Assimp doc
        glm::mat4x4 mOffsetMatrix;

        // Number of weights
        unsigned int mNumWeights;

        // Array of associate weigts
        WeightData *mWeights;
    };

    /**
     * AnimatedMesh pointer definition.
     */
    typedef AnimatedMesh * AnimatedMeshPtr;

    /**
     * Constructor : initialize Mesh attributes.
     *
     * @param name The name of the Mesh Object.
     */
    AnimatedMesh(std::string name);

    /**
     * Constructor
     *
     * @param name The name of the Mesh Object.
     * @param vertices Pointer to the vertices structures defining the mesh points.
     * @param numVertices Number of vertices structures in "vertices".
     * @param indices Pointer to the vertices structures indices that define the triangular faces of the mesh.
     * @param numIndices Number of indices in "indices".
     * @param bones Pointer to the bones structure.
     * @param nulBones Number of bones in "bones".
     */
    AnimatedMesh(std::string name, VertexData *vertices, int numVertices, int *indices, int numIndices, BoneData *bones, int numBones, AssetManager *resources);
    ~AnimatedMesh();

    /**
     * Update the mesh position
     *
     * @param t Current time
     */
    void update();

    /*********** Bones ***********/
    /**
     * @return The numbers of bones
     */
    unsigned int nBones() const {
        return mNumBones;
    }

    /**
     * Return the bone as corresponding to index
     * @param index Index of bone in array
     * @return The bone selected
     */
    BoneData *getBone(unsigned int index) const {
        return &( mBones[index] );
    }

private:

    unsigned int mNumBones;
    BoneData *mBones;   // Bones data struct array

    AssetManager *mResources;

};

} //vortex
#endif // ANIMATEDMESH_H
