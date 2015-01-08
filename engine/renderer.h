/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "scenemanager.h"
#include "visitors.h"
#include "renderstate.h"
#include "texture.h"
#include "fbo.h"

namespace vortex {


/**
 *  @defgroup RenderersGroup Renderers for vortex engine.
 *  Here are defined all renderers to be used with VortexEngine
 */

/**
 * Abstract class that must render a scenegraph.
 * User willing to implement their own rendering system must derive this class and implement
 * all virtual methods.
 *
 * This class has a reference to the scene manager. there is, for the moment, no way to have
 * scene manager interact with renderers. see todo page for more information
 *
 * @todo think and implement a way to have bidirectional interaction between scene manager
 * and renderers.
 *
 * @ingroup RenderersGroup
 */
class Renderer {
public:
    /**
     * Constructor : the sceneManager parameter contain all information about the scene to render.
     *
     */
    Renderer(SceneManager *sceneManager) : mSceneManager(sceneManager) {}
    /**
     * Destructor
     */
    virtual ~Renderer() {}

    /**
     * Set output dimensions for the renderer.
     *
     * @param width width of the viewport
     * @param height height of the viewport
     *
     * This method must be called when subclassing as Renderer::initialize(width, height) an is responsible for FBOs resizing.
     */
    virtual void setViewport(int width, int height) {
        mWidth = width;
        mHeight = height;
        glAssert(glViewport(0, 0, mWidth, mHeight));
    }


    virtual void updateAnimations(double time) {
        (void)time;
    }

    virtual void setRenderingMode(int mode) {
        (void)mode;
    }

    /**
     * Render the scene.
     */
    virtual void render(vortex::Camera* camera) {}

    /**
      render scene from matrices
      @todo comprendre pourquoi clang dit hidden virtual function si on ne déclare pas ça ...
    */
    virtual void render(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {}

    virtual void initRessources(AssetManager *asset){

    }

    virtual float readDepthAt(int x, int y) {
        return 1.f;
    }

    int width(){ return mWidth; }
    int height(){ return mHeight; }

    void setCamera(Camera *cam ){ mCamera=cam;}

    Camera *getCamera(){ return mCamera; }

    SceneManager *getScene() {return mSceneManager; }

    virtual void reloadShaders(){}

protected:
    SceneManager *mSceneManager;
    Camera *mCamera;
    int mWidth;
    int mHeight;
};


/**
 * Picking Renderer
 * @ingroup RenderersGroup
 */
class SelectionRenderer : public Renderer {
public:
    /**
     * Constructor : initialize the SelectionRenderer attributes.
     *
     * @param sceneManager The SceneManager responsible for the scene to be rendered.
     * @param width Specify the render window width for FBO managing.
     * @param height Specify the render window height for FBO managing.
     */
    SelectionRenderer(SceneManager *sceneManager, int width, int height) : Renderer(sceneManager),
        mFBO(FBO(FBO::Components(FBO::DEPTH | FBO::COLOR), width, height)), mVerticeIdFBO(FBO(FBO::Components(FBO::DEPTH | FBO::COLOR), width, height)),
        mMode(false), mPrimitive(false)/*, mShowBboxes(false)*/ {
        // building shaderProgram and renderstate for BBox rendering
        ShaderConfiguration config;
        //config.addProperty("SHOW_BBOX");
        //ShaderProgram *bboxProg = mSceneManager->getAsset()->getShaderProgram(config);
        //mBboxRenderState.setShaderProgram(bboxProg);

        // building SelectionGroup
        /* first shaderConfigs */
        mMeshIdConfiguration = ShaderConfiguration(ShaderConfiguration::DEFAULT, "setPickingId");
        mMeshIdConfiguration.addProperty("MESH_ID");

        mFaceIdConfiguration = ShaderConfiguration(ShaderConfiguration::DEFAULT, "setPickingId");
        mFaceIdConfiguration.addProperty("FACE_ID");

        mVerticeIdConfiguration = ShaderConfiguration(ShaderConfiguration::ALL, "verticeId");

        mShowPrimitiveConfiguration = ShaderConfiguration();
        mShowPrimitiveConfiguration.addProperty("SHOW_HALF_WIREFRAMED");

        mShowSelectedPrimitiveConfiguration = ShaderConfiguration();
        mShowSelectedPrimitiveConfiguration.addProperty("SHOW_SELECTED_PRIMITIVE");

        /* then group */
//      mGroup = new SelectionShaderGroup( mSceneManager->getAsset()->getShaderProgram(mMeshIdConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mFaceIdConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mVerticeIdConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mShowPrimitiveConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mShowSelectedPrimitiveConfiguration), mProjection );

        // building id Texture
        mIdMap = new Texture("Mesh_Face_Id", GL_TEXTURE_RECTANGLE);
        mVerticeIdMap = new Texture("Vertice_Id", GL_TEXTURE_RECTANGLE);

    }

    ~SelectionRenderer() {
        delete mIdMap;
        delete mVerticeIdMap;
    }

    void setViewport(int width, int height);

    void render(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);

    void showBboxes(bool show) {
        //mShowBboxes = show;
    }
    bool isSelectionRenderer() {
        return true;
    }

    /**
     * Select the mesh rendered at position ("x", "y") in the render window.
     *
     */
    void pick(int x, int y);

    /**
     * Reset the SelectionRenderer.
     *
     */
    void reset() {
        mMode = false;
        mPrimitive = false;
//      mGroup = new SelectionShaderGroup( mSceneManager->getAsset()->getShaderProgram(mMeshIdConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mFaceIdConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mVerticeIdConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mShowPrimitiveConfiguration),
//                                         mSceneManager->getAsset()->getShaderProgram(mShowSelectedPrimitiveConfiguration), mProjection );
    }

    /**
     * Specify the rendering mode.
     *
     * @param mode The mode t be used for rendering. True specify the edit mode, false the object mode.
     *
     * @return True if the mode has been set.
     * @return False if the mode to be set is the edit mode but no mesh has been picked.
     */
    bool setMode(bool mode) {
        if ((mode /*&& mGroup->hasSelectedMesh()*/) || !mode) {
            mMode = mode;
            return true;
        } else
            return false;
    }

    /**
     * Specify the primitives to be picked when rendering in edit mode.
     *
     * @param primitives The primitive type to be selected. True specify faces to be picked, false vertices.
     */
    void selectPrimitive(bool primitive) {
        mPrimitive = primitive;
    }

private:
    FBO mFBO;
    FBO mVerticeIdFBO;
    Texture *mIdMap;
    Texture *mVerticeIdMap;
    ShaderConfiguration mMeshIdConfiguration;
    ShaderConfiguration mFaceIdConfiguration;
    ShaderConfiguration mVerticeIdConfiguration;
    ShaderConfiguration mShowPrimitiveConfiguration;
    ShaderConfiguration mShowSelectedPrimitiveConfiguration;
    glm::mat4x4 mModelView, mProjection;

//  Mesh *mScreenQuad;

    bool mMode; // false = objectMode, true = EditMode (cf blender for mode job)
    bool mPrimitive;

//  SelectionShaderGroup *mGroup;
//    bool mShowBboxes;
//    RenderState mBboxRenderState;
};

}

#endif // RENDERER_H
