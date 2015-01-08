/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <vector>
#include <string>
#include <map>

#include "shaderobject.h"
#include "texture.h"
#include "material.h"

#include "animatedmesh.h"
#include "animation.h"


namespace vortex {

/* classe de gestion des ressources du moteur :
- Maillages
- Textures
- Materiaux
- Shaders
- ...
*/

/**
 * Resources management class
 *
 * This class manages scene resources : Textures, Materials, ShaderPrograms, Lights ...
 *
 */


/** @todo : make it a singleton !! */
class AssetManager {
public :
    /**
         * Constructor : initialize all resources and initilise image loading library
         *
         */
    AssetManager();

    /**
         * Destructor : responsible for all Textures OpenGL deletion
         *
         */

    ~AssetManager();

    /**
         * Indicate the minimal number of textures and materials to be reserved for the scene
         *
         * @param numTextures Minimal number of Textures to be reserved
         * @param numMaterials Minimal number of Materials to be reserved
         *
         */
    void expectedValues(int numTextures, int numMaterials) {
        if(numTextures>0)
            mTextures.reserve(numTextures);
        if(numMaterials>0)
            mMaterials.reserve(numMaterials);
    }

    /**
         * Set the texture folder
         *
         * @param folder Path for the texture image folder
         *
         */
    void setTextureFolder(std::string folder) {
        mFolder = folder;
    }

    /**
         * Add a new Texture if not already loaded
         *
         * @param fileName Name of the texture image file in the texture folder
         *
         * @return The index of the Texture in Texture storage. This index may be used as "AssetManager::getTexture" parameter to access this Texture
         */
    int addTexture(std::string fileName);

    /**
         * Create a new Material
         *
         * @param name Name of the new Material object
         *
         * @return The index of the Material in Material storage. This index may be used used as "AssetManager::getMaterial" parameter to access this Material
         */
    int addMaterial(std::string name);

    /**
         * @return A pointer to the ShaderProgram using "configuration"
         *
         * @param configuration ShaderConfiguration for ShaderProgram compilation
         *
         * If no such ShaderProgram exists, this method will attempt to create a new Shader Program along with configuration.
         * If any compilation problem occurs during its creation, this method will return a pointer to the default ShaderProgram.
         */
    ShaderProgram *getShaderProgram(const ShaderConfiguration &configuration);

    /**
         * @return The ShaderConfiguration used by "program"
         *
         * If the ShaderProgram had not been created using the getShaderProgram method, this method will return the default ShaderConfiguration
         */
    ShaderConfiguration getShaderConfiguration(ShaderProgram *program);

    /**
         * @return A pointer to the Texture at index "index" in Texture storage
         *
         * If index value is negative, this method will return the default Texture
         */
    Texture *getTexture(int index) const;

    /**
         * @return A pointer to the Material at index "index" in Texture storage
         *
         */
    Material *getMaterial(unsigned int index) const;


    /**
         * Reload all shaders
         *
         * If shader compilation occurs, a log message is written on the error out put
         */
    void reloadShaders();
    /**
      * @return A pointer to the ShaderProgram located at "index" in ShaderProgram storage
      *
      * If index value is negative then a pointer to the default ShaderProgam is returned.
      */
    ShaderProgram *getShaderProgram(int index) const;
    int addShaderProgram(std::string fileName);

    /**
      * Print various statistics about the current asset
      */
    void statistics();

    /**
      * Add an animation
      */
    void addAnimation(Animation *anim);

    /**
      * Get the requested animation
      */
    Animation *getAnimation(unsigned int index) const;


    /**
      * Set the current animation
      * @todo : check index value : ensure 0<=index<NumAnimation
      */
    void setCurrentAnimation(unsigned int index) {
        mCurrentAnimation = index;
    }

    /**
      * Get the current animation
      */
    Animation *getCurrentAnimation() const {
        return mAnimations[mCurrentAnimation];
    }

    /**
     * Update all animations of the scene
     * @param time The current time in second
     * @return true if scene was modified, false otherwise
     */
    bool updateAnimations(double time);


    /**
     * Update all animations of the scene
     * @param percent The position in the animation
     * @return the time corresponding to the position
     */
    double setAnimationsAt(int percent);


    /**
      * get the number of animated meshes
      */
    unsigned int nAnimatedMesh() const { return mNumAnimatedMeshs; }

    /**
      * get the animated meshes at the given index
      */
    AnimatedMesh::AnimatedMeshPtr getAnimatedMesh(unsigned int index) const { return mAnimatedMeshs[index]; }

    /**
      * Add an animated mesh to the asset
      */
    void addAnimatedMesh(AnimatedMesh::AnimatedMeshPtr mesh);

    void setCurrentFrame(unsigned int frame) { mCurrentFrame = frame; }
    unsigned int getCurrentFrame() const { return mCurrentFrame; }


    void setShaderBasePath(const std::string &path) {
        mShaderBasePath = path;
    }

    std::string &getShaderBasePath(void){
        return mShaderBasePath;
    }


    /**
     * @brief setShaderFileExtentions
     * To work properly, vertex and fragment file extention must be different, since vertex shader is taken from name+vertexFileExtention, and fragment from name+fragmentFileExtention
     * @param vertexFileExt new vertex file extention (it is .vert by default)
     * @param fragmentFileExt new fragment file extention  (it is .frag by default)
     */
    void setShaderFileExtentions(std::string vertexFileExt, std::string fragmentFileExt){
        assert(vertexFileExt != fragmentFileExt);
        mFragFileExt = fragmentFileExt;
        mVertFileExt = vertexFileExt;
    }

    unsigned int numMeshs() const;

    void addMesh(Mesh::MeshPtr themesh);
    Mesh::MeshPtr getMesh(unsigned int meshId);
private:

    /**
         * Record a new ShaderProgram
         *
         * @param program Pointer to the ShaderProgram to be recorded
         *
         * @return The index of "program" in ShaderProgram storage. This index may be used used as "AssetManager::getShaderProgram" parameter to access this ShaderProgram
         */
    int addShaderProgram(ShaderProgram *program);

    std::vector<ShaderProgram *> mShaderProgram;
    std::map<ShaderConfiguration , int> mConfiguredPrograms;
    ShaderProgram *mDefaultShaderProgram;

    std::map<std::string, int> mTextureMap;

    std::vector<Texture *> mTextures;
    Texture *mDefaultTexture;

    std::vector<Material *> mMaterials;
    Material *mDefaultMaterial;

    std::string mFolder;
    std::string mFragFileExt;
    std::string mVertFileExt;

    unsigned int mCurrentAnimation;
    unsigned int mNumAnimations;
    std::vector<Animation *> mAnimations;

    unsigned int mCurrentFrame;

    unsigned int mNumAnimatedMeshs;
    std::vector<AnimatedMesh::AnimatedMeshPtr> mAnimatedMeshs;

    unsigned int mNumMeshs;
    std::vector<Mesh::MeshPtr> mMeshs;

    std::string mShaderBasePath;
};

} // namespace vortex
#endif // ASSETMANAGER_H
