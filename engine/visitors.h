/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef VISITORS_H
#define VISITORS_H
#include <deque>

#include "assetmanager.h"
#include "scenegraph.h"

#include "renderloop.h"

namespace vortex {

/**
 * Print information about a node on std::cerr
 * May be used on pre-order or post-order visitor
 * @ingroup GraphOperatorGroup
 */
class PrintNodeInfo : public SceneGraph::VisitorOperation {
public:
    void operator()(SceneGraph::Node *theNode);
};


/**
 * Build the Bbox of a node.
 * To be used in a postorder visitor.
 * The bbox is built using bbox of sons
 *  @ingroup GraphOperatorGroup
 */
class BBoxBuilder : public SceneGraph::VisitorOperation {
public:
    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
};

/**
 * Render the Bbox of a node.
 * May be used on pre-order or post-order visitor
 *  @ingroup GraphOperatorGroup
 */
class BBoxRenderer : public SceneGraph::VisitorOperation {
    ShaderProgram *shader_;
public:
    BBoxRenderer(ShaderProgram *shader) :
                shader_(shader) {}
    void operator()(SceneGraph::Node *theNode);
};



/**
 * Build the scene default RenderLoop
 * use the default shader : the one store in the renderstate of a node
 * @ingroup GraphOperationGroup
 */
class DefaultLoopBuilder : public SceneGraph::VisitorOperation {
public:
    DefaultLoopBuilder(SceneGraph *sceneGraph, ShaderLoop *loop);
    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
private:
    SceneGraph *mSceneGraph;
    ShaderLoop *mLoop;
};

/**
 * Build the scene default RenderLoop using the particular shader with material properties filtering
 * @ingroup GraphOperationGroup
 */
class ShaderLoopBuilder : public SceneGraph::VisitorOperation {
public:
    ShaderLoopBuilder(AssetManager *resourcesManager, SceneGraph *sceneGraph, ShaderLoop *loop, std::string shaderName);

    void setFilter(MaterialPropertyFilter *filter) {
        mPropertiesFilter = filter;
    }

    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
private:
    AssetManager *mResourcesManager;
    SceneGraph *mSceneGraph;
    ShaderLoop *mLoop;
    std::string mShaderName;
    MaterialPropertyFilter *mPropertiesFilter;
};

/**
 * Build the specialized render loop
 * Assume shader will be set before calling loop. This prevent using automatic shader configuration
 *
 * @ingroup GraphOperationGroup
 */
class TransformLoopBuilder : public SceneGraph::VisitorOperation {
public:
    TransformLoopBuilder(SceneGraph *sceneGraph, TransformLoop *loop, ShaderProgram *shader);
    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
private:
    SceneGraph *mSceneGraph;
    TransformLoop *mLoop;
    ShaderProgram *mShader;
};

/**
 * Build several loops in on graph visiting operation.
 * @todo do not use pointers to LoopBuilder in the mBuilderSet deque but dont know how to make polymorphims works
 * @ingroup GraphOperationGroup
 */
class SetOfLoopBuilder : public SceneGraph::VisitorOperation {
public:
    SetOfLoopBuilder();
    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
    void addLoopBuilder(SceneGraph::VisitorOperation *builder);
private:
    std::deque<SceneGraph::VisitorOperation *> mBuilderSet;
};

/**
 *
 */
class LightTransformVisitor : public SceneGraph::VisitorOperation {
public:
    LightTransformVisitor(SceneGraph *sceneGraph) : mSceneGraph(sceneGraph){}
    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
private:
    SceneGraph *mSceneGraph;
};



/**
  * Shader compilation visitor with configuration management
  *
  */
class ShaderBuilder : public vortex::SceneGraph::VisitorOperation {
public:
    ShaderBuilder(vortex::AssetManager *resourcesManager, std::string shaderName, bool setDefault=true);

    void setFilter(vortex::MaterialPropertyFilter *filter) {
        mPropertiesFilter = filter;
    }

    void operator()(vortex::SceneGraph::Node *theNode);

private :
    vortex::AssetManager *mResourcesManager;
    std::string mShaderName;
    bool mSetAsDefault;
    vortex::MaterialPropertyFilter *mPropertiesFilter;

};


/**
 * Get informations about the graph
 * @ingroup GraphOperationGroup
 */
class GraphStatistic : public SceneGraph::VisitorOperation {
public:
    GraphStatistic();
    void operator()(SceneGraph::Node *theNode);
    friend std::ostream & operator << (std::ostream &out, const GraphStatistic &box);
private:
    int leafCount;
    int innerCount;
    int meshCount;
    int faceCount;
    int verticesCount;
};

/**
 * Set a material for one mesh (by name)
 * @ingroup GraphOperationGroup
 */
class MaterialSetter : public SceneGraph::VisitorOperation {
public:
    MaterialSetter(Material *material, std::string name);
    void operator()(SceneGraph::Node *theNode);
private :
    Material *mMaterial;
    std::string mName;
};


/**
 * Update the mesh position (animation)
 * @author Alexandre Bonhomme
 * @date 02/05/2012
 */
class MeshUpdater : public SceneGraph::VisitorOperation {
public:
    MeshUpdater(double time)
    {
        mTime = time;
    }

    void operator()(vortex::SceneGraph::Node *theNode);
private:
    double mTime;
};

} // namespace vortex

#endif
