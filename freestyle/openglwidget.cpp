#include "openglwidget.h"

#include "../engine/timer.h"

#include "meshconverter.h"

// vortex
#include <cameracontroller.h>
//Qt
#include <QTimer>
#include <QWheelEvent>

#include <QtWidgets/QPushButton>

//glm
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace vortex;
using namespace vortex::ui;

void cerrInfoGL(OpenGLWidget *w) {
    const GLubyte *str;
    glAssert( str = glGetString(GL_RENDERER) );
    std::cerr << "Renderer : " << str << std::endl;
    glAssert( str = glGetString(GL_VENDOR) );
    std::cerr << "Vendor : " << str << std::endl;
    glAssert( str = glGetString(GL_VERSION) );
    std::cerr << "Version : " << str << std::endl;
    glAssert( str = glGetString(GL_SHADING_LANGUAGE_VERSION) );
    std::cerr << "GLSL Version : " << str << std::endl;

    if (!w->isValid()){
        std::cerr << " Invalid (sic) "<< std::endl;
    }
}

OpenGLWidget::OpenGLWidget ( QWidget *parent ) : QGLWidget ( parent ), camera_(NULL) {
    setFocusPolicy ( Qt::StrongFocus );
    makeCurrent();
    cerrInfoGL(this);
    objectpicker_ = NULL;
    assetManager_ = new AssetManager();
    sceneManager_ = new SceneManager(assetManager_);
    renderer_ = new FtylRenderer(sceneManager_);
    sculptorController = new SculptorController(renderer_);
    timeRefreshPicking = 0.1;
    timerPicking.start();
    cameraController_ = NULL;
}

//#endif

OpenGLWidget::~OpenGLWidget() {
    delete renderer_;
}

void OpenGLWidget::clear() {
    updateGL();
}

