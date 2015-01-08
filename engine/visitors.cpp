/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "visitors.h"

namespace vortex {


void PrintNodeInfo::operator()(SceneGraph::Node* theNode)
{
    std::cerr << theNode->name() << " " << theNode->boundingBox() << std::endl;
}

/*
 * -------------------------------------------------------------------------------
 */
void BBoxBuilder::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {
    if (theNode->isLeaf()) {
       SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
       for (int i = 0; i < leafNode->nMeshes(); ++i){
           if ((*leafNode)[i]){
                BBox theBox = (*leafNode)[i]->boundingBox();
                if (! theBox.isEmpty()){
                    BBox theTransformedBox = theBox.getTransformedBBox(modelViewMatrix);
                    leafNode->updateBbox(theTransformedBox);
                 }
            }
       }
//       BBox theBox = theNode->boundingBox();
//       std::cerr << "LeafNode " << theNode->name() << " : " << theBox << std::endl;
   } else {
       SceneGraph::InnerNode *innerNode = static_cast<SceneGraph::InnerNode *>(theNode);
       for (int i = 0; i < innerNode->nChilds(); ++i){
           BBox theBox = (*innerNode)[i]->boundingBox();
           if (! theBox.isEmpty()){
                BBox theTransformedBox = theBox; // transformation already integrated in childrens bbox
                innerNode->updateBbox(theTransformedBox);
           }
       }
//       BBox theBox = theNode->boundingBox();
//       std::cerr << "InnerNode " << theNode->name() << " : " << theBox << std::endl;
   }
}

void BBoxRenderer::operator()(SceneGraph::Node *theNode) {
    if (theNode->isLeaf()) {
        shader_->setUniform("featureColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        theNode->drawBbox(glm::mat4(1.f), glm::mat4(1.f));
    } else {
        shader_->setUniform("featureColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        theNode->drawBbox(glm::mat4(1.f), glm::mat4(1.f));
    }
}


/*
 * -------------------------------------------------------------------------------
 */
DefaultLoopBuilder::DefaultLoopBuilder(SceneGraph *sceneGraph, ShaderLoop *loop) : mSceneGraph(sceneGraph), mLoop(loop)
{
}

void DefaultLoopBuilder::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
//    std::cerr << " -- DefaultLoopBuilder::operator()" << std::endl;
    if (theNode->isLeaf()) {
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
        RenderState *state = NULL;
        ShaderProgram *prog = NULL;
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]) {
            state = leafNode->getRenderState(i);
            prog = state->getShaderProgram();
            //@TODO check not needed data duplication, allocation ... !!! *prog is copied instead of linked ...
            (*mLoop)[ *prog ][ TransformState(modelViewMatrix, projectionMatrix, prog)]
            [ MaterialState(state->getMaterial(), prog)].push_back((*leafNode)[i]);
}
        }

    }
}
/*
 * -------------------------------------------------------------------------------
 */
ShaderLoopBuilder::ShaderLoopBuilder(AssetManager *resourcesManager, SceneGraph *sceneGraph, ShaderLoop *loop, std::string shaderName)
    : mResourcesManager(resourcesManager), mSceneGraph(sceneGraph), mLoop(loop), mPropertiesFilter(NULL)
{
    mShaderName = mResourcesManager->getShaderBasePath() +  shaderName;
}

void ShaderLoopBuilder::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
//    std::cerr << " -- ShaderLoopBuilder::operator()" << std::endl;
    if (theNode->isLeaf()) {
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
        RenderState *state = NULL;
        ShaderProgram *prog = NULL;
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]){
                state = leafNode->getRenderState(i);

                // build a shaderConfiguration object from material
                ShaderConfiguration nodeShaderConfiguration(ShaderConfiguration::DEFAULT, mShaderName);
                Material * nodeMaterial = state->getMaterial();
                for (int j = 0; j < nodeMaterial->numTexture(); ++j) {
                    if ( !mPropertiesFilter || mPropertiesFilter->match(nodeMaterial->getTextureSemantic(j)) )
                        nodeShaderConfiguration.addProperty(nodeMaterial->getTextureSemanticString(j));
                }

                // get or generate the shader associated with this configuration
                prog = mResourcesManager->getShaderProgram(nodeShaderConfiguration);
                prog->setConfiguration(nodeShaderConfiguration); // TODO Useless ???
                (*mLoop)[ *prog ][ TransformState(modelViewMatrix, projectionMatrix, prog)]
                [ MaterialState(nodeMaterial, prog)].push_back((*leafNode)[i]);
            }
        }
    }
}

/*
 * -------------------------------------------------------------------------------
 */

TransformLoopBuilder::TransformLoopBuilder(SceneGraph *sceneGraph, TransformLoop *loop, ShaderProgram *shader) : mSceneGraph(sceneGraph), mLoop(loop),
    mShader(shader)
{
}

void TransformLoopBuilder::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
    if (theNode->isLeaf()) {
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
        RenderState *state = NULL;
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
if ((*leafNode)[i]) {
            state = leafNode->getRenderState(i);
            (*mLoop)[ TransformState(modelViewMatrix, projectionMatrix, mShader)]
            [ MaterialState(state->getMaterial(), mShader)].push_back((*leafNode)[i]);
}
        }

    }
}

