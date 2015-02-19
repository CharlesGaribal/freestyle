#ifndef ASSIMPEXPORTER_H
#define ASSIMPEXPORTER_H

#include "assimp/scene.h"
#include "assimp/Exporter.hpp"

#include "scenemanager.h"

namespace vortex {

class assimpexporter
{
public:
    assimpexporter();

    bool exportScene(std::string filename, std::string ext, SceneManager *sm);
    void buildAIScene(aiScene *out);

private:
    SceneManager *sceneManager;
    AssetManager *assetManager;
    SceneGraph *sceneGraph;
    aiMesh *buildAIMesh(Mesh *m);
};

}

#endif // ASSIMPEXPORTER_H
