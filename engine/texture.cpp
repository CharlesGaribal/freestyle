/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "texture.h"
#include <QImage>
#include <QGLWidget>
#include <iostream>

namespace vortex {
Texture::Texture(std::string name, GLenum target, TexType type, GLuint zOffset)
: mName(name), mPixels(NULL), mTexId(0), mZOffset(zOffset), mTarget(target), mType(type) {
}

Texture *Texture::loadFromImage(std::string fileName)
{
        QImage img;
        QImage glImg;
        if(!img.load(fileName.c_str())){
            std::cerr << "texture not found (Texture::loadFromImage) " << fileName << std::endl;
            img = QImage(2,2,QImage::Format_RGB32);
            img.fill(QColor().green());
        }

        glImg = QGLWidget::convertToGLFormat(img);
        Texture * theTexture = new Texture(fileName, GL_TEXTURE_2D);

        int bpp = GL_RGB;
        GLenum format = GL_RGB;
        if(glImg.hasAlphaChannel()){
            bpp = GL_RGBA;
            format = GL_RGBA;
        }

        theTexture->initGL(bpp, glImg.width(), glImg.height(), format, GL_UNSIGNED_BYTE, glImg.bits());
        theTexture->useMipMap(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

        return theTexture;
}

int clamp(float i){
    return i>255?255:i<0?0:int(i);
}

void Texture::saveToImage(std::string fileName)
{
    QImage img(width(), height(), QImage::Format_ARGB32);
    GLfloat *bits = new GLfloat[width()*height()*4];
    bind(0);
    glGetTexImage(mTarget, 0, GL_RGBA, GL_FLOAT, bits);

    for(int j=0; j<height(); j++){
        for(int i=0; i<width(); i++){
            img.setPixel(i, height()-j-1,
                         qRgba(clamp(bits[(j*height()+i)*4]*255),
                         clamp(bits[(j*height()+i)*4+1]*255),
                    clamp(bits[(j*height()+i)*4+2]*255),
                    clamp(bits[(j*height()+i)*4+3]*255)));
        }
    }
    img.save(QString(fileName.c_str()));
    delete [] bits;
}


void Texture::textureFloatInitGL(int floatsperpixel, int width, int height, int format, int type, void * data)
{
    glAssert(glGenTextures(1, &mTexId));
    glAssert(glBindTexture(mTarget, mTexId));
    glAssert(glTexImage2D(mTarget, 0, floatsperpixel, width, height, 0, format, type, data));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

void Texture::initGL(int bytesperpixel, int width, int height, int format, int type, void* data)
{
    glAssert(glGenTextures(1, &mTexId));
    glAssert(glBindTexture(mTarget, mTexId));
    glAssert(glTexImage2D(mTarget, 0, bytesperpixel, width, height, 0, format, type, data));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, GL_REPEAT));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    mBytesPerPixels = (bytesperpixel==GL_RGB)?3:4;
    mWidth = width;
    mHeight = height;
    if (data) {
        mPixels = new unsigned char[mWidth*mHeight*mBytesPerPixels];
        memcpy(mPixels, data, mWidth*mHeight*mBytesPerPixels);
    }
}

void Texture::cubeInitGL(int bytesperpixel, int width, int height, int format, int type, void** data)
{
    glAssert(glGenTextures(1, &mTexId));
    glAssert(glBindTexture(mTarget, mTexId));
    glAssert(glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, bytesperpixel, width, height, 0, format, GL_FLOAT, data[0]));
    glAssert(glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, bytesperpixel, width, height, 0, format, GL_FLOAT, data[1]));
    glAssert(glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, bytesperpixel, width, height, 0, format, GL_FLOAT, data[2]));
    glAssert(glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, bytesperpixel, width, height, 0, format, GL_FLOAT, data[3]));
    glAssert(glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, bytesperpixel, width, height, 0, format, GL_FLOAT, data[4]));
    glAssert(glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, bytesperpixel, width, height, 0, format, GL_FLOAT, data[5]));
    /*
    glAssert(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glAssert(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glAssert(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    */
    glAssert(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glAssert(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

}

/** TODO Check that
  (param) width  is 2n for some integer n
  (param) height is 2m for some integer m
  (param) depth  is 2k for some integer k
  **/
void Texture::volumeInitGL(int bytesperpixel, int width, int height, int depth, int format, int type, void * data) {

    glAssert(glGenTextures(1, &mTexId));
    glAssert(glBindTexture(mTarget, mTexId));
    glAssert(glTexImage3D(mTarget, 0, bytesperpixel, width, height, depth, 0, format, type, data));
    glAssert(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glAssert(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    glAssert(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glAssert(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

void Texture::useMipMap(GLenum minFliter, GLenum magFilter)
{
    glAssert(glBindTexture(mTarget, mTexId));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, magFilter));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, minFliter));
    glAssert(glGenerateMipmap(mTarget));
}

void Texture::setFilter(GLenum minFliter, GLenum magFilter)
{
    glAssert(glBindTexture(mTarget, mTexId));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, magFilter));
    glAssert(glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, minFliter));
}

