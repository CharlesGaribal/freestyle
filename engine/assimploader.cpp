/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "assimploader.h"


#include "assimp/DefaultLogger.hpp"
#define ASSIMPINFOINCLUDED
#include "printsceneinfo.cpp"
#undef ASSIMPINFOINCLUDED

#include "printgraph.h"
#include "animatedmesh.h"

#include "timer.h"

namespace vortex {

bool AssimpLoader::loadScene(const std::string &filename, AssetManager &am, SceneGraph **sg)
{

    mResources = &am;
    mSceneGraph = sg;

    Assimp::DefaultLogger::create("monlog.txt",Assimp::Logger::VERBOSE);

//     Create an instance of the Importer class
    Assimp::Importer importer;
    /// @todo try new assimp version for a hard bug when setting this.
    //importer.SetExtraVerbose(true);

//     And have it read the given file with some example postprocessing
//     Usually - if speed is not the most important aspect for you - you'll
//     propably to request more postprocessing than we do in this example.
    std::cerr << "Loading scene : " << filename << std::endl;
    Timer timeAssimp;
    timeAssimp.start();

    const aiScene* scene = importer.ReadFile(filename,
                           0 |
                           aiProcess_JoinIdenticalVertices         |
                           aiProcess_GenSmoothNormals              |
                           aiProcess_CalcTangentSpace              |
                           aiProcess_Triangulate                   |
                           aiProcess_SortByPType                   |
                           aiProcess_ImproveCacheLocality          |
                           aiProcess_SplitLargeMeshes              |
                           aiProcess_TransformUVCoords             |
                           aiProcess_RemoveRedundantMaterials      |
                           aiProcess_GenUVCoords                   |
                           aiProcess_FindDegenerates               |
                           aiProcess_FindInvalidData               |
                           aiProcess_ValidateDataStructure         |  /* --> not needed  ? */
                           0);

    timeAssimp.stop();
    if (scene) {
        std::cerr << "--------------- ASIMP STATISTICS ---------------" << std::endl;
        std::cerr << "Assimp Loading time : " << timeAssimp.value() << std::endl;
        Assimp_Info(scene, &importer);
        std::cerr << "------------------------------------------------" << std::endl;

        std::cerr << "Converting scene to internal format ..." << std::endl;
        Timer timeConverter;
        timeConverter.start();
        buildInternalRepresentation(filename, scene);
        timeConverter.stop();
        std::cerr << "Conversion time : " << timeConverter.value() << std::endl;

        mLastErrorNum = 0;
        mLastErrorString = "No Error";
    } else {
        mLastErrorNum = 1;
        mLastErrorString = importer.GetErrorString();
    }
    return !mLastErrorNum;
}

const char * AssimpLoader::getLastErrorString() const
{
    return mLastErrorString.c_str();
}

int AssimpLoader::lastError() const
{
    return mLastErrorNum;
}

Mesh *buildMesh(aiMesh *inputMesh, const std::string &meshName, AssetManager *resources)
{

    if (!inputMesh->HasFaces()) {
        std::cerr << "no face found in " << meshName << std::endl;
        /* Create an empty mesh */

        return NULL;
    }

    int numVertices = inputMesh->mNumVertices;
    Mesh::VertexData *vertices = new Mesh::VertexData[numVertices];

    int numIndices = inputMesh->mNumFaces * 3;
    int *indices   = new int[numIndices];

    for (unsigned int i = 0; i < inputMesh->mNumVertices; ++i) {
        vertices[i].mVertex = glm::vec3(inputMesh->mVertices[i][0],
                                        inputMesh->mVertices[i][1],
                                        inputMesh->mVertices[i][2]);
        if (inputMesh->HasNormals())
            vertices[i].mNormal = glm::vec3(inputMesh->mNormals[i][0],
                                            inputMesh->mNormals[i][1],
                                            inputMesh->mNormals[i][2]);

        if (inputMesh->HasTextureCoords(0))
            vertices[i].mTexCoord = glm::vec4(inputMesh->mTextureCoords[0][i][0],
                                              inputMesh->mTextureCoords[0][i][1],
                                              inputMesh->mTextureCoords[0][i][2],
                                              1.f);
        if (inputMesh->HasTangentsAndBitangents())
            vertices[i].mTangent = glm::vec3(inputMesh->mTangents[i][0],
                                             inputMesh->mTangents[i][1],
                                             inputMesh->mTangents[i][2]);

    }

    for (unsigned int i = 0; i < inputMesh->mNumFaces; ++i) {
        aiFace f = inputMesh->mFaces[i];
        // TODO : corriger ce BEURK !
        assert( (f.mNumIndices == 3) || (f.mNumIndices == 2 ) || (f.mNumIndices == 1 ) );
        if (f.mNumIndices == 3) {
            unsigned int *idx = f.mIndices;
            indices[3*i+0] = idx[0];
            indices[3*i+1] = idx[1];
            indices[3*i+2] = idx[2];
        } else if (f.mNumIndices == 2) {
            unsigned int *idx = f.mIndices;
            indices[3*i+0] = idx[0];
            indices[3*i+1] = idx[1];
            indices[3*i+2] = idx[1];
        }  else if (f.mNumIndices == 1) {
            unsigned int *idx = f.mIndices;
            indices[3*i+0] = idx[0];
            indices[3*i+1] = idx[0];
            indices[3*i+2] = idx[0];
        }
    }

    //@todo verify for animation
    // Alexandre
    if(inputMesh->HasBones()) {

        int numBones = inputMesh->mNumBones;
        AnimatedMesh::BoneData *bones = new AnimatedMesh::BoneData[numBones];

        // Array of bones
        for (unsigned int i = 0; i < inputMesh->mNumBones; ++i) {
            // Name of bone
            bones[i].mName = std::string(inputMesh->mBones[i]->mName.data);

            aiMatrix4x4 aiMatrix = inputMesh->mBones[i]->mOffsetMatrix;
            glm::mat4x4 offset(
                aiMatrix[0][0], aiMatrix[1][0], aiMatrix[2][0], aiMatrix[3][0],
                aiMatrix[0][1], aiMatrix[1][1], aiMatrix[2][1], aiMatrix[3][1],
                aiMatrix[0][2], aiMatrix[1][2], aiMatrix[2][2], aiMatrix[3][2],
                aiMatrix[0][3], aiMatrix[1][3], aiMatrix[2][3], aiMatrix[3][3]
            );

            bones[i].mOffsetMatrix = offset;

            // Array of weights
            bones[i].mNumWeights = inputMesh->mBones[i]->mNumWeights;
            bones[i].mWeights = new AnimatedMesh::WeightData[bones[i].mNumWeights];

            for (unsigned int j = 0; j < bones[i].mNumWeights; ++j) {
                bones[i].mWeights[j].mVertexId = inputMesh->mBones[i]->mWeights[j].mVertexId;
                bones[i].mWeights[j].mWeight = inputMesh->mBones[i]->mWeights[j].mWeight;
            }
        }

        AnimatedMesh *newMesh = new AnimatedMesh(meshName, vertices, numVertices, indices, numIndices, bones, numBones, resources);
        newMesh->init();

        delete [] vertices;
        delete [] indices;
        delete [] bones;

        // Add mesh ptr to the manager
        resources->addAnimatedMesh(newMesh);
        resources->addMesh(newMesh);
        return newMesh;
    }  else {
        Mesh *newMesh = new Mesh(meshName, vertices, numVertices, indices, numIndices);
        newMesh->init();

        delete [] vertices;
        delete [] indices;

        resources->addMesh(newMesh);
        return newMesh;
    }
}

void  AssimpLoader::fillLeafNode(aiNode *currentInputNode, SceneGraph::LeafMeshNode * currentOutputNode, const aiScene *inputScene)
{
//     assimp matrices are row major --> transpose, we store column major !
    aiMatrix4x4 aiMatrix = currentInputNode->mTransformation;
    glm::mat4x4 matrix(
        aiMatrix[0][0], aiMatrix[1][0], aiMatrix[2][0], aiMatrix[3][0],
        aiMatrix[0][1], aiMatrix[1][1], aiMatrix[2][1], aiMatrix[3][1],
        aiMatrix[0][2], aiMatrix[1][2], aiMatrix[2][2], aiMatrix[3][2],
        aiMatrix[0][3], aiMatrix[1][3], aiMatrix[2][3], aiMatrix[3][3]
    );
    currentOutputNode->setTransformMatrix(matrix);

    if (currentInputNode->mNumMeshes > 0) {
        currentOutputNode->initMeshes(currentInputNode->mNumMeshes);
        if (currentInputNode->mNumMeshes == 1) {
            std::string meshName = currentInputNode->mName.data;
            aiMesh *inputMesh = inputScene->mMeshes[currentInputNode->mMeshes[0]];
            (*currentOutputNode)[0] = buildMesh(inputMesh, meshName, mResources);
            Material *meshMaterial = mResources->getMaterial(inputMesh->mMaterialIndex);
            currentOutputNode->getRenderState(0)->setMaterial(meshMaterial);

        } else {
            for (unsigned int i = 0; i < currentInputNode->mNumMeshes; ++i) {
                std::stringstream str;
                str << currentInputNode->mName.data << "_submesh_" << i;
                std::string meshName(str.str());
                aiMesh *inputMesh = inputScene->mMeshes[currentInputNode->mMeshes[i]];
                (*currentOutputNode)[i] = buildMesh(inputMesh, meshName, mResources);
                Material *meshMaterial = mResources->getMaterial(inputMesh->mMaterialIndex);
                currentOutputNode->getRenderState(i)->setMaterial(meshMaterial);
            }
        }
    }
}

void AssimpLoader::fillInnerNode(aiNode *currentInputNode, SceneGraph::InnerNode * currentOutputNode, const aiScene *inputScene)
{
//     assimp matrices are row major --> transpose, we store column major !
    aiMatrix4x4 aiMatrix = currentInputNode->mTransformation;
    glm::mat4x4 matrix(
        aiMatrix[0][0], aiMatrix[1][0], aiMatrix[2][0], aiMatrix[3][0],
        aiMatrix[0][1], aiMatrix[1][1], aiMatrix[2][1], aiMatrix[3][1],
        aiMatrix[0][2], aiMatrix[1][2], aiMatrix[2][2], aiMatrix[3][2],
        aiMatrix[0][3], aiMatrix[1][3], aiMatrix[2][3], aiMatrix[3][3]
    );

    currentOutputNode->setTransformMatrix(matrix);

    currentOutputNode->initChilds(currentInputNode->mNumChildren);

    for (unsigned int i = 0; i < currentInputNode->mNumChildren; ++i) {
        std::string currentName(currentInputNode->mChildren[i]->mName.data);

        if (currentInputNode->mChildren[i]->mNumChildren) {
//             add innerNode
            SceneGraph::InnerNode *newNode = new SceneGraph::InnerNode(currentName, currentOutputNode);
            (*currentOutputNode)[i] = newNode;
            fillInnerNode(currentInputNode->mChildren[i], newNode, inputScene);
        } else {
//             add leaf node

                SceneGraph::LeafMeshNode *newNode = new SceneGraph::LeafMeshNode(currentName, currentOutputNode);
                (*currentOutputNode)[i] = newNode;
                fillLeafNode(currentInputNode->mChildren[i], newNode, inputScene);

        }
    }
}

void AssimpLoader::buildInternalRepresentation(const std::string &name, const aiScene* scene)
{

    // try simple singleton *mResources = AssetManager();
//     send expected value to asset manager -> resize vectors
    mResources->expectedValues(scene->mNumTextures, scene->mNumMaterials);

    size_t found;
    found = name.find_last_of("/\\");
    mResources->setTextureFolder(name.substr(0, found + 1));

//     construit le graphe interne et charge les différentes données

//     parse materials
    //@TODO CHECK
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial *tmpMat = scene->mMaterials[i];
        aiString name;
        tmpMat->Get(AI_MATKEY_NAME, name);
        std::string materialName = name.data;//std::string(name.data);

        int matIndex = mResources->addMaterial(materialName);
        aiColor3D c(0.f, 0.f, 0.f);
        float f;
        Material *mat = mResources->getMaterial(matIndex);

//         material -- fill all avalable properties
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_COLOR_DIFFUSE, c)) {
            const float diffuseFactor = 1.f;
            mat->setDiffuseColor(diffuseFactor * glm::vec3(c.r, c.g, c.b));
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_COLOR_SPECULAR, c)) {
            float strength = 1.0;
            float shine=1.0;
            tmpMat->Get(AI_MATKEY_SHININESS, shine);
            const float specFactor = 1.0;//(shine+8.0)/(8.0*M_PI);
            //tmpMat->Get(AI_MATKEY_SHININESS_STRENGTH, strength);
            //mat->setSpecularColor(specFactor * glm::vec3(strength * c.r, strength * c.g, strength * c.b));
            mat->setSpecularColor(specFactor * glm::vec3(c.r, c.g, c.b));
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_COLOR_AMBIENT, c)) {
            mat->setAmbientColor(glm::vec3(c.r, c.g, c.b));
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_SHININESS, f)) {
            mat->setShininess(f);
        }