/*
 * -------------------------------------------------------------------------------
 */

SetOfLoopBuilder::SetOfLoopBuilder(): SceneGraph::VisitorOperation() {
}

void SetOfLoopBuilder::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix){
//    std::cerr << "SetOfLoopBuilder::operator() -- (" << mBuilderSet.size() << ")" <<std::endl;
    for (std::deque<SceneGraph::VisitorOperation *>::iterator it=mBuilderSet.begin(); it!= mBuilderSet.end(); ++it)
        (*(*it))(theNode, modelViewMatrix, projectionMatrix);
}

void SetOfLoopBuilder::addLoopBuilder(SceneGraph::VisitorOperation *builder){
    mBuilderSet.push_back(builder);
}

/*
 * -------------------------------------------------------------------------------
 */

void LightTransformVisitor::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix)
{
    if (theNode->isLeaf()) {
        if(mSceneGraph->mLights.size()>0) {
            for(unsigned int i=0;i<mSceneGraph->mLights.size(); ++i){
                if(0 == mSceneGraph->mLights[i].mName.compare(theNode->name())){
                    mSceneGraph->mLights[i].mTransform = modelViewMatrix;
                }
            }

        }
    }
}



/*
 * -------------------------------------------------------------------------------
 */
// ShaderCreatorVisitor

ShaderBuilder::ShaderBuilder(vortex::AssetManager *resourcesManager, std::string shaderName, bool setDefault) :
    mResourcesManager(resourcesManager),
    mShaderName(shaderName),
    mSetAsDefault(setDefault), mPropertiesFilter(NULL){
    mShaderName = mResourcesManager->getShaderBasePath() + shaderName;
    //std::cerr << "Shader builder visitor : " << mShaderName << std::endl;
}

void ShaderBuilder::operator()(SceneGraph::Node *theNode)
{
    if (theNode->isLeaf()) {
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]) {
                RenderState::RenderStatePtr stateObject = leafNode->getRenderState(i);
                stateObject->setAssetManager(mResourcesManager);

            // build a shaderConfiguration object from material
            ///@todo make ShaderConfiguration::DEFAULT configurable
    // TODO allow tesss and geom shader
                ShaderConfiguration nodeShaderConfiguration(ShaderConfiguration::DEFAULT, mShaderName);
                Material * nodeMaterial = stateObject->getMaterial();
                for (int j = 0; j < nodeMaterial->numTexture(); ++j) {
                    if ( mPropertiesFilter ) {
                        if (mPropertiesFilter->match(nodeMaterial->getTextureSemantic(j)) )
                            nodeShaderConfiguration.addProperty(nodeMaterial->getTextureSemanticString(j));
                    } else {
                        nodeShaderConfiguration.addProperty(nodeMaterial->getTextureSemanticString(j));
                    }
                }
                // get or generate the shader associated with this configuration
                ShaderProgram *theNodeProgram = mResourcesManager->getShaderProgram(nodeShaderConfiguration);
                if (mSetAsDefault){
                    // associate the shader with the state object
                    stateObject->setShaderProgram(theNodeProgram);
                }
            }
        }
    }
}

/*
 * -------------------------------------------------------------------------------
 */
GraphStatistic::GraphStatistic() : leafCount(0), innerCount(0), meshCount(0), faceCount(0), verticesCount(0){

}

void GraphStatistic::operator()(SceneGraph::Node *theNode){
    if (theNode->isLeaf()) {
        leafCount+=1;
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
        meshCount += leafNode->nMeshes();
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]) {
                Mesh::MeshPtr theMesh= (*leafNode)[i];
                faceCount += theMesh->numIndices()/3;
                verticesCount += theMesh->numVertices();
            }
        }
    } else {
       innerCount+=1;
    }
}

std::ostream & operator << (std::ostream &out, const GraphStatistic &stat){
    out << "Inner nodes : " << stat.innerCount << " / leaf nodes : " << stat.leafCount;
    out << " --- Meshes : " << stat.meshCount << " - Faces : " << stat.faceCount << " - Vertex : " << stat.verticesCount;
    return out;
}


// MeshUpdater

/**
 * Update if the mesh is animated
 * @author Alexandre Bonhomme
 * @date 02/05/2012
 */
void MeshUpdater::operator()(SceneGraph::Node *theNode) {

    if (theNode->isLeaf()) {

        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);

        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if (mTime >=0){
                // update the mesh
                (*leafNode)[i]->update();
            } else {
               (*leafNode)[i]->reset();
            }
        }
    }
}

/*
 * Material Setter
 * -------------------------------------------------------------------------------
 */

MaterialSetter::MaterialSetter(Material *material, std::string name) : mMaterial(material), mName(name) {
}

void MaterialSetter::operator ()(SceneGraph::Node *theNode) {
    if (theNode->isLeaf()) {
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);

        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]->name() == mName)
                leafNode->getRenderState(i)->setMaterial(mMaterial);
        }
    }
}

} // namespace vortex
