#ifndef SCULPTORCONTROLLER_H
#define SCULPTORCONTROLLER_H

#include "ftylrenderer.h"
#include "sculptor.h"
#include "picker.h"
#include "timer.h"

#include <QMouseEvent>

class MainWindow;

class FindSelectedVertex : public vortex::SceneGraph::VisitorOperation {
public:
    FindSelectedVertex(int meshSelectedId, int faceSelectedId, int width, int height, glm::vec2 clickPos) :
        meshSelectedId(meshSelectedId),
        faceSelectedId(faceSelectedId),
        halfScreen(width/2., height/2.),
        clickPos(clickPos)
    {}

    void operator()(vortex::SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);

    vortex::Mesh::VertexData getVertexSelected() const;

private:
    float dist(glm::vec2 p1, glm::vec2 p2) {
        return sqrt(pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2));
    }

    int meshSelectedId, faceSelectedId;
    vortex::Mesh::VertexData vertexSelected;

    glm::vec2 halfScreen;
    glm::vec2 clickPos;
};

class SculptorController
{
public:
    enum OperatorType {SWEEP, INFDEFLATE, TWIST, N};
    static const int NO_OPERATOR = -1;

    SculptorController(MainWindow *mw);
    ~SculptorController();

    SculptorParameters getParameters();

    void toolDialogHidden();

    void sweepSelected();
    void infDefSelected();
    void twistSelected();
    OperatorType getToolSelected();

    void mouseMoveEvent(QMouseEvent *e);
    void mouseWheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void sceneLoaded();

    void toolRadiusChanged(float value);
    float getToolRadius() const;

    float getMinToolRadius() const;
    float getMaxToolRadius() const;

    void subdivide();

private:
    Sculptor sculptor;
    MainWindow *mainWindow;
    vortex::Mesh::VertexData vertexSelected;
    bool validSelection;
    float minToolRadius, maxToolRadius;

    bool existMesh;

    vortex::Timer timerClick;
    float timeRefreshClick;
    bool mouseClicked;

    vortex::Timer timerPicking;
    float timeRefreshPicking;

    /* For picking */
    vortex::Picker *objectpicker;
    void select(int i, int j);
};

#endif // SCULPTORCONTROLLER_H
