/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include <iostream>
#include <cstdio>
#include <sstream>

#include "scenemanager.h"



namespace vortex {

SceneManager::SceneManager(AssetManager *assetManager) : mAssetManager(assetManager), mSceneGraph(NULL)
{
    mLoader = new AssimpLoader();
}

SceneManager::~SceneManager()
{
    delete mSceneGraph;
    delete mLoader;
}

bool SceneManager::loadScene(const std::string &filename)
{
    SceneGraph *graph = NULL;
    bool res = mLoader->loadScene(filename, *mAssetManager, &graph);
    if (res) {
        if (mSceneGraph)
            delete mSceneGraph;
        mSceneGraph = graph;

        {
            BBoxBuilder bboxBuilder;
            SceneGraph::PostOrderVisitor visit(mSceneGraph, bboxBuilder);
            visit.go(glm::mat4(1.f), glm::mat4(1.f));
        }
        //graph->printDebug();
    }
    return res;
}


void SceneManager::newScene() {
    delete mSceneGraph;
    mSceneGraph = NULL;
}


} // namespace vortex
