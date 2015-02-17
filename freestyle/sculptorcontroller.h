#ifndef SCULPTORCONTROLLER_H
#define SCULPTORCONTROLLER_H

#include "ftylrenderer.h"
#include "sculptor.h"
#include "picker.h"

#include <QMouseEvent>

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
    SculptorController(FtylRenderer *renderer);

    void mouseMoveEvent(QMouseEvent *e, int *selectionBuffer, bool vertexSelected);

    Sculptor *getSculptor() {
        return &sculptor;
    }

private:
    Sculptor sculptor;
    FtylRenderer *renderer;
};

#endif // SCULPTORCONTROLLER_H