//         textures
        aiString textureName;
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_DIFFUSE);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), textureName)) {
            // cf ColladaLoader.cpp l1284
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_AMBIENT);
        }
       if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_SPECULAR);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_EMISSIVE);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), textureName)) {
            std::cerr << "Got an aiTextureType_HEIGHT " <<  textureName.data << std::endl;
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_HEIGHT);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_NORMALS);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_SHININESS, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_SHININESS);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_OPACITY, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_OPACITY);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_DISPLACEMENT, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_DISPLACEMENT);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_LIGHTMAP);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_REFLECTION, 0), textureName)) {
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_REFLECTION);
        }
        if (AI_SUCCESS == tmpMat->Get(AI_MATKEY_TEXTURE(aiTextureType_UNKNOWN, 0), textureName)) {
            std::cerr << "Unknown texture : " << textureName.data << std::endl;
            int texIndex = mResources->addTexture(textureName.data);
            mat->addTexture(mResources->getTexture(texIndex), Material::TEXTURE_UNKNOWN);
        }
    }

//     parse aiScene graph and select only meshes
    aiNode * sceneNode = scene->mRootNode;
    SceneGraph::Node::NodePtr rootNode = NULL;
    if (sceneNode->mNumChildren) {
        std::string rootName(sceneNode->mName.data);
        rootNode = new SceneGraph::InnerNode(rootName);
        fillInnerNode(sceneNode, static_cast<SceneGraph::InnerNode *>(rootNode), scene);
    } else {
        rootNode = new SceneGraph::LeafMeshNode(name);
        fillLeafNode(sceneNode, static_cast<SceneGraph::LeafMeshNode *>(rootNode), scene);
    }

    delete *mSceneGraph;
    *mSceneGraph = new SceneGraph(rootNode);

    // Parse aiScene graph and select animations

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {

        // Create animation
        Animation *anim = new Animation(scene->mAnimations[i]);

        // Create & Attach skeleton to anim
        SkeletonGraph *skeleton = buildInternalSkeleton(anim);
        if (skeleton) {
            anim->setSkeleton(skeleton);
            // Add animation
            mResources->addAnimation(anim);
        } else {
            delete anim;
        }
    }

    // TODO if thereis no animation data but animated meshese in the scene, extract only skeleton

    // setup lights
    for (unsigned int i = 0; i < scene->mNumLights; ++i) {
        std::cerr << "Light Source " << scene->mLights[i]->mName.data << std::endl;
        (*mSceneGraph)->mLights.push_back(Light(scene->mLights[i]));
    }
    LightTransformVisitor transformLightAction(*mSceneGraph);
    SceneGraph::PreOrderVisitor transformLight(*mSceneGraph, transformLightAction);
    transformLight.go(glm::mat4x4(1.0), glm::mat4x4(1.0));

    //setup cameras
    for (unsigned int i = 0; i < scene->mNumCameras; ++i) {
        std::cerr << "Camera  " << scene->mCameras[i]->mName.data << std::endl;
        // todo : compute the correct informations for loaded camera
//        (*mSceneGraph)->mCameras.push_back(Camera(scene->mCameras[i]));
    }
