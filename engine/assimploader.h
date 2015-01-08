/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef ASSIMPLOADER_H
#define ASSIMPLOADER_H

#include <iostream>
#include <cstdio>
#include <sstream>

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"          // Output data structure
#include "assimp/postprocess.h"     // Post processing flags


#include "assetmanager.h"
#include "scenegraph.h"
#include "loader.h"

#include "visitors.h"

namespace vortex {

/**
 * Abstract class Loader implementation for ASSIMP library loading
 *
 */
class AssimpLoader : public Loader {

public :
    AssimpLoader() {}
    ~AssimpLoader() {}

    bool loadScene(const std::string &filename, AssetManager &am, SceneGraph **sg);

    const char *getLastErrorString() const;

    int lastError() const;

private :

    std::string mLastErrorString;
    int mLastErrorNum;

    AssetManager *mResources;

    SceneGraph **mSceneGraph;

    /**
     * Transform ASSIMP's leaf nodes in SceneGraph::LeafNodes
     *
     * @param currentInputNode Pointer to the ASSIMP's leaf node to transform
     * @param currentOutputNode Pointer to the SceneGraph::LeafNode to be filled by "currentInnerNode" transformed data
     * @param inputScene Pointer to the ASSIMP's scene graph to help transform children nodes
     */
    void fillLeafNode(aiNode *currentInputNode, SceneGraph::LeafMeshNode * currentOutputNode, const aiScene *inputScene);

    /**
     * Transform ASSIMP's inner nodes in SceneGraph::InnerNodes
     *
     * @param currentInputNode Pointer to the ASSIMP's inner node to transform
     * @param currentOutputNode Pointer to the SceneGraph::InnerNode to be filled by "currentInnerNode" transformed data
     * @param inputScene Pointer to the ASSIMP's scene graph to help transform children nodes
     */
    void fillInnerNode(aiNode *currentInputNode, SceneGraph::InnerNode * currentOutputNode, const aiScene *inputScene);

    /**
     * Transform ASSIMP's scene graph in SceneGraph
     *
     * @param name SceneName
     * @param scene Pointer to the ASSIMP's scene graph to be transformed
     */
    void buildInternalRepresentation(const std::string &name, const aiScene* scene);

     /**
     * Create a skeleton (a tree of bone) from the scene graph
     * @param sceneNode The root of the skeleton in the scene graph
     * @param parent Initialy NULL (for the root node)
     */
    SkeletonGraph::Node* createSkeletonFromScene(std::map<std::string, bool> *bonesMap, SceneGraph::Node* sceneNode, SkeletonGraph::Node* parent);

    /**
     * Find the skeleton root in the scene graph
    //TODO
     */
    void findSkeletonRoot(const std::string &name, Animation *anim, SceneGraph::Node* node, SceneGraph::Node::NodePtr *root);


    SkeletonGraph* buildInternalSkeleton(Animation *anim);

    void selectUsefullNode(std::map<std::string, bool> *bonesMap, SceneGraph::Node *node);

};

}

#endif
