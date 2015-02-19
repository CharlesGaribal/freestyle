/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "picker.h"
#include "fbo.h"

namespace vortex {

static const char *selectionvertexshader = "#version 410\n\
        uniform mat4 MVP;\n\
        in vec3 inPosition;\n\
        in vec3 inNormal;\n\
        in vec3 inTangent;\n\
        in vec4 inTexCoord;\n\
        out vec4 varTexCoord;\n\
        out float vertexID;\n\
        void main(void)\n\
        {\n\
          vertexID = gl_VertexID;\n\
          gl_Position = MVP*vec4(inPosition.xyz, 1.0);\n\
          varTexCoord = inTexCoord;\n\
        }\n";

static const char *selectionfragmentshader = "#version 410\n\
        layout(location = 0) out ivec4 outIds;\n\
        layout(location = 1) out vec4 outdebug;\n\
        in vec4 varTexCoord;\n\
        uniform int materialId;\n\
        uniform int meshId;\n\
        in float vertexID;\n\
        void main(void)\n\
        {\n\
            outIds = ivec4(materialId, meshId, gl_PrimitiveID, 1);\n\
            outdebug = vec4(materialId/100., meshId/100., gl_PrimitiveID/1000., 1);\n\
        }\n";


Picker::Picker(AssetManager *assetmanager, SceneGraph *scenegraph, int w, int h) : assetmanager_(assetmanager), scenegraph_(scenegraph), width_(0), height_(0) {

    std::cerr << "Construction du picker (" << w << "x" << h << ")" << std::endl;
    selectionFBO_ = new FBO(FBO::Components(FBO::DEPTH | FBO::COLOR), w, h);
    selectionFBO_->bind();
    selectionFBO_->attachRBO(GL_DEPTH_ATTACHMENT);
    selectionFBO_->unbind();

    glCheckError();

    selectionTexture_ =  new Texture("selectbuffer", GL_TEXTURE_2D);
    debugTexture_ =  new Texture("debugbuffer", GL_TEXTURE_2D);

    glCheckError();

    ShaderObject *vertexShader = new ShaderObject;
    vertexShader->create(GL_VERTEX_SHADER);
    vertexShader->source(&selectionvertexshader,1);
    vertexShader->compile();
    vertexShader->check();
    ShaderObject *fragmentShader = new ShaderObject;
    fragmentShader->create(GL_FRAGMENT_SHADER);
    fragmentShader->source(&selectionfragmentshader,1);
    fragmentShader->compile();
    fragmentShader->check();
    shader_ = new ShaderProgram();
    shader_->create();
    shader_->add(vertexShader);
    shader_->add(fragmentShader);
    shader_->link();
    shader_->check();

    delete vertexShader;
    delete fragmentShader;

    glCheckError();

    selectionloop_ = new SelectionLoop();

    SelectionLoopBuilder loopBuilder(assetmanager_, scenegraph_, selectionloop_, shader_);
    SceneGraph::PostOrderVisitor loopBuilderVisitor(scenegraph_, loopBuilder);
    loopBuilderVisitor.go(glm::mat4(1.0), glm::mat4(1.0));

    glCheckError();

    setPickingViewport(w, h);
}

Picker::~Picker() {
    delete selectionFBO_;
    delete selectionTexture_;
    delete debugTexture_;
    delete shader_;
    delete selectionloop_;
}


void Picker::updateSelectionLoop() {
    selectionloop_->clear();
    SelectionLoopBuilder loopBuilder(assetmanager_, scenegraph_, selectionloop_, shader_);
    SceneGraph::PostOrderVisitor loopBuilderVisitor(scenegraph_, loopBuilder);
    loopBuilderVisitor.go(glm::mat4(1.0), glm::mat4(1.0));
}

void Picker::setPickingViewport(int w, int h) {

    // fbo and texture : note that texture must be allocated each time the size of the window change
    if ( (w != width_) || (h != height_)) {
        width_=w;
        height_=h;
        if (selectionTexture_->getId() != 0)
            selectionTexture_->deleteGL();
        selectionTexture_->initGL(GL_RGBA32I, width_, height_, GL_RGBA_INTEGER, GL_INT, NULL);
        selectionTexture_->setFilter(GL_LINEAR, GL_LINEAR);
        selectionTexture_->setClamp(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        if (debugTexture_->getId() != 0)
            debugTexture_->deleteGL();
        debugTexture_->initGL(GL_RGBA, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        debugTexture_->setFilter(GL_LINEAR, GL_LINEAR);
        debugTexture_->setClamp(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        selectionFBO_->setSize(width_, height_);
        selectionFBO_->bind();
        selectionFBO_->attachTexture(GL_COLOR_ATTACHMENT0, selectionTexture_);
        selectionFBO_->attachTexture(GL_COLOR_ATTACHMENT1, debugTexture_);
        glAssert( glReadBuffer(GL_COLOR_ATTACHMENT0) );
        GLenum bufs[]={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, bufs);
        selectionFBO_->check();
        selectionFBO_->unbind();
        glAssert( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    }

}

int Picker::select(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, int x, int y) {
    int selectionresult[4];

    glm::ivec4 nullselection(-1, -1, -1, -1);
    selectionFBO_->useAsTarget(width(), height());
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glAssert( glClearBufferiv(GL_COLOR, 0, glm::value_ptr(nullselection)) );
    glAssert( glClearBufferfv(GL_COLOR, 1, glm::value_ptr(glm::vec4(glm::vec3(0.f), 1.f))) );
    shader_->bind();
    selectionloop_->draw(modelViewMatrix, projectionMatrix);
    glAssert(glReadPixels(x, y, 1, 1, GL_RGBA_INTEGER, GL_INT, selectionresult));
    selectionFBO_->unbind();
    glAssert( glBindFramebuffer(GL_FRAMEBUFFER, 0) );

    selectedmaterial_ = selectionresult[0];
    selectedmesh_ = selectionresult[1];
    selectedface_ = selectionresult[2];

    return selectionresult[3]!=-1;
}

vortex::AssetManager *Picker::assetmanager() const {
    return assetmanager_;
}

void Picker::setAssetmanager(vortex::AssetManager *assetmanager) {
    assetmanager_ = assetmanager;
}

vortex::SceneGraph *Picker::scenegraph() const {
    return scenegraph_;
}

void Picker::setScenegraph(vortex::SceneGraph *scenegraph) {
    scenegraph_ = scenegraph;
}

int Picker::width() const {
    return width_;
}

void Picker::setWidth(int width) {
    width_ = width;
}

int Picker::height() const {
    return height_;
}

void Picker::setHeight(int height) {
    height_ = height;
}

int Picker::selectedmesh() const {
    return selectedmesh_;
}

void Picker::setSelectedmesh(int selectedmesh) {
    selectedmesh_ = selectedmesh;
}

int Picker::selectedface() const {
    return selectedface_;
}

void Picker::setSelectedface(int selectedface) {
    selectedface_ = selectedface;
}

int Picker::selectedmaterial() const {
    return selectedmaterial_;
}

void Picker::setSelectedmaterial(int selectedmaterial) {
    selectedmaterial_ = selectedmaterial;
}

/***************************************/

//-------------------------------------------------------------------------------
// Identifiable objects for selection rendering
//-------------------------------------------------------------------------------


void IdentifiableMesh::draw() {
//    std::cerr << "Dessin du maillage " << themesh_->meshId() << std::endl;
    shader_->setUniform("meshId", themesh_->meshId());
    themesh_->draw();
}

IdentifiableMaterialState:: IdentifiableMaterialState(Material *mat, ShaderProgram *shaderId) : Bindable(), mMaterial(mat), mShader(shaderId) {}


void IdentifiableMaterialState::bind() const {
    if (mMaterial) {
        bind(mMaterial, mShader);
    }

}

void IdentifiableMaterialState::bind(Material *mat, ShaderProgram *shader) {
//    std::cerr << "Bind du materiau " << mat->materialId() << std::endl;

    // for identifying the material when in selection mode
    shader->setUniform("materialId", mat->materialId());
    // TODO : in a future evolution, make this allowable (using configurations for selection shader ...
#if 0
    // allow objects to have an opacity map
    Texture * tex = mat->getTexture(Material::TEXTURE_OPACITY);
    if (tex) {
        shader->setUniform("opacityLevel", 0.01f);
        shader->setUniformTexture("map_opacity", tex);
    } else {
        shader->setUniform("opacityLevel", 0.01f);
    }
#endif
}

bool IdentifiableMaterialState::operator< (const IdentifiableMaterialState &other) const {
    return (mMaterial->materialId() < other.mMaterial->materialId());
}

/*
 * -------------------------------------------------------------------------------
 */
SelectionLoopBuilder::SelectionLoopBuilder(AssetManager *resourcesManager, SceneGraph *sceneGraph, SelectionLoop *loop, ShaderProgram *shader)
    : mResourcesManager(resourcesManager), mSceneGraph(sceneGraph), mLoop(loop), mShader(shader){
}

void SelectionLoopBuilder::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {
    if (theNode->isLeaf()) {
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
        RenderState *state = NULL;
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]){
                state = leafNode->getRenderState(i);
                // build a shaderConfiguration object from material
                Material *nodeMaterial = state->getMaterial();
                (*mLoop)[ IdentifiableMaterialState(nodeMaterial, mShader)][ TransformState(modelViewMatrix, projectionMatrix, mShader)].push_back( IdentifiableMesh((*leafNode)[i], mShader) );
            }
        }
    }
}

template<>
void DrawableMap<IdentifiableMaterialState, MeshSelectionLoop>::draw(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix) {
    for (typename DrawableMap< IdentifiableMaterialState, MeshSelectionLoop >::iterator it = this->begin(); it != this->end(); ++it) {
        it->first.bind();
        it->second.draw(modelviewMatrix, projectionMatrix);
    }
}


template<>
void DrawableMap<TransformState, DrawableVector<IdentifiableMesh>>::draw(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix) {
    for (typename DrawableMap< TransformState, DrawableVector<IdentifiableMesh> >::iterator it = this->begin(); it != this->end(); ++it) {
        it->first.activate(modelviewMatrix, projectionMatrix);
        it->second.draw();
    }
}


} // namespace vortex