#ifdef PRINTGRAPHS
    // Display internal graph
    std::cerr << "---------- Internal final Graph -----------------" << std::endl;
    PrintGraph visitor(*mSceneGraph);
    visitor.go();
    std::cerr << "---------- Internal final Graph -----------------" << std::endl;
#endif
}

SkeletonGraph* AssimpLoader::buildInternalSkeleton(Animation *anim) {

    // Find the "root"
    // but no the best way...
    SceneGraph::Node::NodePtr skeletonSceneRoot;
    std::cerr << "searching root from " << anim->getChannel(0)->mBoneName << std::endl;
    findSkeletonRoot(anim->getChannel(0)->mBoneName, anim, (*mSceneGraph)->getRootNode(), &skeletonSceneRoot);
    std::cerr << "Found root : " << skeletonSceneRoot->name() << std::endl;
    skeletonSceneRoot->setAcceptVisitors(false); // for skipping visiting the skeleton while rendering

    if (mResources->nAnimatedMesh()) {
        // map of skeleton nodes, initialized to false
        std::map<std::string, bool> usefullNode;
        std::cerr << "Animated Meshes : (" << mResources->nAnimatedMesh() << ")" << std::endl;
        for (unsigned int i = 0; i < mResources->nAnimatedMesh(); ++i) {
            AnimatedMesh *aMesh = mResources->getAnimatedMesh(i);
            std::cerr << "Mesh " << aMesh->name() << " (" << aMesh->nBones() << " bones )" << std::endl;
            for (unsigned int j = 0; j < aMesh->nBones(); ++j) {
                //std::cerr << " Bone " << j << " --- " << aMesh->getBone(j)->mName.data() << std::endl;
                usefullNode[std::string(aMesh->getBone(j)->mName.data())] = false;
            }
        }

        // Update the map struct
        selectUsefullNode(&usefullNode, skeletonSceneRoot);

        // Build the skeleton
        SkeletonGraph::Node *skeletonRoot = createSkeletonFromScene(&usefullNode, skeletonSceneRoot, NULL);
        assert(skeletonRoot);
        SkeletonGraph *skeleton = new SkeletonGraph(skeletonRoot);
        skeleton->buildBonesMap(skeleton->getRoot()); // Build the map with bones references

#ifdef PRINTGRAPHS
        //DEBUG
        std::cerr << "------------------- Skeleton -------------------------" << std::endl;
        skeleton->printSkeleton(skeleton->getRoot());
        std::cerr << "------------------- End skeleton ---------------------" << std::endl;
#endif
        return skeleton;
    } else
        return NULL;
}

