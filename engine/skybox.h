/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef __SKYBOX_H__
#define __SKYBOX_H__
#include <string>

/// @todo remove QT dependencies here
#include <QImage>

#include "texture.h"
#include "mesh.h"
#include "pfmreader.h"

namespace vortex{
class SkyBox{
    float *mData[6];
public:
    SkyBox();

    ~SkyBox();

    // type : 0 --> pfm, 1 --> 6 negx to posz
    void setupTextures(std::string name, int type);

    void draw();

    bool valid;

    Texture *mTex;
    Mesh *mCube;
    float coeffs[9][3] ;                /* Spherical harmonic coefficients */

    glm::mat4 mShMatrices[3];
    int mWidth;
    int mHeight;

    void tomatrix(void);

    void updateCoeffs(float *hdr, float x, float y, float z, float domega);

    float *getPixel(float x, float y, float z);

    // http://graphics.stanford.edu/papers/envmap/prefilter.c
    void computeSHMatrices();
};

}
#endif
