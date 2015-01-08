/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>

#include "assetmanager.h"
#include "scenegraph.h"
#include "assimploader.h"

#include "visitors.h"
#include "loader.h"

namespace vortex {
/**
 * Class which stores the information about scene.
 *
 * @see AssetManager
 * @see SceneGraph
 */

class SceneManager {
public:
    SceneManager(AssetManager *);
    virtual ~SceneManager();

    virtual void newScene();
    /**
     * Load the scene
     *
     * @param filename absolute path of a file to load
     * @return true if scene was load was successful, false otherwise
     */
    virtual bool loadScene(const std::string &filename);

    /**
     * Gets textual information about the last error that occurred
     */
    const char *getLastErrorString() const {
        return mLoader->getLastErrorString();
    }
    /**
     * Gets numerical information about the last error that occurred
     */
    int lastError() const {
        return mLoader->lastError();
    }

    /**
     * SceneGraph getter
     */
    SceneGraph *sceneGraph() const {
        return mSceneGraph;
    }

    /**
     * AssetManager getter
     */
    AssetManager *getAsset() {
        return mAssetManager;
    }

    void setAssetManager(AssetManager *assetManager ){mAssetManager = assetManager;}


    virtual BBox getBBox() {
        if (mSceneGraph)
            return mSceneGraph->getBBox();
        else
            return BBox();
    }
protected:

    Loader *mLoader;
    AssetManager *mAssetManager;
    SceneGraph *mSceneGraph;
};

} // namespace vortex
#endif