void AssimpLoader::selectUsefullNode(std::map<std::string, bool> *bonesMap, SceneGraph::Node *node) {
    std::map<std::string, bool>::iterator it = bonesMap->find(node->name());
    if(it != bonesMap->end()) {
        it->second = true;

        SceneGraph::Node *parent = node->parent();
        while(parent != NULL) {
            (*bonesMap)[parent->name()] = true;

            parent = parent->parent();
        }
    }

    if(!node->isLeaf()) {
        SceneGraph::InnerNode* innerNode = static_cast<SceneGraph::InnerNode*>(node);

        for (int i = 0; i < innerNode->nChilds(); ++i) {
            // Recursive traitement on childs
            // and add in internal childs array
            selectUsefullNode(bonesMap, (*innerNode)[i]);
        }
    }
}

/**
 * Build the skeleton of an animation (simply)
 */
SkeletonGraph::Node* AssimpLoader::createSkeletonFromScene(std::map<std::string, bool> *bonesMap, SceneGraph::Node* sceneNode, SkeletonGraph::Node* parent) {

    std::map<std::string, bool>::iterator it = bonesMap->find(sceneNode->name());

    if(it != bonesMap->end()) {
        SkeletonGraph::Node* internalNode = new SkeletonGraph::Node(sceneNode->name(), parent);

        // Copy the local transform from scene graph
        internalNode->setBindPoseTransform(sceneNode->transformMatrix());


        //TODO do something more clean
        for (unsigned int i = 0; i < mResources->nAnimatedMesh(); ++i) {
            AnimatedMesh *aMesh = mResources->getAnimatedMesh(i);
            for (unsigned int j = 0; j < aMesh->nBones(); ++j) {
                if(aMesh->getBone(j)->mName == internalNode->name()) {
                    internalNode->setOffsetTransform(aMesh->getBone(j)->mOffsetMatrix);
                }
            }
        }

        // Recusive
        if(!sceneNode->isLeaf()) {
            SceneGraph::InnerNode* innerNode = static_cast<SceneGraph::InnerNode*>(sceneNode);

            for (int i = 0; i < innerNode->nChilds(); ++i) {
                // Recursive traitement on childs
                // and add in internal childs array
                SkeletonGraph::Node *child = createSkeletonFromScene(bonesMap, (*innerNode)[i], internalNode);
                if(child != NULL)
                    internalNode->addChild(child);
            }
        }

        return internalNode;
    } else {
        return NULL;
    }
}

//TODO
// A revoir....
void AssimpLoader::findSkeletonRoot(const std::string &name, Animation *anim, SceneGraph::Node* node, SceneGraph::Node::NodePtr* root) {

    // Bone found
    if(node->name() == name) {
        // Research of skeleton root
        //! On Suppose que les squelletes ne sont pas des fils des maillages
        SceneGraph::Node* nodeFrom = NULL;
        while(node->parent() != NULL) {
            nodeFrom = node;
//            if(anim->getAnimByBoneName(node->name()) == NULL) {
//                // If no bone with this name we are in da root...
//                break;
//            }

            node = node->parent();
        }

        // root found
        *root = nodeFrom;

        return;
    }

    // Bone not found, recusive research
    if(!node->isLeaf()) {
        SceneGraph::InnerNode* innerNode = static_cast<SceneGraph::InnerNode*>(node);

        for (int i = 0; i < innerNode->nChilds(); ++i) {
            findSkeletonRoot(name, anim, (*innerNode)[i], root);
        }
    }
}

}
