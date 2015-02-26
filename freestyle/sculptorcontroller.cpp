#include "sculptorcontroller.h"

#include <QCoreApplication>

#include "mainwindow.h"
#include "meshconverter.h"
#include "operator.h"
#include "subdivider.h"

SculptorController::SculptorController(MainWindow *mw) :
    sculptor(),
    mainWindow(mw),
    minToolRadius(0.25f),
    maxToolRadius(1.f)
{
    validSelection = false;

    sculptor.addOperator(new SweepOperator());
    sculptor.addOperator(new InfDefOperator());
    sculptor.addOperator(new TwistOperator());

    sculptor.setCurrentOperator(INFDEFLATE);

    sculptor.setRadius(minToolRadius);
    mainWindow->getOGLWidget()->getRenderer()->toolRadiusChanged(minToolRadius);

    timeRefreshPicking = 0.01;
    timerPicking.start();
    objectpicker = NULL;

    mouseClicked = false;
    timeRefreshClick = 1./50.; // 5 fps
    timerClick.start();
}

SculptorController::~SculptorController() {
    if (objectpicker != NULL)
        delete objectpicker;
}

SculptorParameters SculptorController::getParameters() {
    return sculptor.getParameters();
}

void SculptorController::toolDialogHidden() {
    mainWindow->switchToolsVisibility(false);
}

void SculptorController::sweepSelected() {
    sculptor.setCurrentOperator(SWEEP);
    mainWindow->getToolsDialog()->setToolSelected(SWEEP);
}

void SculptorController::infDefSelected() {
    sculptor.setCurrentOperator(INFDEFLATE);
    mainWindow->getToolsDialog()->setToolSelected(INFDEFLATE);
}

void SculptorController::twistSelected() {
    sculptor.setCurrentOperator(TWIST);
    mainWindow->getToolsDialog()->setToolSelected(TWIST);
}

SculptorController::OperatorType SculptorController::getToolSelected() {
    return (SculptorController::OperatorType)(static_cast< SculptorController::OperatorType >(sculptor.getCurrentIndexOp()));
}

void SculptorController::mouseMoveEvent(QMouseEvent *e) {
    FtylRenderer *renderer = mainWindow->getOGLWidget()->getRenderer();

    if (e->modifiers() & Qt::ControlModifier) {
        timerPicking.stop();
        if (timerPicking.value() > timeRefreshPicking) {
            timerPicking.restart();
            select(e->pos().x(), renderer->getCamera()->height() - e->pos().y());

            if (validSelection)
                renderer->setVertexSelected(vertexSelected.mVertex);
            else
                renderer->noSelection();
        }
        else
            timerPicking.start();

        //*
        timerClick.stop();
        if (existMesh && validSelection && mouseClicked && timerClick.value() > timeRefreshClick) {
            timerClick.restart();

            vortex::Timer t;
            t.start();

            sculptor.loop(QuasiUniformMesh::Point(vertexSelected.mVertex.x, vertexSelected.mVertex.y, vertexSelected.mVertex.z));

            vortex::Mesh *m = renderer->getScene()->getAsset()->getMesh(0);
            m->release();
            QuasiUniformMesh pm;
            sculptor.getMesh(pm);
            MeshConverter::convert(&pm, m);
            m->init();

            t.stop();
            std::cout << "Timer deformation : " << t.value() << std::endl;
        }
        else
            timerClick.start();//*/
    } else
        renderer->noSelection();
}

void SculptorController::mouseWheelEvent(QWheelEvent *e) {
    if (e->modifiers() & Qt::ControlModifier) {
        float nSteps = 100;
        float stepRadius = (maxToolRadius - minToolRadius) / nSteps;
        int sign = e->delta() > 0 ? 1 : -1;

        float newRadius = sculptor.getRadius() + sign*stepRadius;
        if (newRadius >= minToolRadius && newRadius <= maxToolRadius)
            toolRadiusChanged(newRadius);
    }
}

void SculptorController::mousePressEvent(QMouseEvent *e) {
    if (e->modifiers() & Qt::ControlModifier && sculptor.getCurrentIndexOp() != NO_OPERATOR) {
        FtylRenderer *renderer = mainWindow->getOGLWidget()->getRenderer();

        Operator *op = sculptor.getOperator(sculptor.getCurrentIndexOp());
        OperatorType opType = static_cast< SculptorController::OperatorType >(sculptor.getCurrentIndexOp());
        switch (opType) {
            case SWEEP:
                break;
            case INFDEFLATE:
            {
                InfDefOperator *idop = dynamic_cast<InfDefOperator *>(op);
                assert(idop != NULL);

                if (e->button() == Qt::RightButton)
                    idop->setDirection(InfDefOperator::DEFLATE);
                else
                    idop->setDirection(InfDefOperator::INFLATE);
            }
            break;
            case TWIST:
            {
                TwistOperator *top = dynamic_cast<TwistOperator *>(op);
                assert(top != NULL);

                if (e->button() == Qt::RightButton)
                    top->setDirection(InfDefOperator::DEFLATE);
                else
                    top->setDirection(InfDefOperator::INFLATE);
            }
            break;
            default:
                break;
        }

        /*
        timerClick.restart();

        vortex::Timer t;
        t.start();

        sculptor.loop(QuasiUniformMesh::Point(vertexSelected.mVertex.x, vertexSelected.mVertex.y, vertexSelected.mVertex.z));

        vortex::Mesh *m = renderer->getScene()->getAsset()->getMesh(0);
        m->release();
        QuasiUniformMesh pm;
        sculptor.getMesh(pm);
        MeshConverter::convert(&pm, m);
        m->init();

        t.stop();
        std::cout << "Timer deformation : " << t.value() << std::endl;
        //*/
        mouseClicked = true;

    }
}

