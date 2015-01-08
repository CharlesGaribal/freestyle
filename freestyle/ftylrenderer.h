#ifndef FTYLRENDERER_H
#define FTYLRENDERER_H

#include "renderer.h"
#include "skybox.h"
#include "scenemanager.h"
#include "camera.h"

#include <string>

class FtylRenderer {
public:
    enum TextureId {COLOR_TEXTURE, DEPTH_TEXTURE, NORMAL_TEXTURE, NUM_TEXTURE};

    // Constructors -- Destructors
    FtylRenderer(vortex::SceneManager *sceneManager, int width = 1, int height = 1);
    virtual ~FtylRenderer();

    // Internal classes
    class RenderOperator{
    protected:
        FtylRenderer *mAssociatedRenderer;
        RenderOperator() : mAssociatedRenderer(NULL) {}
    public:
        RenderOperator(FtylRenderer *theRenderer) : mAssociatedRenderer(theRenderer) {}
        virtual ~RenderOperator(){}
        virtual void operator()(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) = 0;
    };

    // Public methods
    void buildRenderingLoops();
    void render(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);

    void setViewport(int width, int height);

    void initRessources(vortex::AssetManager *assetManager);

    void setRenderingMode(int mode) {
        bool needLoopRebuild = (mRenderMode != mode);
        mRenderMode=mode;
        if (needLoopRebuild)
            buildRenderingLoops();
    }
    bool getRenderMode() const {
        return (mRenderMode==0);
    }
    float readDepthAt(int x, int y);

    void drawScreenQuad() { mScreenQuad->draw(); }
    void reloadShaders();

private:
    /// Textures for all the rendering steps
    vortex::Texture *mTextures[NUM_TEXTURE];

    class FilledRenderOperator : public RenderOperator{
        FilledRenderOperator() : RenderOperator(){}
    public:
        FilledRenderOperator(FtylRenderer *theRenderer) : RenderOperator(theRenderer) {}
        ~FilledRenderOperator(){}
        void operator()(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix){
            mAssociatedRenderer->renderFilled(modelViewMatrix, projectionMatrix);
        }
    };

    class WireRenderOperator : public RenderOperator{
        WireRenderOperator() :  RenderOperator(){}
    public:
        WireRenderOperator(FtylRenderer *theRenderer) : RenderOperator(theRenderer) {}
        ~WireRenderOperator(){}
        void operator()(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix){
            mAssociatedRenderer->renderWireframe(modelViewMatrix, projectionMatrix);
        }
    };

    std::vector<RenderOperator*> mRenderOperators;

    void renderFilled(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
    void renderWireframe(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);

    void drawSkyBox(int shaderId, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
    void ambientPass(vortex::ShaderLoop &theRenderingLoop, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, const glm::mat4x4 &viewToWorldMatrix);
    void lightsPass(vortex::ShaderLoop &theRenderingLoop, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, const glm::mat4x4 &viewToWorldMatrix);
    void displayTexture(vortex::Texture * theTexture);

    vortex::FBO *mFbo;
    vortex::Mesh *mScreenQuad;

    /* Image processing shaders */
    int mDisplayShaderId;

    vortex::MaterialPropertyFilter *mAmbientAndNormalFilter;
    vortex::MaterialPropertyFilter *mDepthFilter;

    int mRenderMode;

    // render loops
    vortex::ShaderLoop mMainDrawLoop;
    vortex::ShaderLoop mAmbientAndNormalLoop;
public:

    int width(){ return mWidth; }
    int height(){ return mHeight; }

    void setCamera(vortex::Camera *cam ){ mCamera=cam;}

    vortex::Camera *getCamera(){ return mCamera; }

    vortex::SceneManager *getScene() {return mSceneManager; }

protected:
    vortex::SceneManager *mSceneManager;
    vortex::Camera *mCamera;
    int mWidth;
    int mHeight;
};

#endif // FTYLRENDERER_H
