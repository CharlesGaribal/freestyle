/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef DIRECTIONSAMPLER_H
#define DIRECTIONSAMPLER_H

/* This class sample the direction space based on a geodesic sphere */
#include <vector>
#include "opengl.h"
#include "texture.h"

namespace vortex {

class DirectionSampler
{
public:
    struct Edge {
        int splitIndex;
        glm::vec3 splitPosition;
        Edge() : splitIndex(-1), splitPosition(0.f) {
        }
    };

    class Face {
    public:
        Face (int i0, int i1, int i2) {
                indexes_[0] = i0;
                indexes_[1] = i1;
                indexes_[2] = i2;
        };
        Face (const int* pIds) {
                indexes_[0] = pIds[0];
                indexes_[1] = pIds[1];
                indexes_[2] = pIds[2];
        };
        unsigned int indexes_[3];
        inline unsigned int& operator[] (int k) {
                assert (k<3);
                return indexes_[k];
        }
        inline unsigned int operator[] (int k) const {
                assert (k<3);
                return indexes_[k];
        }
    };

    typedef std::vector<glm::vec3> DirectionSet;
    typedef std::vector<Face> FaceSet;

    DirectionSet directions_;
    int nbDirections_;

    FaceSet faces_;
    int nbFaces_;

    DirectionSampler(int level=0);

    void compileGL();
    void drawGL();

    Texture *getAsTexture(){return mTexture_;}

    int getNbSamples(){return nbDirections_;}//return (int)(pow(2, 2*dirsamplerres_)*10 +2);}

private:
    int getIndex(Edge** tab, const glm::vec3 &vert, int indV1, int indV2);

    // OpenGL stuffs
    GLuint mVertexArrayObject;
    enum {VBO_VERTICES, VBO_INDICES};
    GLuint mVertexBufferObjects[2];

    Texture *mTexture_;

};

}
#endif // DIRECTIONSAMPLER_H
