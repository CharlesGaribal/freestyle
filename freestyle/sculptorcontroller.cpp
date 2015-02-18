#include "sculptorcontroller.h"

#include "ftylrenderer.h"
#include "camera.h"
#include "scenegraph.h"

SculptorController::SculptorController(FtylRenderer *renderer) :
    sculptor(),
    renderer(renderer)
{
}

void SculptorController::mouseMoveEvent(QMouseEvent *e, int *selectionBuffer, bool vertexSelected) {
    if (vertexSelected) {
        FindSelectedVertex findSelectedVertex(selectionBuffer[1], selectionBuffer[2]);
        vortex::SceneGraph::PreOrderVisitor visitor(renderer->getScene()->sceneGraph(), findSelectedVertex);
        visitor.go();

        vortex::Mesh::VertexData vertexSelected = findSelectedVertex.getVertexSelected();

        std::cout << "Vertex selected : (" << vertexSelected.mVertex.x << ", " << vertexSelected.mVertex.y << ", " << vertexSelected.mVertex.z << ")" << std::endl;

        renderer->setVertexSelected(vertexSelected.mVertex);
    } else
        renderer->noVertexSelected();
}

void FindSelectedVertex::operator()(vortex::SceneGraph::Node *theNode) {
    if (theNode->isLeaf()) {
        vortex::SceneGraph::LeafMeshNode *leafNode = static_cast<vortex::SceneGraph::LeafMeshNode *>(theNode);
        for (int i = 0; i < leafNode->nMeshes(); ++i) {
            if ((*leafNode)[i]) {
                vortex::Mesh::MeshPtr mesh = (*leafNode)[i];
                if (mesh->meshId() == meshSelectedId) {
                    vertexSelected = mesh->vertices()[mesh->indices()[faceSelectedId*3]];
                }
            }
        }
    }
}

vortex::Mesh::VertexData FindSelectedVertex::getVertexSelected() const {
    return vertexSelected;
}
