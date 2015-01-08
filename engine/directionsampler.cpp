/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "directionsampler.h"
#include <iostream>
#include <random>

//#define HALTON_SEQUENCE
#ifdef HALTON_SEQUENCE
#include "halton/halton_sampler.h"
#endif

namespace vortex {

DirectionSampler::DirectionSampler(int level) : mTexture_(NULL)
{

    float nombreDOr = (1+sqrt(5))/2;
    float scale = sqrt( 1.f / (1.f + (nombreDOr*nombreDOr) ) );
    float a=scale * nombreDOr;
    float b=scale;

    directions_.push_back(glm::vec3(a, 0, b));
    directions_.push_back(glm::vec3(a, 0, -b));
    directions_.push_back(glm::vec3(-a, 0, b));
    directions_.push_back(glm::vec3(-a, 0,-b));
    directions_.push_back(glm::vec3(b, a, 0));
    directions_.push_back(glm::vec3(-b, a, 0));
    directions_.push_back(glm::vec3(b, -a, 0));
    directions_.push_back(glm::vec3(-b, -a, 0));
    directions_.push_back(glm::vec3(0, b, a));
    directions_.push_back(glm::vec3(0, -b, a));
    directions_.push_back(glm::vec3(0, b, -a));
    directions_.push_back(glm::vec3(0, -b, -a));

    faces_.push_back( Face(0,1,4) );
    faces_.push_back( Face(0,6,1) );
    faces_.push_back( Face(0,4,8) );
    faces_.push_back( Face(0,8,9) );
    faces_.push_back( Face(0,9,6) );//5
    faces_.push_back( Face(1,10,4) );
    faces_.push_back( Face(1,11,10) );
    faces_.push_back( Face(1,6,11) );
    faces_.push_back( Face(2,5,3) );
    faces_.push_back( Face(2,3,7) );//10
    faces_.push_back( Face(2,7,9) );
    faces_.push_back( Face(2,9,8) );
    faces_.push_back( Face(2,8,5) );
    faces_.push_back( Face(3,5,10) );
    faces_.push_back( Face(3,10,11) );//15
    faces_.push_back( Face(3,11,7) );
    faces_.push_back( Face(4,10,5) );
    faces_.push_back( Face(4,5,8) );
    faces_.push_back( Face(6,9,7) );
    faces_.push_back( Face(6,7,11) );//20

    nbDirections_  = directions_. size();
    nbFaces_ = faces_.size();

    for (int numSubdiv=0; numSubdiv < level; ++numSubdiv){

        int nbDir = nbDirections_;
        Edge **edgeTab = new Edge*[nbDir];
        for(int i = 0; i < nbDir; ++i){
            edgeTab[i] = new Edge[6];
        }

        std::vector<Face> tmpFaces;

        for(int indTgl = 0; indTgl < nbFaces_; ++indTgl){
            int indV1, indV2, indV3;

            indV1 = faces_[indTgl][0];
            indV2 = faces_[indTgl][1];
            indV3 = faces_[indTgl][2];

            glm::vec3 v1, v2, v3;

            v1 = directions_[indV1];
            v2 = directions_[indV2];
            v3 = directions_[indV3];

            glm::vec3 v12, v23, v31;

            v12 = glm::normalize((v1+v2)/2.f);
            v23 = glm::normalize((v2+v3)/2.f);
            v31 = glm::normalize((v3+v1)/2.f);

            int indV12 = getIndex(edgeTab, v12, indV1, indV2);
            int indV23 = getIndex(edgeTab, v23, indV2, indV3);
            int indV31 = getIndex(edgeTab, v31, indV3, indV1);

            tmpFaces.push_back(Face(indV1, indV12, indV31));
            tmpFaces.push_back(Face(indV2, indV23, indV12));
            tmpFaces.push_back(Face(indV3, indV31, indV23));
            tmpFaces.push_back(Face(indV12, indV23, indV31));

        }

        for(int i=0; i<nbDir; i++){
            delete [] edgeTab[i];
        }
        delete [] edgeTab;

        faces_ = tmpFaces;
        nbFaces_= faces_.size();


    }
    // (2^(2*level) * 10 + 2 points
    std::cerr << "Points : " << nbDirections_ << " (" << pow(2, 2*level)*10 +2 << ")" << std::endl;

}

#define EPSILON 1e-6f
int DirectionSampler::getIndex(Edge** edgetab, const glm::vec3 &vert, int indV1, int indV2){
        int index =-1;
        int i;

        for(i = 0; i<6; i++){
            if( (edgetab[indV1][i].splitIndex != -1) &&  (edgetab[indV1][i].splitPosition == vert) ){
                index = edgetab[indV1][i].splitIndex;
                break;
            }
        }

        if(6 == i){
            index = nbDirections_;
            directions_.push_back(vert);
            nbDirections_ = nbDirections_ +1;

            i = 0;
            while( (6 > i)  && (edgetab[indV1][i].splitIndex != -1) )
                i++;
            if (6 > i){
                edgetab[indV1][i].splitIndex = index;
                edgetab[indV1][i].splitPosition = vert;
            } else
                std::cerr << "DirectionSampler::getIndice : erreur de connectivité." << std::endl;

            i = 0;
            while( (6 > i)  && (edgetab[indV2][i].splitIndex != -1) )
                i++;
            if (6 > i){
                edgetab[indV2][i].splitIndex = index;
                edgetab[indV2][i].splitPosition = vert;
            } else
                std::cerr << "DirectionSampler::getIndice : erreur de connectivité." << std::endl;
        }

        return index;
    }

void DirectionSampler::compileGL(){
    std::vector<glm::vec4> lambdaomega;
#ifdef HALTON_SEQUENCE
    Halton_sampler halton_sampler;
    halton_sampler.init_faure();

    for (int i=0; i< nbDirections_; ++i) {
        float value = halton_sampler.sample(0, i+3);
        lambdaomega.push_back(glm::vec4(directions_[i], value ));
    }

#else
    // TODO : find or implement a better random generator (cf halton sequence)
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    for (int i=0; i< nbDirections_; ++i) {
        float value = distribution(generator) ;
        lambdaomega.push_back(glm::vec4(directions_[i], value));
    }
#endif

    glAssert(glGenVertexArrays(1, &mVertexArrayObject));

    // bind vertex Array
    glAssert(glBindVertexArray(mVertexArrayObject));

    // always generate all buffers : one for vertexdata one for indices
    glAssert(glGenBuffers(1, mVertexBufferObjects));

    // bind vertexdata
    glAssert(glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjects[VBO_VERTICES]));
    glAssert(glBufferData(GL_ARRAY_BUFFER, nbDirections_ * sizeof(glm::vec4),  &(lambdaomega[0]), GL_STATIC_DRAW));

    // global values
    GLuint stride = 0;
    GLboolean normalized = GL_FALSE;
    GLenum type = GL_FLOAT;

    // point
    GLuint index = 0;
    GLint size = 4;
    GLvoid *pointer = BUFFER_OFFSET(0);

    glAssert(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
    glAssert(glEnableVertexAttribArray(index));

    if (mTexture_)
        delete mTexture_;
    mTexture_= new Texture("spheresamples", GL_TEXTURE_RECTANGLE, Texture::TEX_RECT);
    mTexture_->textureFloatInitGL(GL_RGBA32F, nbDirections_, 1, GL_RGBA, GL_FLOAT, &(lambdaomega[0]));

}

void DirectionSampler::drawGL(){
    glAssert(glBindVertexArray(mVertexArrayObject));
    glAssert(glDrawArrays(GL_POINTS, 0, nbDirections_));
}

}
