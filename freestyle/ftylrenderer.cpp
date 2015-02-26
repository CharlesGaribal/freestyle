#include "ftylrenderer.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include "../engine/timer.h"

#include "meshconverter.h"
#include "../engine/camera.h"

using namespace vortex;
using namespace vortex::util;

static GLenum bufs[]={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};

FtylRenderer::FtylRenderer(SceneManager *sceneManager, int width, int height) :  mRenderMode(0), mSceneManager(sceneManager) {

    glCheckError();

    mFbo = new FBO(FBO::Components(FBO::DEPTH | FBO::COLOR), width, height);

    glCheckError();

    mTextures[DEPTH_TEXTURE] = new Texture("depth", GL_TEXTURE_2D);
    mTextures[NORMAL_TEXTURE] = new Texture("normal", GL_TEXTURE_2D);
    mTextures[COLOR_TEXTURE] = new Texture("color", GL_TEXTURE_2D );

    glCheckError();

    mScreenQuad = ScreenQuadBuilder().build("ScreenQuad");
    mScreenQuad->init();

    glCheckError();

    FBO::bindDefault();
    glAssert( glDrawBuffer(GL_BACK) );
    glAssert( glReadBuffer(GL_BACK) );

    glCheckError();
}

FtylRenderer::~FtylRenderer() {
    delete mFbo;

    for (int i=0; i<NUM_TEXTURE; i++)
            delete mTextures[i];

    delete mScreenQuad;
}

void FtylRenderer::displayTexture(Texture * theTexture){
    FBO::bindDefault();
    glAssert( glDrawBuffer(GL_BACK); );
    glAssert( glDepthFunc(GL_ALWAYS) );
    glAssert(glViewport(0, 0, width(), height()));
    ShaderProgram *shader = mSceneManager->getAsset()->getShaderProgram(mDisplayShaderId);
    shader->bind();
    shader->setUniformTexture("color", theTexture);
    mScreenQuad->draw();
    glAssert( glDepthFunc(GL_LESS) );
}

void FtylRenderer::ambientPass(vortex::ShaderLoop &theRenderingLoop, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, const glm::mat4x4 &viewToWorldMatrix){
    // render ambient and normal
    ShadersGlobalParameters  ambientAndNormalParamameters;
    ambientAndNormalParamameters.addParameter("view2worldMatrix", viewToWorldMatrix);
    ambientAndNormalParamameters.addParameter("ambientIntensity", 1.0f );
    theRenderingLoop.draw(ambientAndNormalParamameters, modelViewMatrix, projectionMatrix);
}

void FtylRenderer::lightsPass(vortex::ShaderLoop &theRenderingLoop, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, const glm::mat4x4 &viewToWorldMatrix){
    if(mSceneManager->sceneGraph()->mLights.size()>0){
        for(unsigned int i = 0; i<mSceneManager->sceneGraph()->mLights.size(); ++i ){
            glm::mat4x4 lightMatrix = modelViewMatrix*mSceneManager->sceneGraph()->mLights[i].mTransform;
            Light l = Light(mSceneManager->sceneGraph()->mLights[i], lightMatrix);
            LightGlobalParameters lightParamameters(l);
            lightParamameters.addParameter("inverseViewMatrix", viewToWorldMatrix);
            lightParamameters.addParameter("vertexSelected", glm::vec4(mVertexSelected, 1.0));
            lightParamameters.addParameter("validSelection", mValidSelection);
            lightParamameters.addParameter("toolRadius", mToolRadius);
            theRenderingLoop.draw(lightParamameters, modelViewMatrix, projectionMatrix);
        }
    } else { // no lights in scene, set up a headlight
        Light l;
        l.mDiffuse=glm::vec3(4.0,4.0,4.0);
        LightGlobalParameters lightParamameters(l);
        lightParamameters.addParameter("inverseViewMatrix", viewToWorldMatrix);
        lightParamameters.addParameter("vertexSelected", glm::vec4(mVertexSelected, 1.0));
        lightParamameters.addParameter("validSelection", mValidSelection);
        lightParamameters.addParameter("toolRadius", mToolRadius);
        theRenderingLoop.draw(lightParamameters, modelViewMatrix, projectionMatrix);
    }
}

