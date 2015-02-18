#include "assimpexporter.h"

namespace vortex {

assimpexporter::assimpexporter()
{
}

bool assimpexporter::exportScene(std::string filename, std::string ext, SceneManager *sm)
{
    Assimp::Exporter exporter;

    sceneManager = sm;
    assetManager = sm->getAsset();
    sceneGraph = sm->sceneGraph();

    aiScene* scene = new aiScene();

    buildAIScene(scene);

    aiReturn r;

    if(ext == "dae")
        r = exporter.Export(scene, "collada", filename);
    else if(ext == "obj")
        r = exporter.Export(scene, "obj", filename);
    else
        std::cerr << "Extension non prise en compte: " << ext << std::endl;

    if(AI_SUCCESS == r)
        return true;
    else
        return false;
}

void assimpexporter::buildAIScene(aiScene *out)
{
    out->mRootNode = new aiNode();

    out->mMaterials = new aiMaterial*[ 1 ];
    out->mMaterials[ 0 ] = nullptr;
    out->mNumMaterials = 1;

    out->mMaterials[ 0 ] = new aiMaterial();

    out->mMeshes = new aiMesh*[ assetManager->numMeshs() ];
    out->mMeshes[ 0 ] = nullptr;
    out->mNumMeshes = assetManager->numMeshs();

    out->mMeshes[ 0 ] = new aiMesh();
    out->mMeshes[ 0 ]->mMaterialIndex = 0;

    out->mRootNode->mMeshes = new unsigned int[ assetManager->numMeshs() ];
    out->mRootNode->mMeshes[ 0 ] = 0;
    out->mRootNode->mNumMeshes = assetManager->numMeshs();

    //meshes
    //for(int i = 0; i < assetManager->numMeshs(); ++i)
    {
        out->mMeshes[i] = buildAIMesh(assetManager->getMesh(i));
    }
}

aiMesh* assimpexporter::buildAIMesh(vortex::Mesh *m)
{
    aiMesh *ai_mesh = new aiMesh();

    int numVert = m->numVertices();
    int numInd = m->numIndices();
    ai_mesh->mVertices = new aiVector3D[numVert];
    ai_mesh->mNormals = new aiVector3D[numVert];
    ai_mesh->mTextureCoords[0] = new aiVector3D[numVert];
    ai_mesh->mNumUVComponents[0] = numVert;
    ai_mesh->mTangents = new aiVector3D[numVert];
    ai_mesh->mNumVertices = numVert;
    ai_mesh->mNumFaces = numInd/3;
    ai_mesh->mFaces = new aiFace[numInd/3];
    ai_mesh->mName = m->name().data();

    for (int i = 0; i < numVert; ++i)
    {
        ai_mesh->mVertices[i] = aiVector3D(m->vertices()[i].mVertex.x,
                                           m->vertices()[i].mVertex.y,
                                           m->vertices()[i].mVertex.z);

        ai_mesh->mNormals[i] = aiVector3D(m->vertices()[i].mNormal.x,
                                           m->vertices()[i].mNormal.y,
                                           m->vertices()[i].mNormal.z);

        ai_mesh->mTextureCoords[0][i] = aiVector3D(m->vertices()[i].mTexCoord.x,
                                           m->vertices()[i].mTexCoord.y,
                                           m->vertices()[i].mTexCoord.z);

        ai_mesh->mTangents[i] = aiVector3D(m->vertices()[i].mTangent.x,
                                           m->vertices()[i].mTangent.y,
                                           m->vertices()[i].mTangent.z);
    }

    int k = 0;
    for(int i = 0; i < numInd/3; i++)
    {
        aiFace &face = ai_mesh->mFaces[i];
        face.mIndices = new unsigned int[3];
        face.mNumIndices = 3;

        face.mIndices[0] = m->indices()[k];
        face.mIndices[1] = m->indices()[k+1];
        face.mIndices[2] = m->indices()[k+2];
        k = k + 3;
    }

    return ai_mesh;
}

}