void SculptorController::mouseReleaseEvent(QMouseEvent *e) {
    mouseClicked = false;
}

void SculptorController::sceneLoaded() {
    FtylRenderer *renderer = mainWindow->getOGLWidget()->getRenderer();
    vortex::AssetManager *asset = renderer->getScene()->getAsset();

    vortex::Mesh *m = asset->getMesh(0);

    QuasiUniformMesh *pm = new QuasiUniformMesh(), pm2;

    MeshConverter::convert(m, pm);
    m->release();

    sculptor.setMesh(*pm);
    sculptor.getMesh(pm2);

    MeshConverter::convert(&pm2, m);
    m->init();

    mainWindow->getParametersDialog()->setParameters(sculptor.getParameters());

    if (objectpicker)
        delete objectpicker;
    objectpicker = new vortex::Picker(asset, renderer->getScene()->sceneGraph(), renderer->getCamera()->width(), renderer->getCamera()->height());

    existMesh = true;
}

void SculptorController::toolRadiusChanged(float value) {
    assert(value >= minToolRadius && value <= maxToolRadius);
    sculptor.setRadius(value);
    mainWindow->getToolsDialog()->setToolRadius(value);
    mainWindow->getOGLWidget()->getRenderer()->toolRadiusChanged(value);
}

float SculptorController::getToolRadius() const {
    return sculptor.getRadius();
}

float SculptorController::getMinToolRadius() const {
    return minToolRadius;
}

float SculptorController::getMaxToolRadius() const {
    return maxToolRadius;
}

void SculptorController::subdivide()
{
    QuasiUniformMesh *pm = new QuasiUniformMesh(), pm2;
    sculptor.getMesh(*pm);

    Subdivider::subdivide(*pm);

    FtylRenderer *renderer = mainWindow->getOGLWidget()->getRenderer();
    vortex::AssetManager *asset = renderer->getScene()->getAsset();
    vortex::Mesh *m = asset->getMesh(0);

    m->release();

    sculptor.setMesh(*pm);
    sculptor.getMesh(pm2);

    MeshConverter::convert(&pm2, m);
    m->init();
}

void SculptorController::select(int i, int j) {
    FtylRenderer *renderer = mainWindow->getOGLWidget()->getRenderer();
    vortex::Camera *camera = renderer->getCamera();
    objectpicker->setPickingViewport(camera->width(), camera->height());

    validSelection = objectpicker->select(camera->getModelViewMatrix(), camera->getProjectionMatrix(), i, j);
    int meshId = objectpicker->selectedmesh();
    int faceId = objectpicker->selectedface();

    if (validSelection) {
        FindSelectedVertex findSelectedVertex(meshId ,faceId, camera->width(), camera->height(), glm::vec2(i,j));
        vortex::SceneGraph::PreOrderVisitor visitor(renderer->getScene()->sceneGraph(), findSelectedVertex);
        visitor.go(camera->getModelViewMatrix(), camera->getProjectionMatrix());

        vertexSelected = findSelectedVertex.getVertexSelected();
    }
}


void FindSelectedVertex::operator()(vortex::SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {
    if (theNode->isLeaf()) {
        vortex::SceneGraph::LeafMeshNode *leafNode = static_cast<vortex::SceneGraph::LeafMeshNode *>(theNode);
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]) {
                vortex::Mesh::MeshPtr mesh = (*leafNode)[i];
                if (mesh->meshId() == meshSelectedId) {
                    //vertexSelected = mesh->vertices()[mesh->indices()[faceSelectedId*3]];

                    // TODO : Project each vertex of the triangle on the screen and find closest to the pixel selected
                    glm::mat4x4 MVP = projectionMatrix * modelViewMatrix;

                    float closestDist = FLT_MAX;
                    for (int i = 0 ; i <= 2; i++) {
                        vortex::Mesh::VertexData v = mesh->vertices()[mesh->indices()[faceSelectedId*3+i]];
                        glm::vec4 vproj = MVP * glm::vec4(v.mVertex, 1);
                        glm::vec3 vclipspc = glm::vec3(vproj) / vproj.w;
                        glm::vec2 vwindowspc = halfScreen * glm::vec2(vclipspc) + halfScreen;

                        float dist2d = dist(clickPos, vwindowspc);
                        if (dist2d < closestDist) {
                            vertexSelected = v;
                            closestDist = dist2d;
                        }
                    }
                }
            }
        }
    }
}

vortex::Mesh::VertexData FindSelectedVertex::getVertexSelected() const {
    return vertexSelected;
}