void FtylRenderer::renderFilled(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix){
    //
    // Important note before modifying this method :
    // see FtylRenderer::setViewport for FBO configurations
    //
    glm::mat4x4 viewToWorldMatrix = glm::inverse(modelViewMatrix);

    // draw the scene

    mFbo->useAsTarget(mWidth, mHeight);
    glAssert(glDrawBuffers(2, bufs));
    glAssert(glClearColor(0.1, 0.1, 0.1, 1.));
    glAssert(glClearDepth(1.0));
    glAssert(glDepthFunc(GL_LESS));
    glAssert(glDisable(GL_BLEND));

    mFbo->clear(FBO::ALL);// to clear all attached texture

    // render ambient and normal
    ambientPass(mAmbientAndNormalLoop, modelViewMatrix, projectionMatrix, viewToWorldMatrix);

    // setup per light rendering : blend each pass onto the previous one
    glAssert(glDrawBuffers(1, bufs));
    glAssert(glDepthFunc(GL_LEQUAL));
    glAssert(glEnable(GL_BLEND));
    glAssert(glBlendFunc(GL_ONE, GL_ONE));
    glDepthMask(GL_FALSE);

    glAssert(glEnable(GL_POLYGON_OFFSET_FILL));

    // render for each light
    lightsPass(mMainDrawLoop, modelViewMatrix, projectionMatrix, viewToWorldMatrix);

    glAssert(glDisable(GL_POLYGON_OFFSET_FILL));

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1,-1);

    glAssert(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    glAssert(glEnable(GL_LINE_SMOOTH));
    glAssert(glLineWidth(1.f) );

    glAssert(glDrawBuffers(1, bufs));
    glAssert(glDepthFunc(GL_LEQUAL));
    glAssert(glEnable(GL_BLEND));
    glAssert(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glAssert(glDepthMask(GL_FALSE));

    ShadersGlobalParameters ambientAndNormalParamameters;
    ambientAndNormalParamameters.addParameter("color", glm::vec4(0.7,0.7,1.0,1));
    mAmbientAndNormalLoop.draw(ambientAndNormalParamameters, modelViewMatrix, projectionMatrix);

    glAssert( glDisable(GL_BLEND) );
    glAssert( glDepthFunc(GL_LESS) );
    glAssert( glDepthMask(GL_TRUE) );
    glDisable(GL_POLYGON_OFFSET_LINE);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // restore parameter
    glAssert( glDisable(GL_BLEND) );
    glAssert( glDepthFunc(GL_LESS) );
    glDepthMask(GL_TRUE);

    glDepthFunc(GL_LESS);

}

void FtylRenderer::renderWireframe(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix){

    mFbo->useAsTarget(mWidth, mHeight);
    glAssert(glDrawBuffers(1, bufs));
    glAssert(glClearColor(0.1, 0.1, 0.1, 1.));
    glAssert(glClearDepth(1.0));
    glAssert(glDepthFunc(GL_LESS));
    glAssert(glDisable(GL_BLEND));

    mFbo->clear(FBO::ALL);// to clear all attached texture


    glAssert(glEnable(GL_POLYGON_OFFSET_FILL));
    glAssert(glPolygonOffset(1.0, 5));

    ShadersGlobalParameters  ambientAndNormalParamameters;
    ambientAndNormalParamameters.addParameter("color", glm::vec4(0.5,0.5,0.5,1));
    mAmbientAndNormalLoop.draw(ambientAndNormalParamameters, modelViewMatrix, projectionMatrix);

    glAssert(glDisable(GL_POLYGON_OFFSET_FILL));
    glAssert(glPolygonOffset(1.0, 0));

    glAssert(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    glAssert(glEnable(GL_LINE_SMOOTH));
    glAssert(glLineWidth(1.f) );

    glAssert(glDrawBuffers(1, bufs));
    glAssert(glDepthFunc(GL_LEQUAL));
    glAssert(glEnable(GL_BLEND));
    glAssert(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glAssert(glDepthMask(GL_FALSE));

    ambientAndNormalParamameters = ShadersGlobalParameters();
    ambientAndNormalParamameters.addParameter("color", glm::vec4(0.7,0.7,1.0,1));
    mAmbientAndNormalLoop.draw(ambientAndNormalParamameters, modelViewMatrix, projectionMatrix);

    glAssert( glDisable(GL_BLEND) );
    glAssert( glDepthFunc(GL_LESS) );
    glAssert( glDepthMask(GL_TRUE) );

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// --------------------------------------------
void FtylRenderer::render(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix){

    glAssert ( glClearColor ( 0.3f, 0.3f, 0.3f, 0.f ) );
    glAssert ( glClear ( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT ) );

    if ( ! mSceneManager->sceneGraph())
        return;
    {
        (*(mRenderOperators[mRenderMode]))(modelViewMatrix, projectionMatrix);
        displayTexture(mTextures[COLOR_TEXTURE]);
    }
}

void FtylRenderer::setViewport(int width, int height)
{
    //Renderer::setViewport(width, height);
    mWidth = width;
    mHeight = height;

    mFbo->setSize(mWidth, mHeight);

    if (mTextures[DEPTH_TEXTURE]->getId() != 0)
            mTextures[DEPTH_TEXTURE]->deleteGL();
    mTextures[DEPTH_TEXTURE]->initGL(GL_DEPTH_COMPONENT24, mWidth, mHeight, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    if (mTextures[NORMAL_TEXTURE]->getId() != 0)
        mTextures[NORMAL_TEXTURE]->deleteGL();
    if (mTextures[COLOR_TEXTURE]->getId() != 0)
        mTextures[COLOR_TEXTURE]->deleteGL();

    mTextures[NORMAL_TEXTURE]->initGL(GL_RGBA32F, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    mTextures[COLOR_TEXTURE]->initGL(GL_RGBA32F, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    mFbo->bind();
    mFbo->attachTexture(GL_COLOR_ATTACHMENT0, mTextures[COLOR_TEXTURE]);
    mFbo->attachTexture(GL_COLOR_ATTACHMENT1, mTextures[NORMAL_TEXTURE]);
    mFbo->attachTexture(GL_DEPTH_ATTACHMENT, mTextures[DEPTH_TEXTURE]);
    mFbo->check();
    mFbo->unbind();

    FBO::bindDefault();
    glAssert (glDrawBuffer(GL_BACK); );
    glAssert (glReadBuffer(GL_BACK); );
}


void FtylRenderer::initRessources(AssetManager *assetManager){

    assetManager->setShaderFileExtentions(std::string(".vert.glsl"), std::string(".frag.glsl"));
    assetManager->setShaderBasePath(std::string("../freestyle/freestyle/shaders/"));

    /*
     * Image Computation shaders
     */
    if(mSceneManager->sceneGraph()){
        // Ambient and normal computation
        {
            ShaderBuilder shaderCreatorVisitor(assetManager, "ambient", false);
            mAmbientAndNormalFilter = new MaterialPropertiesSelectorFilter;
            mAmbientAndNormalFilter->addProperty(Material::TEXTURE_OPACITY);
            mAmbientAndNormalFilter->addProperty(Material::TEXTURE_HEIGHT);
            mAmbientAndNormalFilter->addProperty(Material::TEXTURE_NORMALS);
            mAmbientAndNormalFilter->addProperty(Material::TEXTURE_AMBIENT);
            mAmbientAndNormalFilter->addProperty(Material::TEXTURE_DIFFUSE);
            mAmbientAndNormalFilter->addProperty(Material::TEXTURE_SPECULAR);
            shaderCreatorVisitor.setFilter(mAmbientAndNormalFilter);
            SceneGraph::PreOrderVisitor visit(mSceneManager->sceneGraph(), shaderCreatorVisitor);
            visit.go();
        }

        // Lighting computation
        {
            ShaderBuilder shaderCreatorVisitor(assetManager, "phong");
            SceneGraph::PreOrderVisitor visit(mSceneManager->sceneGraph(), shaderCreatorVisitor);
            visit.go();
        }

        buildRenderingLoops();
    }

    /*
     * Image post-processing shaders
     */
    mDisplayShaderId = assetManager->addShaderProgram("display");
    mRenderOperators.push_back(new FilledRenderOperator(this));
    mRenderOperators.push_back(new WireRenderOperator(this));

    if(mSceneManager->sceneGraph()){
        assetManager->statistics();
        GraphStatistic statistic;
        SceneGraph::PreOrderVisitor statisticVisitor(mSceneManager->sceneGraph(), statistic);
        statisticVisitor.go();
        std::cerr << statistic << std::endl;
    }

}

void FtylRenderer::buildRenderingLoops(){

    mMainDrawLoop.clear();
    mAmbientAndNormalLoop.clear();
    if (mRenderMode == 0) { // Fill mode
        // Light loop builder
        DefaultLoopBuilder mainDrawLoopBuilder(mSceneManager->sceneGraph(), &mMainDrawLoop);

        // Ambient and normal loop builder
        ShaderLoopBuilder ambientAndNormalLoopBuilder(mSceneManager->getAsset(), mSceneManager->sceneGraph(), &mAmbientAndNormalLoop, "ambient");
        ambientAndNormalLoopBuilder.setFilter(mAmbientAndNormalFilter);

        // Setup loop builder
        SetOfLoopBuilder renderPassesBuilder;
        renderPassesBuilder.addLoopBuilder(&mainDrawLoopBuilder);
        renderPassesBuilder.addLoopBuilder(&ambientAndNormalLoopBuilder);
        SceneGraph::PostOrderVisitor renderPassesVisitor(mSceneManager->sceneGraph(), renderPassesBuilder);
        renderPassesVisitor.go(glm::mat4(1.0), glm::mat4(1.0));

    } else { // WireMode
        ShaderLoopBuilder ambientAndNormalLoopBuilder(mSceneManager->getAsset(), mSceneManager->sceneGraph(), &mAmbientAndNormalLoop, "fill");
        ambientAndNormalLoopBuilder.setFilter(mAmbientAndNormalFilter);
        // Setup loop builder
        SceneGraph::PostOrderVisitor renderPassesVisitor(mSceneManager->sceneGraph(), ambientAndNormalLoopBuilder);
        renderPassesVisitor.go(glm::mat4(1.0), glm::mat4(1.0));
    }
}

float FtylRenderer::readDepthAt(int x, int y){
    float depth;
    mFbo->bind();
    glAssert( glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth) );
    mFbo->unbind();
    return depth;
}