void OpenGLWidget::initializeGL() {
    std::cerr << "initalizeGL" <<std::endl;

    /* initialise openGL */
    glAssert(glEnable(GL_DEPTH_TEST));
    glAssert(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    // WARNING : uncoment the next line gives segfault on GeForce 8800 GTX :(
    glAssert(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

    renderer_->initRessources(assetManager_);

    width_=10;
    height_=10;
    resetCamera();
    renderer_->setCamera(camera_);

    glCheckError();
}

void OpenGLWidget::resizeGL ( int w, int h ) {
    width_=w;
    height_=h;
    renderer_->setViewport(width_, height_);
    glCheckError();
}

void OpenGLWidget::paintGL() {
    glCheckError();

    camera_->setScreenWidthAndHeight(width_, height_);
    camera_->computeModelViewMatrix();
    camera_->computeProjectionMatrix();
    glm::mat4x4 modelViewMatrix = camera_->getModelViewMatrix();
    glm::mat4x4 projectionMatrix = camera_->getProjectionMatrix();

    renderer_->render(modelViewMatrix, projectionMatrix);
}

void OpenGLWidget::keyPressEvent ( QKeyEvent * e ) {
    std::cerr <<"KeyPressEvent"<< std::endl;
    switch(e->key()) {
    case Qt::Key_R:
        resetCamera();
        break;
    case Qt::Key_Q:
        camera_->moveUp(1.f);
        break;
    case Qt::Key_E:
        camera_->moveUp(-1.f  );
        break;
    case Qt::Key_W:
        camera_->moveForward(1.f  );
        break;
    case Qt::Key_S:
        camera_->moveForward(-1.f  );
        break;
    case Qt::Key_A:
        camera_->moveRight(-1.f  );
        break;
    case Qt::Key_D:
        camera_->moveRight(1.f  );
        break;


    case Qt::Key_Y:
        camera_->rotateAroundUp(1.f);
        break;
    case Qt::Key_I:
        camera_->rotateAroundUp(-1.f  );
        break;
    case Qt::Key_U:
        camera_->rotateAroundView(1.f  );
        break;
    case Qt::Key_J:
        camera_->rotateAroundView(-1.f  );
        break;
    case Qt::Key_H:
        camera_->rotateAroundRight(-1.f  );
        break;
    case Qt::Key_K:
        camera_->rotateAroundRight(1.f  );
        break;
    }
    updateGL();
}


void OpenGLWidget::wheelEvent ( QWheelEvent * e ) {
    cameraController_->wheelEvent(e);
    updateGL();
}

void OpenGLWidget::mouseMoveEvent ( QMouseEvent * e ) {
    if (e->modifiers() & Qt::ControlModifier)
        cameraController_->mouseMoveEvent(e);

    timerPicking.stop();
    if (timerPicking.value() > timeRefreshPicking) {
        int selectionBuffer[4];
        int found;
        camera_->setScreenWidthAndHeight(width_, height_);
        camera_->computeModelViewMatrix();
        camera_->computeProjectionMatrix();
        glm::mat4x4 modelViewMatrix = camera_->getModelViewMatrix();
        glm::mat4x4 projectionMatrix = camera_->getProjectionMatrix();
        found = select(modelViewMatrix, projectionMatrix, e->pos().x(), height_ - e->pos().y(), selectionBuffer);
        if (found)
            std::cerr << "materialId : " << selectionBuffer[0] << " -- meshId : " << selectionBuffer[1] << " -- faceId : " << selectionBuffer[2] << std::endl;
        sculptorController->mouseMoveEvent(e, selectionBuffer, found);
        timerPicking.reset();
    }
    timerPicking.start();

    updateGL();
}

void OpenGLWidget::mouseReleaseEvent ( QMouseEvent *e ) {
    if (e->modifiers() & Qt::ControlModifier)
        cameraController_->mouseReleaseEvent(e);
}


void OpenGLWidget::mousePressEvent ( QMouseEvent * e ) {
    if (e->modifiers() & Qt::ControlModifier)
        cameraController_->mousePressEvent(e);
    updateGL();
}

void OpenGLWidget::switchRenderingMode(bool on){
    renderer_->setRenderingMode((int)on);
    updateGL();
}

void OpenGLWidget::resetCamera(){
    if (camera_){
        delete camera_;
        delete cameraController_;
    }
    camera_ = new Camera;
    cameraController_ = new CameraController(camera_);
    camera_->setScreenWidthAndHeight(width_, height_);
    if (sceneManager_ && sceneManager_->sceneGraph()) {
        camera_->setSceneBoundingBox(sceneManager_->getBBox().getMin(),
                                     sceneManager_->getBBox().getMax());
    }
    camera_->showEntireScene();

    camera_->computeModelViewMatrix();
    camera_->computeProjectionMatrix();
    //! \todo
    //mCamera->frame()->setRotationSensitivity(2);
    camera_->setRevolveAroundPoint(camera_->sceneCenter());

    renderer_->setCamera(camera_);
}

bool OpenGLWidget::loadScene(std::string fileName){
    std::cerr << "*** VortexEngine::loadScene ***" << std::endl;
    sceneManager_->newScene();
    delete assetManager_;
    assetManager_ = new AssetManager();
    sceneManager_->setAssetManager(assetManager_);

    bool ret = sceneManager_->loadScene(fileName);

    if(ret) {
        BBox sceneBox = sceneManager_->getBBox();
        glm::vec3 ext=sceneBox.getExtend();

        std::cerr << "Bbox : " << sceneBox <<  std::endl;
        std::cerr << "Extend : " << ext[0] << " " << ext[1] << " " << ext[2] <<  std::endl;
        resetCamera();

        /* For picking */
        if (objectpicker_)
                delete objectpicker_;
        objectpicker_ = new Picker(assetManager_, sceneManager_->sceneGraph(), width_, height_);

        // here the rendering loops are constructed, any modification on the scene graph must be done before (such as subdivision ...)
        renderer_->initRessources(assetManager_);
    }
    return ret;
}


/* For picking */
int OpenGLWidget::select(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, int i, int j, int *selectionBuffer) {
    objectpicker_->setPickingViewport(width_, height_);

    int found = objectpicker_->select(modelViewMatrix, projectionMatrix, i, j);
    selectionBuffer[0] = objectpicker_->selectedmaterial();
    selectionBuffer[1] = objectpicker_->selectedmesh();
    selectionBuffer[2] = objectpicker_->selectedface();
    selectionBuffer[3] = found; // may be replaced by another picking attributes
    return found;
}
