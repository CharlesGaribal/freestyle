#ifndef SCULPTORCONTROLLER_H
#define SCULPTORCONTROLLER_H

#include "ftylrenderer.h"
#include "sculptor.h"
#include "picker.h"

#include <QMouseEvent>

class MainWindow;

class FindSelectedVertex : public vortex::SceneGraph::VisitorOperation {
public:
    FindSelectedVertex(int meshSelectedId, int faceSelectedId) :
        meshSelectedId(meshSelectedId),
        faceSelectedId(faceSelectedId)
    {}

    void operator()(vortex::SceneGraph::Node *theNode);

    vortex::Mesh::VertexData getVertexSelected() const;

private:
    int meshSelectedId, faceSelectedId;
    vortex::Mesh::VertexData vertexSelected;
};

class SculptorController
{
public:
    enum OperatorType {SWEEP, INFDEFLATE, TWIST, N};
    static const int NO_OPERATOR = -1;

    SculptorController(MainWindow *mw);

    SculptorParameters getParameters();

    void toolDialogHidden();

    void sweepSelected();
    void infDefSelected();
    void twistSelected();
    OperatorType getToolSelected();

    void mouseMoveEvent(QMouseEvent *e, int *selectionBuffer, bool found);
    void mouseWheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void sceneLoaded();

    void toolRadiusChanged(float value);
    float getToolRadius() const;

    float getMinToolRadius() const;
    float getMaxToolRadius() const;

private:
    Sculptor sculptor;
    MainWindow *mainWindow;
    vortex::Mesh::VertexData vertexSelected;
    bool validSelection;

    float minToolRadius, maxToolRadius;
};

#endif // SCULPTORCONTROLLER_H
