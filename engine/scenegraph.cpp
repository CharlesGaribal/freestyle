/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include <iostream>
#include <stack>

#include "scenegraph.h"


using namespace std;
namespace vortex {
SceneGraph::SceneGraph(Node::NodePtr rootNode)
{
    mRootNode = rootNode;
}
SceneGraph::~SceneGraph() {}

void SceneGraph::printDebug()
{
    //
    mRootNode->debug();

    for(vector<Light>::iterator itr = mLights.begin(), stop = mLights.end(); itr!=stop; ++ itr)
        itr->printDebug();
}

void SceneGraph::Node::drawBbox(const glm::mat4x4& modelViewMatrix, const glm::mat4x4& projectionMatrix)
{
    if (!boxMesh_) {
    // Creation d'un maillage correspondant à la bbox
    BBoxLineMeshBuilder boxbuilder;
    boxMesh_ = boxbuilder.build(mName, mBbox);

    // dessin du maillage
    boxMesh_->init();
    }
    boxMesh_->drawLines();

}

/* InnerNode implementation */
SceneGraph::InnerNode::~InnerNode()
{
    for (unsigned int i = 0; i < mNumChilds; ++i)
        delete mChilds[i];
    delete[] mChilds;
}

/** Allocate child array */
void SceneGraph::InnerNode::initChilds(unsigned int numChilds)
{
    mNumChilds = numChilds;
    mChilds = new SceneGraph::Node::NodePtr[numChilds];
}

void SceneGraph::InnerNode::draw(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
    for (unsigned int i = 0; i < mNumChilds; ++i)
        mChilds[i]->draw(mTransform * modelViewMatrix, projectionMatrix);
}

void SceneGraph::InnerNode::debug(int indentLevel)
{
    for (int i = 0; i < indentLevel; ++i)
        std::cerr << "--";
    std::cerr << mName << " : Inner Node with " << mNumChilds << " childs :" << std::endl;

    for (unsigned int i = 0; i < mNumChilds; ++i)
        mChilds[i]->debug(indentLevel + 1);
}

/* LeafNode implementation */

SceneGraph::LeafMeshNode::~LeafMeshNode()
{
    for (unsigned int i = 0; i < mNumMeshes; ++i) {
        delete mMeshes[i];
        delete mRenderStates[i];
    }
    delete[] mMeshes;
    delete[] mRenderStates;
}

/** Allocate meshes array */
void SceneGraph::LeafMeshNode::initMeshes(unsigned int numMeshes)
{
    mNumMeshes = numMeshes;
    mMeshes = new Mesh::MeshPtr[mNumMeshes];
    mRenderStates = new RenderState::RenderStatePtr [mNumMeshes];
    for (unsigned int i = 0; i < numMeshes; i++) {
        mRenderStates[i] = new RenderState;
        mMeshes[i] = NULL;
    }
}


void SceneGraph::LeafMeshNode::draw(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
    glm::mat4x4 localModelView = transformMatrix() * modelViewMatrix;
    glm::mat4x4 localProjection = projectionMatrix;
    glm::mat4x4 normalMatrix = glm::transpose(glm::inverse(localModelView));
    glm::mat4x4 MVP = localProjection * modelViewMatrix;
    for (unsigned int i = 0; i < mNumMeshes; ++i) {
        mRenderStates[i]->bind(localModelView, localProjection, MVP, normalMatrix);
        mMeshes[i]->draw();
        mRenderStates[i]->unbind();
    }
}

void SceneGraph::LeafMeshNode::debug(int indentLevel)
{
    for (int i = 0; i < indentLevel; ++i)
        std::cerr << "--";
    std::cerr << mName << " : Leaf Node with " << mNumMeshes << " meshes :" << std::endl;
    for (int i = 0; i < indentLevel; ++i)
        std::cerr << "--";

    std::cerr << mName << " materials  " << std::endl;
    for (unsigned int i = 0; i < mNumMeshes; ++i) {
        mRenderStates[i]->getMaterial()->debug();
    }
}

/* Visitors implementation */
SceneGraph::Visitor::NullAction SceneGraph::Visitor::mNullAction;

void SceneGraph::PreOrderVisitor::run(SceneGraph::Node *theNode)
{
    if (theNode->acceptVisitors()){
        mAction(theNode);
        if (!theNode->isLeaf()) {
            SceneGraph::InnerNode * innerNode = static_cast<SceneGraph::InnerNode *>(theNode);
            for (int i = 0; i < innerNode->nChilds(); ++i) {
                run((*innerNode)[i]);
            }
        }
    }
}

void SceneGraph::PostOrderVisitor::run(SceneGraph::Node *theNode)
{
    if (theNode->acceptVisitors()){
        if (!theNode->isLeaf()) {
            SceneGraph::InnerNode * innerNode = static_cast<SceneGraph::InnerNode *>(theNode);
            for (int i = 0; i < innerNode->nChilds(); ++i) {
                run((*innerNode)[i]);
            }
        }
        mAction(theNode);
    }
}

void SceneGraph::PostOrderVisitor::run(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
    if (theNode->acceptVisitors()){
        if (!theNode->isLeaf()) {
            SceneGraph::InnerNode * innerNode = static_cast<SceneGraph::InnerNode *>(theNode);
            for (int i = 0; i < innerNode->nChilds(); ++i) {
                run((*innerNode)[i],  modelViewMatrix * theNode->transformMatrix(), projectionMatrix);
            }
        }
        mAction(theNode, modelViewMatrix * theNode->transformMatrix() , projectionMatrix);
    }
}


void SceneGraph::PreOrderVisitor::run(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
    if (theNode->acceptVisitors()){
        mAction(theNode, modelViewMatrix * theNode->transformMatrix(), projectionMatrix);
        if (!theNode->isLeaf()) {
            SceneGraph::InnerNode * innerNode = static_cast<SceneGraph::InnerNode *>(theNode);
            for (int i = 0; i < innerNode->nChilds(); ++i) {
                run((*innerNode)[i], modelViewMatrix * theNode->transformMatrix(), projectionMatrix);
            }
        }
    }
}
}//namespace vortex
