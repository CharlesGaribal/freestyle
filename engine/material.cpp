/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include <iostream>

#include "material.h"

namespace vortex {

std::string Material::TextureSemanticStrings[] = {"TEXTURE_AMBIENT", "TEXTURE_DIFFUSE", "TEXTURE_SPECULAR", "TEXTURE_EMISSIVE", "TEXTURE_HEIGHT", "TEXTURE_NORMALS",
        "TEXTURE_SHININESS", "TEXTURE_OPACITY", "TEXTURE_DISPLACEMENT", "TEXTURE_LIGHTMAP", "TEXTURE_REFLECTION", "TEXTURE_UNKNOWN"
                                                 };





Material::Material(std::string name) :
    mName(name), mNumTextures(0), mMaterialId(-1)
{
}

Texture *Material::getTexture(TextureSemantic semantic)
{
    std::map<TextureSemantic, Texture *>::iterator it = mTextures.find(semantic);
    if (it != mTextures.end()) {
        return it->second;
    } else
        return NULL;
}
int Material::materialId() const
{
    return mMaterialId;
}

void Material::setMaterialId(int materialId)
{
    mMaterialId = materialId;
}


}