void Texture::setClamp(GLenum warpS, GLenum warpT)
{
    glAssert(glBindTexture(mTarget, mTexId));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpT);
}

void Texture::setClamp(GLenum warpS, GLenum warpT, GLenum warpR) {
    glAssert(glBindTexture(mTarget, mTexId));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warpS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warpT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, warpR);
}

void Texture::bind(int unit)
{
    GLenum texUnit = GL_TEXTURE0+unit;
    glAssert(glActiveTexture(texUnit));
    glAssert(glBindTexture(mTarget, mTexId));
}

void Texture::deleteGL()
{
   glAssert( glDeleteTextures(1, &mTexId));
}

glm::vec4 Texture::getTexel(float u, float v){

    float scaledU = fmodf(u * mWidth, mWidth);
    float scaledV = fmodf(v * mHeight, mHeight);

    int is = (int)(scaledU);
    int it = (int)(scaledV);

    float ds = scaledU - is;
    float dt = scaledV - it;

    glm::vec3 centerColor;
    int texPos = (it*mWidth+is)*mBytesPerPixels; // mBytesPerPixels*((mHeight-1-is)*mWidth+it)
    unsigned char *texel= &(mPixels[texPos]);
    centerColor = glm::vec3( (float)(texel[0])/255.f, (float)(texel[1])/255.f, (float)(texel[2])/255.f );

    glm::vec3 rightColor;
    if (is <(mWidth-1)){
        texPos = (it*mWidth+is+1)*mBytesPerPixels; // mBytesPerPixels*((mHeight-1-is)*mWidth+it)
        texel= &(mPixels[texPos]);
        rightColor = glm::vec3( (float)(texel[0])/255.f, (float)(texel[1])/255.f, (float)(texel[2])/255.f );
    } else {
        rightColor = centerColor;
    }

    glm::vec3 upColor;
    if (it <(mHeight-1)){
        texPos = ((it+1)*mWidth+is)*mBytesPerPixels; // mBytesPerPixels*((mHeight-1-is)*mWidth+it)
        texel= &(mPixels[texPos]);
        upColor = glm::vec3( (float)(texel[0])/255.f, (float)(texel[1])/255.f, (float)(texel[2])/255.f );
    } else {
        upColor = centerColor;
    }

    glm::vec3 upRightColor;
    if ( (it <(mHeight-1)) && (is <(mWidth-1)) ){
        texPos = ((it+1)*mWidth+is+1)*mBytesPerPixels; // mBytesPerPixels*((mHeight-1-is)*mWidth+it)
        texel= &(mPixels[texPos]);
        upRightColor = glm::vec3( (float)(texel[0])/255.f, (float)(texel[1])/255.f, (float)(texel[2])/255.f );
    } else {
        upRightColor = centerColor;
    }

    rightColor = (1.f-ds)*centerColor + ds*rightColor;
    upColor = (1.f-ds)*upColor + ds*upRightColor;
    glm::vec3 color = (1.f-dt)*rightColor + dt*upColor;

    return glm::vec4(color, 1.f);


/*
    if (mBytesPerPixels == 3)
        return glm::vec4((float)(texel[0])/255.f, (float)(texel[1])/255.f, (float)(texel[2])/255.f, 1.f);
    else
        return glm::vec4((float)(texel[0])/255.f, (float)(texel[1])/255.f, (float)(texel[2])/255.f, (float)(texel[3])/255.f);
*/
}

}
