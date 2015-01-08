/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include <iostream>
#include <sstream>

#include "opengl.h"

#include "animatedmesh.h"
#include <assimp/anim.h>
#include "assetmanager.h"

namespace vortex {
/**
  @todo make all glm function call to take radians
  */
#define DEG_TO_RAD(x) ((x)*M_PI/180.0)


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

AnimatedMesh::AnimatedMesh(std::string name) :
    Mesh(name),
    mNumBones(0),
    mBones(NULL),
    mResources(NULL)
{
}

AnimatedMesh::AnimatedMesh(std::string name, VertexData *vertices, int numVertices, int *indices, int numIndices, BoneData *bones, int numBones, AssetManager *resources) :
    Mesh(name, vertices, numVertices, indices, numIndices),
    mNumBones(numBones),
    mBones(NULL),
    mResources(resources) {
    mBones = new BoneData[mNumBones];
    assert(mBones);

    for (unsigned int i = 0; i < mNumBones; ++i){
        mBones[i] = bones[i];

        mBones[i].mWeights = new WeightData[mBones[i].mNumWeights];
        assert(mBones[i].mWeights);
        memcpy(mBones[i].mWeights, bones[i].mWeights, mBones[i].mNumWeights * sizeof(WeightData));
    }
}

AnimatedMesh::~AnimatedMesh() {
    for (unsigned int i = 0; i < mNumBones; ++i) {
        delete[] mBones[i].mWeights;
    }
    delete[] mBones;
}

/**
 *
 * @author Alexandre Bonhomme
 * @date Last edit : 09/05/12
 */
void AnimatedMesh::update() {

    BBox updatedBbox;
    //TODO
    // work with only one animation
    SkeletonGraph *skeleton = mResources->getCurrentAnimation()->getSkeleton();

    // new vertices
    VertexData *animVertex = new VertexData[mNumVertices];
    memset(animVertex, 0, mNumVertices * sizeof(VertexData));
    // Parcours des bones
    for (unsigned int i = 0; i < mNumBones; ++i) {

        // get the transform matrix
        SkeletonGraph::Node* boneNode = skeleton->getBone(mBones[i].mName);
        assert(boneNode); // TODO check
        if (boneNode){
            glm::mat4x4 transformMatrix = boneNode->globalTransform();
            glm::mat4x4 offsetMatrix = boneNode->offsetTransform();

            // Calcul vect :  Vect * M_transf
            // Calcul normal : Normal * inv( transp( M_transf )
            // Calcul tangente : Tang * inv( transp( M_transf )
            for (unsigned int w = 0; w < mBones[i].mNumWeights; ++w) {
                unsigned int vertexId = mBones[i].mWeights[w].mVertexId;
                float weight = mBones[i].mWeights[w].mWeight;

                // Vertex
                glm::vec4 vertex(mVertices[vertexId].mVertex.x,
                                 mVertices[vertexId].mVertex.y,
                                 mVertices[vertexId].mVertex.z,
                                 1.0
                );
                vertex = transformMatrix * offsetMatrix * vertex;
                updatedBbox += glm::vec3(vertex.x, vertex.y, vertex.z);
                animVertex[vertexId].mVertex += glm::vec3(vertex) * weight;

                // Normal
                glm::vec4 normal(mVertices[vertexId].mNormal.x,
                                 mVertices[vertexId].mNormal.y,
                                 mVertices[vertexId].mNormal.z,
                                 0.0
                );
                glm::mat4x4 temp = glm::inverse(glm::transpose(transformMatrix)); // opti

                normal = temp * offsetMatrix * normal;

                animVertex[vertexId].mNormal += glm::vec3(normal) * weight;

                // Tangent
                glm::vec4 tangent(mVertices[vertexId].mTangent.x,
                                  mVertices[vertexId].mTangent.y,
                                  mVertices[vertexId].mTangent.z,
                                  0.0
                );
                tangent = temp * offsetMatrix * tangent;

                animVertex[vertexId].mTangent += glm::vec3(tangent) * weight;

                // Texture coord
                animVertex[vertexId].mTexCoord = mVertices[vertexId].mTexCoord;

            }
        }
    }

    // bind vertexdata
    glAssert( glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjects[VBO_VERTICES]) );
    // No need to allocate buffer, just replace its content
    glAssert( glBufferSubData(GL_ARRAY_BUFFER, 0, mNumVertices * sizeof(VertexData), animVertex) );

    delete [] animVertex;
    mBbox = updatedBbox;
}


}// vortex
