/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H
#include <string>

#include "opengl.h"
#include "light.h"
#include "camera.h"
#include "mesh.h"
#include "renderstate.h"

namespace vortex {
/**
 *  @defgroup SceneGraphGroup Scene Graph.
 *  Here are defined all scene graph related classes
 */

/**
 * Scene graph management.
 * @ingroup SceneGraphGroup
 */
class SceneGraph {

    /* Node definition for graph representation */
public:

    /**
     *  @defgroup SceneGraphNodesGroup Scene Graph nodes.
     *
     *  @ingroup SceneGraphGroup
     */
    /** generic node of the graph
     * @ingroup SceneGraphNodesGroup
     */
    class Node {
    public :
        typedef Node *NodePtr;

        Node(std::string name, Node *parent = NULL) : mName(name), mParent(parent), boxMesh_(NULL), mAcceptVisitors(true) {
        }

        virtual ~Node() {
            delete boxMesh_;
        }

        NodePtr parent() const {
            return mParent;
        }

        void setTransformMatrix(const glm::mat4x4 &transform) {
            mTransform = transform;
        }

        glm::mat4x4 &transformMatrix() {
            return mTransform;
        }

        /** is the node a leaf ? */
        virtual bool isLeaf() {
            return false;
        }

        /** draw the node ;) */
        virtual void draw(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) = 0;

        /** draw the bbox of a node */
        void drawBbox(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);

        /** print Node Info */
        virtual void debug(int indentLevel = 0) = 0;

        const std::string &name() const {
            return mName;
        }

        BBox boundingBox() {
            return mBbox;
        }

        void updateBbox(BBox box) {
            mBbox += box;
        }

        void setAcceptVisitors(bool b){
            mAcceptVisitors=b;
        }

        bool acceptVisitors(){
            return mAcceptVisitors;
        }

    protected :
        glm::mat4x4 mTransform; // default constructor : identiy
        std::string mName;
        Node *mParent;
        BBox mBbox;
        // todo : put this elsewere and keep unique
        Mesh *boxMesh_;

        bool mAcceptVisitors;
    private :
        Node() : mParent(NULL) {}
    };

    /** generic node of the graph
    * an array of sons !
    *  @ingroup SceneGraphNodesGroup
    * */
    class InnerNode : public Node {
    public:
        InnerNode(std::string name, Node *parent = NULL) :
            Node(name, parent),
            mChilds(NULL),
            mNumChilds(0) {}

        virtual ~InnerNode();

        /** Allocate child array */
        void initChilds(unsigned int numChilds);

        int nChilds() const {
            return mNumChilds;
        }

        /** Access to the ith child */
        Node::NodePtr & operator[](unsigned int i) {
            return mChilds[i];
        }

        void draw(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);

        void debug(int indentLevel = 0);


    private:

        Node::NodePtr * mChilds;
        unsigned int mNumChilds;
    };

    /** leaf node of the graph
    * a set of meshes and a material/shader
    *  @ingroup SceneGraphNodesGroup
    **/
    class LeafMeshNode : public Node {
    public:
        LeafMeshNode(std::string name, Node *parent = NULL) : Node(name, parent), mMeshes(NULL), mNumMeshes(0), mRenderStates(NULL) {
        }

        virtual ~LeafMeshNode();

        /** Allocate meshes array */
        void initMeshes(unsigned int numMeshes);

        /** Access to the ith mesh */
        Mesh::MeshPtr & operator[](unsigned int i) {
            return mMeshes[i];
        };

        void draw(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
        void debug(int indentLevel = 0);

        bool isLeaf() {
            return true;
        }
        int nMeshes() const {
            return mNumMeshes;
        }

        /** Access to the ith renderState */
        RenderState::RenderStatePtr &getRenderState(unsigned int i) {
            return mRenderStates[i];
        }
        void setRenderState(unsigned int i, RenderState::RenderStatePtr renderstate) {
            mRenderStates[i] = renderstate;
        }

    private:

        Mesh::MeshPtr *mMeshes;
        unsigned int mNumMeshes;
        RenderState::RenderStatePtr *mRenderStates;
    };




    /**
     *  @defgroup GraphVisitorGroup Scene Graph visitors.
     *  Here are defined all graph visitors
     *  @ingroup SceneGraphGroup
     */

    /**
     *  @defgroup GraphOperatorGroup Scene Graph operators.
     *  Classes of this group are used by visitors to operate on each visited node of a graph
     *  @ingroup SceneGraphGroup
     */

    /**
     * Scenegraph Visitor operation definition.
     * Abstract class to define an operator to apply at each node of a visited graph
     * @ingroup GraphOperatorGroup
     */
    class VisitorOperation {
    public :
        VisitorOperation() {}
        ~VisitorOperation() {}
        /** Implement this if your operator just needs the node to do its job
         */
        virtual void operator()(SceneGraph::Node *theNode) {};
        /** Implement this if your operator needs the node and its matrices to do its job
         */
        virtual void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {};
    };


    /** Scenegraph Visitor operation definition.
     * Abstract base visitor class
     * @ingroup GraphVisitorGroup
     */
    class Visitor {
    public:
        virtual ~Visitor() {};
        void go() {
            if (mVisitedGraph->mRootNode) run(mVisitedGraph->mRootNode);
        }
        void go(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {        
            if (mVisitedGraph->mRootNode)  run(mVisitedGraph->mRootNode, modelViewMatrix, projectionMatrix);
        }
    protected :
        Visitor() : mVisitedGraph(NULL), mAction(mNullAction) {}
	Visitor(SceneGraph *theGraph, VisitorOperation &action) : mVisitedGraph(theGraph), mAction(action) {}
        SceneGraph *mVisitedGraph;
        VisitorOperation &mAction;
        virtual void run(SceneGraph::Node *theNode) = 0;
        virtual void run(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) = 0;

    private :
        class NullAction : public VisitorOperation {
        public:
            NullAction() {};
            void operator()(SceneGraph::Node *) {}
            void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {}
        };
        static NullAction mNullAction;
    };

    /** Pre order visitor.
     * call the visiting operator on a node BEFORE visiting childs
     * @ingroup GraphVisitorGroup
     */
    class PreOrderVisitor : public Visitor {
    public:
        PreOrderVisitor(SceneGraph *theGraph, VisitorOperation &action) : Visitor(theGraph, action) {};
        virtual ~PreOrderVisitor() {};
    protected:
        void run(SceneGraph::Node *theNode);
        void run(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
    };

    /** Post order visitor.
     * call the visiting operator on a node AFTER visiting childs
     * @ingroup GraphVisitorGroup
     */
    class PostOrderVisitor : public Visitor {
    public:
        PostOrderVisitor(SceneGraph *theGraph, VisitorOperation &action) : Visitor(theGraph, action) {};
        virtual ~PostOrderVisitor() {};
    protected:
        void run(SceneGraph::Node *theNode);
        void run(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
    };

public:
    SceneGraph(Node::NodePtr rootNode);
    ~SceneGraph();

    void draw(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {
        if (mRootNode) mRootNode->draw(modelViewMatrix, projectionMatrix);
    }

    BBox getBBox() {
        if (mRootNode)
            return mRootNode->boundingBox();
        else
            return BBox();
    }

    void printDebug();
    std::vector<Light> mLights;

    Node::NodePtr getRootNode() {return mRootNode;}

    std::vector<Camera> mCameras;

protected:
private:
    SceneGraph() : mRootNode(NULL) {}
    Node::NodePtr mRootNode;
    };
}


#endif // SCENEGRAPH_H
