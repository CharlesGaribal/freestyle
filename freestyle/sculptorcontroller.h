#ifndef SCULPTORCONTROLLER_H
#define SCULPTORCONTROLLER_H

#include "ftylrenderer.h"
#include "sculptor.h"
#include "picker.h"

#include "toolsdialog.h"

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

    SculptorController(MainWindow *mw);

    void mouseMoveEvent(QMouseEvent *e, int *selectionBuffer, bool found);
    void mouseWheelEvent(QWheelEvent *e);

    void sceneLoaded();

    void toolRadiusChanged(float value);
    float getToolRadius() const;

    float getMinToolRadius() const;
    float getMaxToolRadius() const;

private:
    Sculptor sculptor;
    MainWindow *mainWindow;

    float minToolRadius, maxToolRadius;
};

#endif // SCULPTORCONTROLLER_H
