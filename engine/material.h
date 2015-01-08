/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "opengl.h"

#include "texture.h"


namespace vortex {

/**
 * Material representation class. Is responsible for storing Texture manner of use along with it.
 *
 */
class Material {
public:
    /**
     * Texture semantic enumeration : Specify the use of a texture
     *
     */
    enum TextureSemantic {TEXTURE_AMBIENT = 0, TEXTURE_DIFFUSE, TEXTURE_SPECULAR, TEXTURE_EMISSIVE, TEXTURE_HEIGHT, TEXTURE_NORMALS, TEXTURE_SHININESS, TEXTURE_OPACITY, TEXTURE_DISPLACEMENT, TEXTURE_LIGHTMAP, TEXTURE_REFLECTION, TEXTURE_UNKNOWN};

    static std::string TextureSemanticStrings[];

    /**
     * Constructor : initialize Material attributes.
     *
     * @param name Name of the Material Object.
     */
    Material(std::string name);

    /**
     * Set the Material diffuse color.
     *
     * @param color New Material diffuse component.
     */
    void setDiffuseColor(const glm::vec3 &color) {
        mDiffuseColor = color;
    }
    glm::vec3 getDiffuseColor() {
        return mDiffuseColor;
    }

    /**
     * Set the Material specular color.
     *
     * @param color New Material specular component.
     */
    void setSpecularColor(const glm::vec3 &color) {
        mSpecularColor = color;
    }
    glm::vec3 getSpecularColor() {
        return mSpecularColor;
    }


    /**
     * Set the Material ambient color.
     *
     * @param color New Material ambient component.
     */
    void setAmbientColor(const glm::vec3 &color) {
        mAmbientColor = color;
    }
    glm::vec3 getAmbientColor() {
        return mAmbientColor;
    }


    /**
     * Add a new Texture to the Material
     *
     * @param texture The Texture to be added.
     * @param semantic Specify how the Texture should be used.
     */
    void addTexture(Texture *texture, TextureSemantic semantic) {        
        mNumTextures += 1;
        mTextures[semantic] = texture;
    }

    /**
     * @return The Texture which use is specified by "semantic"
     *
     * @param semantic The Texture use wanted to be performed.
     */
    Texture *getTexture(TextureSemantic semantic);

    /**
     * @return The number of Textures the Material stores.
     *
     */
    int numTexture() const {
        return mNumTextures;
    }

    /**
     * @return The Texture use, as a string, of the "i"th Texture stored in the Material.
     *
     */
    std::string getTextureSemanticString(int i) {
        std::map<TextureSemantic, Texture *>::iterator it = mTextures.begin();
        while (i--)  ++it;
        return Material::TextureSemanticStrings[it->first];
    }

    /**
     * @return The Texture use of the "i"th Texture stored in the Material.
     *
     */
    TextureSemantic getTextureSemantic(int i) {
        std::map<TextureSemantic, Texture *>::iterator it = mTextures.begin();
        while (i--)  ++it;
        return  it->first;
    }

    /**
     * @return The Material object name.
     */
    std::string getName() {
        return mName;
    }

    float getShininess() {
        return mShininess;
    }
    void setShininess(float shininess) {
        mShininess = shininess;
    }

    void debug() {
        using vortex::util::operator<<;
        std::cerr << "material " << mName << std::endl;
        std::cerr << " Ka " << mAmbientColor << std::endl;
        std::cerr << " Kd " << mDiffuseColor << std::endl;
        std::cerr << " Ks " << mSpecularColor << std::endl;
        std::cerr << " Ns " << mShininess << std::endl;
        for (TextureMap::iterator itr = mTextures.begin(), stop = mTextures.end(); itr != stop; ++itr) {
            std::cerr << "Texture " << itr->first << " " << itr->second->getName() << std::endl;
        }
    }

    int materialId() const;
    void setMaterialId(int materialId);

protected:
    std::string mName;
    glm::vec3 mDiffuseColor;
    glm::vec3 mSpecularColor;
    glm::vec3 mAmbientColor;
    float mShininess;

    /**
    * Associated Textures with this material.
    *
    * The map associate a semantic (diffuse, ambient, normal, ... to the texture.
    * Available semantics are AMBIENT, DIFFUSE, SPECULAR, EMISSIVE, HEIGHT, NORMALS, SHININESS, OPACITY, DISPLACEMENT, LIGHTMAP, REFLECTION, UNKNOWN
    */
    typedef std::map<TextureSemantic, Texture *> TextureMap;
    TextureMap mTextures;
    int mNumTextures;

    int mMaterialId; // relative to the asset : -1 if material not store in the assetmanager
};

class MaterialPropertyFilter {
public :
    MaterialPropertyFilter(){}
    virtual ~MaterialPropertyFilter(){}
    virtual bool match(Material::TextureSemantic)=0;
    virtual void addProperty(Material::TextureSemantic p){}

};

class MaterialAllPropertiesFilter : public MaterialPropertyFilter {
public:
    MaterialAllPropertiesFilter() : MaterialPropertyFilter(){}
    ~MaterialAllPropertiesFilter(){}
    bool match(Material::TextureSemantic) {
        return true;
    }
};

class MaterialPropertiesSelectorFilter : public MaterialPropertyFilter {
public:
    MaterialPropertiesSelectorFilter() : MaterialPropertyFilter(){}
    ~MaterialPropertiesSelectorFilter(){}
    void addProperty(Material::TextureSemantic p){
        mProperties.push_back(p);
    }

    bool match(Material::TextureSemantic p) {
        for (std::vector<Material::TextureSemantic>::const_iterator i=mProperties.begin(); i!=mProperties.end(); ++i)
            if (p == *i)
                return true;
        return false;
    }
private:
    std::vector<Material::TextureSemantic> mProperties;
};


}

#endif // MATERIAL_H
