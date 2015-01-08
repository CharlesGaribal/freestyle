/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef LOADER_H
#define LOADER_H

namespace vortex {

/**
 * Abstract class that must load a scene
 *
 */

class Loader {

public:
    Loader() {}
    virtual ~Loader() {}

    /**
     * Load the scene
     *
     * @param filename absolute path of a file to load
     * @param assetManager AssetManager to fill
     * @param sceneGraph SceneGraph to fill
     * @see AssetManager
     * @see SceneGraph
     *
     * @return true if scene has been successfully loaded, false otherwise
     */
    virtual bool loadScene(const std::string &filename, AssetManager &assetManager, SceneGraph **sceneGraph) = 0;

    /**
     * Gets textual information about the last error that occurred
     *
     * @return Last error as a string
     */
    virtual const char *getLastErrorString() const = 0;

    /**
     * Gets numerical information about the last error that occurred
     *
     * @return Last error code
     */
    virtual int lastError() const = 0;

};

}

#endif
