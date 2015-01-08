/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "printgraph.h"
namespace vortex {

PrintGraph::PrintAction PrintGraph::mPrintAction;

void PrintGraph::run(SceneGraph::Node *theNode) {
    if (theNode->acceptVisitors()) {
        mAction(theNode);
        mPrintAction.incrDepth();
        if (!theNode->isLeaf()) {
            SceneGraph::InnerNode * innerNode = static_cast<SceneGraph::InnerNode *>(theNode);
            for (int i = 0; i < innerNode->nChilds(); ++i) {
                run((*innerNode)[i]);
            }
        }
        mPrintAction.decrDepth();
    }
}
void PrintGraph::run(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {

}


}

