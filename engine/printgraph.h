/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef PRINTGRAPH_H
#define PRINTGRAPH_H

#include "visitors.h"
#include "scenegraph.h"

/**
 * Basic fonction to print the scene graph using visitors
 * @author Alexandre Bonhomme
 */

namespace vortex {

class PrintGraph : public SceneGraph::Visitor {

public:
    PrintGraph(SceneGraph *theGraph) :
        Visitor(theGraph, mPrintAction)
     {
     }

    void run(SceneGraph::Node *theNode);
    void run(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);

private:


    class PrintAction : public SceneGraph::VisitorOperation {
    public:
        PrintAction() {mDepth=0;}

        void operator()(SceneGraph::Node *theNode) {
            for (int i=0; i< mDepth; i++)
                std::cerr << "-=-";
            std::cerr << "->  Node name: " << theNode->name() << std::endl;
        }
        void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix) {
        }

        void incrDepth(){mDepth++;}
        void decrDepth(){mDepth--;}
    private:
        int mDepth;
   };

    static PrintAction mPrintAction;
};

}
#endif // PRINTGRAPH_H
