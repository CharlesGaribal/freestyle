/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include <iostream>
#include <sstream>

#include "assetmanager.h"

namespace vortex {

AssetManager::AssetManager() : mDefaultShaderProgram(NULL),
    mDefaultTexture(NULL),
    mDefaultMaterial(NULL),
    mNumAnimations(0),
    mNumAnimatedMeshs(0)
{
    mDefaultMaterial = new Material("default");
    mVertFileExt = std::string(".vert");
    mFragFileExt = std::string(".frag");
}

AssetManager::~AssetManager()
{
    for (unsigned int i = 0; i < mTextures.size(); ++i) {
        mTextures[i]->deleteGL();
    }
    for (unsigned int i = 0; i < mMaterials.size(); ++i) {
        delete mMaterials[i];
    }
    delete mDefaultMaterial;
    for (unsigned int i=0; i<mShaderProgram.size(); ++i){
        mShaderProgram[i]->deleteProgram();
    }
    delete mDefaultShaderProgram;
}


/**
 * @todo : ensure that image is just loaded once
 */
int AssetManager::addTexture(std::string fileName)
{
    /// @todo use std::pair<iterator, bool> map.insert(...) instead of find + =
    std::map<std::string, int>::iterator it = mTextureMap.find(fileName);
    if (it != mTextureMap.end()) {
        return it->second;
    } else {


        ///@todo : replace all \ by /
        std::size_t found = fileName.find("\\");
        if (found < std::string::npos)
            fileName.replace (found,  1,  "/");

        std::string textureFile = mFolder + fileName;
        Texture * theTexture = Texture::loadFromImage(textureFile);

        mTextures.push_back(theTexture);
        int pos = mTextures.size() - 1;
        mTextureMap[fileName] = pos;
        return pos;
    }
}

int AssetManager::addShaderProgram(ShaderProgram *program)
{
    mShaderProgram.push_back(program);
    return mShaderProgram.size() - 1;
}

unsigned int AssetManager::numMeshs() const
{
    return mNumMeshs;
}

void AssetManager::addMesh(Mesh::MeshPtr themesh){
    themesh->setMeshId(mMeshs.size());
    mMeshs.push_back(themesh);
}

Mesh::MeshPtr AssetManager::getMesh(unsigned int meshId) {
    return mMeshs[meshId];
}

// Non optimal !
int AssetManager::addShaderProgram(std::string fileName){
    std::string shaderFile = mShaderBasePath + fileName;
    ShaderConfiguration programConfiguration(ShaderConfiguration::DEFAULT, shaderFile);
    ShaderProgram *theProgram = getShaderProgram(programConfiguration);
    if (theProgram) {
        std::map<ShaderConfiguration , int>::iterator it = mConfiguredPrograms.find(programConfiguration);
        return it->second;
    }
    else
        return -1;
}

int AssetManager::addMaterial(std::string name)
{
    Material *theMat = new Material(name);
    theMat->setMaterialId(mMaterials.size());
    mMaterials.push_back(theMat);
    return theMat->materialId();
}

Material *AssetManager::getMaterial(unsigned int index) const
{
    assert(index < mMaterials.size());
    return mMaterials[index];
}

ShaderProgram *AssetManager::getShaderProgram(int index) const
{
    if (index < 0) {
        return mDefaultShaderProgram;
    } else {
        return mShaderProgram[index];
    }
}

ShaderProgram *AssetManager::getShaderProgram(const ShaderConfiguration &configuration)
{

    /// @todo use std::pair<iterator, bool> map.insert(...) instead of find + =
    std::map<ShaderConfiguration, int>::iterator it = mConfiguredPrograms.find(configuration);

    if (it == mConfiguredPrograms.end()) {
        // Shader non existant, l'ajouter
        std::stringstream defineString;
        for (int i = 0; i < configuration.numProperties(); ++i) {
            defineString << "#define " << configuration.getPropertyName(i) << "\n";
        }

        for (int i = 0; i < configuration.numDefaultProperties(); ++i) {
            defineString << "#define " << configuration.getDefaultPropertyName(i) << "\n";
        }
        ShaderProgram *theProgram = new ShaderProgram();
        theProgram->setConfiguration(configuration);
        theProgram->create();

        int shadersOK = 0;
        std::string shaderFile =  configuration.getProgramName();
        if (configuration.getType() & ShaderConfiguration::VERTEX_SHADER) {
            ShaderObject *vs = new ShaderObject;
            vs->create(GL_VERTEX_SHADER);
            vs->configure(defineString.str());
            vs->load(shaderFile + mVertFileExt);
            vs->compile();
            shadersOK = vs->check();
            theProgram->add(vs);
        }
        if (configuration.getType() & ShaderConfiguration::FRAGMENT_SHADER) {
            ShaderObject *fs = new ShaderObject;
            fs->create(GL_FRAGMENT_SHADER);
            fs->configure(defineString.str());
            fs->load(shaderFile + mFragFileExt);
            fs->compile();
            shadersOK = shadersOK && fs->check();
            theProgram->add(fs);
        }

        if (configuration.getType() & ShaderConfiguration::GEOMETRY_SHADER) {
            ShaderObject *gs = new ShaderObject;
            gs->create(GL_GEOMETRY_SHADER);
            gs->configure(defineString.str());
            gs->load(shaderFile + ".glsl"); // use this extension to have syntaxic coloration and differentiate shaders
            gs->compile();
            shadersOK = shadersOK && gs->check();
            theProgram->add(gs);
        }

        theProgram->link();
        shadersOK = shadersOK && theProgram->check();
        if (shadersOK) {
            //           std::cerr << "Adding Programm ..." << std::endl;
            //            std::cerr << "AssetManager::getShaderProgram AJOUT de " << configuration << std::endl;
            int numShader = addShaderProgram(theProgram);
            mConfiguredPrograms[configuration] = numShader;
            return theProgram;
        } else {
            delete theProgram;
            std::cerr << "Return Default Program" << std::endl;
            return getShaderProgram(-1);
        }
    } else {
        // shader existant, l'utiliser
        //         std::cerr << "EXISTANT" << std::endl;
        //        std::cerr << "AssetManager::getShaderProgram RECUP de " << configuration << std::endl;
        return getShaderProgram(it->second);
    }
}

ShaderConfiguration AssetManager::getShaderConfiguration(ShaderProgram *program)
{
    for (std::map<ShaderConfiguration , int>::iterator it = mConfiguredPrograms.begin(); it != mConfiguredPrograms.end(); ++it) {
        if (mShaderProgram[it->second] == program) {
            return it->first;
        }
    }
    // TODO add invalid shader conf
    assert(false);
    return ShaderConfiguration();
}


Texture *AssetManager::getTexture(int index) const
{
    if (index < 0) {
        return mDefaultTexture;
    } else {
        return mTextures[index];
    }
}

void AssetManager::reloadShaders()
{
    for (unsigned int i = 0; i < mShaderProgram.size(); ++i) {
        std::cerr << "reload shader " << i  << std::endl;
        mShaderProgram[i]->reload();
    }
}

void AssetManager::statistics(){
    std::cerr << "Total number of programs :          \t" << mShaderProgram.size() << std::endl;
    std::cerr << "Number of programs configurations : \t" << mConfiguredPrograms.size() << std::endl;
    std::cerr << "Total number of textures :          \t" << mTextures.size() << std::endl;
    std::cerr << "Number of unique textures :         \t" << mTextureMap.size() << std::endl;
    std::cerr << "Number of materials :               \t" << mMaterials.size() << std::endl;
}

/**
 * @author Alexandre Bonhomme
 * @date 24/04/12
 */
void AssetManager::addAnimation(Animation *anim) {
    mAnimations.push_back(anim);
    // post incrementation voluntary
    mCurrentAnimation = mNumAnimations++;
}

Animation* AssetManager::getAnimation(unsigned int index) const {
    if(index >= mAnimations.size()) {
        return mAnimations[0];
    } else {
        return mAnimations[index];
    }
}

void AssetManager::addAnimatedMesh(AnimatedMesh::AnimatedMeshPtr mesh) {
    mAnimatedMeshs.push_back(mesh);
    ++mNumAnimatedMeshs;
}

bool AssetManager::updateAnimations(double time) {
    bool modified=(mNumAnimations!=0);
    for (unsigned int i = 0; i < mNumAnimations; ++i) {
        mAnimations[i]->update(time);
    }
    return modified;
}

double AssetManager::setAnimationsAt(int percent){
    if (mNumAnimations) {
        ///@todo : generalize animation control : only anim 0 is trigerred here
        double time = mAnimations[0]->duration()*(double)percent/100;
        mAnimations[0]->update(time);
        return time;
    } else
        return 0.;
}
} // namespace vortex
