/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "skybox.h"

/// @todo remove QT dependencies here -> why ???
#include <QGLWidget>
#include <QFile>
#include <QImage>

namespace vortex{

SkyBox::SkyBox() : valid(false), mTex(NULL){
    Mesh::VertexData vertices[8];
    vertices[0].mVertex = glm::vec3(1.000000  ,-1.000000, -1.000000);
    vertices[1].mVertex = glm::vec3(1.000000  ,-1.000000, 1.000000 );
    vertices[2].mVertex = glm::vec3(-1.000000 ,-1.000000, 1.000000 );
    vertices[3].mVertex = glm::vec3(-1.000000 ,-1.000000, -1.000000);
    vertices[4].mVertex = glm::vec3(1.000000  , 1.000000, -1.000000);
    vertices[5].mVertex = glm::vec3(1.000000  , 1.000000, 1.000000 );
    vertices[6].mVertex = glm::vec3(-1.000000 , 1.000000, 1.000000 );
    vertices[7].mVertex = glm::vec3(-1.000000 , 1.000000, -1.000000);
    vertices[0].mTexCoord=glm::vec4(vertices[0].mVertex, 1.0f);
    vertices[1].mTexCoord=glm::vec4(vertices[1].mVertex, 1.0f);
    vertices[2].mTexCoord=glm::vec4(vertices[2].mVertex, 1.0f);
    vertices[3].mTexCoord=glm::vec4(vertices[3].mVertex, 1.0f);
    vertices[4].mTexCoord=glm::vec4(vertices[4].mVertex, 1.0f);
    vertices[5].mTexCoord=glm::vec4(vertices[5].mVertex, 1.0f);
    vertices[6].mTexCoord=glm::vec4(vertices[6].mVertex, 1.0f);
    vertices[7].mTexCoord=glm::vec4(vertices[7].mVertex, 1.0f);

    int indices[36] = { 0, 1, 2, 0, 2, 3,
                        4, 7, 6, 4, 6, 5,
                        0, 4, 5, 0, 5, 1,
                        1, 5, 6, 1, 6, 2,
                        2, 6, 7, 2, 7, 3,
                        4, 0, 3, 4, 3, 7};

    mCube = new Mesh("skyBox", vertices, 8, indices, 36);
    mCube->init();

    mTex = new Texture("sky", GL_TEXTURE_CUBE_MAP, Texture::TEX_CUBE, 0);
}

SkyBox::~SkyBox(){
    delete mTex;
    delete mCube;
}

void SkyBox::setupTextures(std::string name, int type){


    int width;
    int height;

    if (type == 0) {
        PfmReader::HDRIMAGE *img = PfmReader::open(name.c_str());
        width = img->width/3;
        height = img->height/4;
        for(int imgIdx = 0; imgIdx <6; ++imgIdx){
            mData[imgIdx]=new float[width*height*4];
            int xOffset = 0;
            int yOffset = 0;
            switch(imgIdx){
            case 0 : xOffset = 2*width; yOffset = 2*height; break;
            case 1 : xOffset = 0*width; yOffset = 2*height; break;
            case 2 : xOffset = 1*width; yOffset = 3*height; break;
            case 3 : xOffset = 1*width; yOffset = 1*height; break;
            case 4 : xOffset = 1*width; yOffset = 2*height; break;
            case 5 : xOffset = 1*width; yOffset = 0*height; break;
            default: xOffset = 0; yOffset = 0; break;
            }
            for(int i=0; i<height; ++i)
                for(int j=0; j<width; ++j){
                    if( (imgIdx != 5)){
                        mData[imgIdx][4*((height-1-i)*width+j)+0] = img->pixels[(i+yOffset)*img->width+j+xOffset].r;
                        mData[imgIdx][4*((height-1-i)*width+j)+1] = img->pixels[(i+yOffset)*img->width+j+xOffset].g;
                        mData[imgIdx][4*((height-1-i)*width+j)+2] = img->pixels[(i+yOffset)*img->width+j+xOffset].b;
                        mData[imgIdx][4*((height-1-i)*width+j)+3] = 1.f;
                    }
                    if( imgIdx ==5 ){
                        mData[imgIdx][4*(i*width+width-1-j)+0] = img->pixels[(i+yOffset)*img->width+j+xOffset].r;
                        mData[imgIdx][4*(i*width+width-1-j)+1] = img->pixels[(i+yOffset)*img->width+j+xOffset].g;
                        mData[imgIdx][4*(i*width+width-1-j)+2] = img->pixels[(i+yOffset)*img->width+j+xOffset].b;
                        mData[imgIdx][4*(i*width+width-1-j)+3] = 1.f;
                    }
                }
        }
        delete [] img->pixels;
        delete img;
    } else {
        const float lightFactor = 3.f;

        for(int imgIdx = 0; imgIdx <6; ++imgIdx){
            QImage img;
            QImage glImg;
            QString base(name.c_str());
            QString filename;
            switch(imgIdx){
            case 0 : filename=base+"posx";break;
            case 1 : filename=base+"negx";break;
            case 2 : filename=base+"posy";break;
            case 3 : filename=base+"negy";break;
            case 4 : filename=base+"posz";break;
            case 5 : filename=base+"negz";break;
            }
            QString finalFileName = filename+".png";

            if(!QFile(finalFileName).exists())
                finalFileName = filename+".jpg";

            if(!QFile(finalFileName).exists()){
                switch(imgIdx){
                case 0 : filename=base+"-X-plux";break;
                case 1 : filename=base+"-X-minux";break;
                case 2 : filename=base+"-Y-plux";break;
                case 3 : filename=base+"-Y-minux";break;
                case 4 : filename=base+"-Z-plux";break;
                case 5 : filename=base+"-Z-minux";break;
                }
                finalFileName = filename+".png";
                if(!QFile(finalFileName).exists())
                    finalFileName = filename+".jpg";
                if(!QFile(finalFileName).exists()){
                    std::cerr << "Image " << filename.toStdString().c_str() << " not found" << std::endl;
                }
            }
            filename = finalFileName;

            img = QImage(filename);
            if (img.isNull()){
                std::cerr << "Image " << filename.toStdString().c_str() << " not loaded" << std::endl;
                return;
            }

            glImg = QGLWidget::convertToGLFormat(img);
            width = glImg.width();
            height = glImg.height();
            mData[imgIdx]=new float[width*height*4];
            std::cerr << "Loading " << filename.toStdString().c_str() << ": " << width << " " << height << std::endl;

            for(int i=0; i<height; ++i){
                for(int j=0; j<width; ++j){
                    mData[imgIdx][4*((height-1-i)*width+j)+0] = lightFactor*((float)(glImg.bits()[(j+width*i)*4]))/255.f;
                    mData[imgIdx][4*((height-1-i)*width+j)+1] = lightFactor*((float)(glImg.bits()[(j+width*i)*4+1]))/255.f;
                    mData[imgIdx][4*((height-1-i)*width+j)+2] = lightFactor*((float)(glImg.bits()[(j+width*i)*4+2]))/255.f;
                    mData[imgIdx][4*((height-1-i)*width+j)+3] = 1.0;
                }
            }
        }

    }

    mWidth = width;
    mHeight = height;

    mTex->cubeInitGL(GL_RGBA32F, mWidth, mHeight, GL_RGBA, GL_FLOAT, reinterpret_cast<void**>(mData));
    mTex->useMipMap(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    computeSHMatrices();

    valid=true;
}

void SkyBox::draw(){
    glDepthMask(GL_FALSE);
    mCube->draw();
    glDepthMask(GL_TRUE);
}


void SkyBox::tomatrix(void) {

    /* Form the quadratic form matrix (see equations 11 and 12 in paper) */

    int col ;
    float c1,c2,c3,c4,c5 ;
    c1 = 0.429043 ; c2 = 0.511664 ;
    c3 = 0.743125 ; c4 = 0.886227 ; c5 = 0.247708 ;

    for (col = 0 ; col < 3 ; col++) { /* Equation 12 */

        mShMatrices[col][0][0] = c1*coeffs[8][col] ; /* c1 L_{22}  */
        mShMatrices[col][0][1] = c1*coeffs[4][col] ; /* c1 L_{2-2} */
        mShMatrices[col][0][2] = c1*coeffs[7][col] ; /* c1 L_{21}  */
        mShMatrices[col][0][3] = c2*coeffs[3][col] ; /* c2 L_{11}  */

        mShMatrices[col][1][0] = c1*coeffs[4][col] ; /* c1 L_{2-2} */
        mShMatrices[col][1][1] = -c1*coeffs[8][col]; /*-c1 L_{22}  */
        mShMatrices[col][1][2] = c1*coeffs[5][col] ; /* c1 L_{2-1} */
        mShMatrices[col][1][3] = c2*coeffs[1][col] ; /* c2 L_{1-1} */

        mShMatrices[col][2][0] = c1*coeffs[7][col] ; /* c1 L_{21}  */
        mShMatrices[col][2][1] = c1*coeffs[5][col] ; /* c1 L_{2-1} */
        mShMatrices[col][2][2] = c3*coeffs[6][col] ; /* c3 L_{20}  */
        mShMatrices[col][2][3] = c2*coeffs[2][col] ; /* c2 L_{10}  */

        mShMatrices[col][3][0] = c2*coeffs[3][col] ; /* c2 L_{11}  */
        mShMatrices[col][3][1] = c2*coeffs[1][col] ; /* c2 L_{1-1} */
        mShMatrices[col][3][2] = c2*coeffs[2][col] ; /* c2 L_{10}  */
        mShMatrices[col][3][3] = c4*coeffs[0][col] - c5*coeffs[6][col] ;
        /* c4 L_{00} - c5 L_{20} */
    }
}


void SkyBox::updateCoeffs(float *hdr, float x, float y, float z, float domega) {

    /******************************************************************
   Update the coefficients (i.e. compute the next term in the
   integral) based on the lighting value hdr[3], the differential
   solid angle domega and cartesian components of surface normal x,y,z

   Inputs:  hdr = L(x,y,z) [note that x^2+y^2+z^2 = 1]
            i.e. the illumination at position (x,y,z)

            domega = The solid angle at the pixel corresponding to
            (x,y,z).  For these light probes, this is given by

            x,y,z  = Cartesian components of surface normal

   Notes:   Of course, there are better numerical methods to do
            integration, but this naive approach is sufficient for our
            purpose.

  *********************************************************************/

    int col ;
    for (col = 0 ; col < 3 ; col++) {
        float c ; /* A different constant for each coefficient */

        /* L_{00}.  Note that Y_{00} = 0.282095 */
        c = 0.282095f ;
        coeffs[0][col] += hdr[col]*c*domega ;

        /* L_{1m}. -1 <= m <= 1.  The linear terms */
        c = 0.488603f ;
        coeffs[1][col] += hdr[col]*(c*y)*domega ;   /* Y_{1-1} = 0.488603 y  */
        coeffs[2][col] += hdr[col]*(c*z)*domega ;   /* Y_{10}  = 0.488603 z  */
        coeffs[3][col] += hdr[col]*(c*x)*domega ;   /* Y_{11}  = 0.488603 x  */

        /* The Quadratic terms, L_{2m} -2 <= m <= 2 */

        /* First, L_{2-2}, L_{2-1}, L_{21} corresponding to xy,yz,xz */
        c = 1.092548f ;
        coeffs[4][col] += hdr[col]*(c*x*y)*domega ; /* Y_{2-2} = 1.092548 xy */
        coeffs[5][col] += hdr[col]*(c*y*z)*domega ; /* Y_{2-1} = 1.092548 yz */
        coeffs[7][col] += hdr[col]*(c*x*z)*domega ; /* Y_{21}  = 1.092548 xz */

        /* L_{20}.  Note that Y_{20} = 0.315392 (3z^2 - 1) */
        c = 0.315392f ;
        coeffs[6][col] += hdr[col]*(c*(3*z*z-1))*domega ;

        /* L_{22}.  Note that Y_{22} = 0.546274 (x^2 - y^2) */
        c = 0.546274f ;
        coeffs[8][col] += hdr[col]*(c*(x*x-y*y))*domega ;

    }
}

float sinc(float x) {               /* Supporting sinc function */
    if (fabs(x) < 1.0e-4f) return 1.0f ;
    else return(sin(x)/x) ;
}

float *SkyBox::getPixel(float x, float y, float z){


    float ma = fabs(x);
    int axis = (x>0);
    float tc = axis?z:-z;
    float sc = y;
    if (fabs(y) > ma) {
        ma = fabs(y);
        axis = 2+(y<0);
        tc = -x;
        sc = (axis==2)?-z:z;
    }
    if (fabs(z) > ma) {
        ma = fabs(z);
        axis = 4+(z<0);
        tc = (axis==4)?-x:x;
        sc = y;
    }

    float s = 0.5f*(1.f + sc/ma);
    float t = 0.5f*(1.f + tc/ma);
    int is = (int)(s * mWidth);
    int it = (int)(t * mHeight);

    return &(mData[axis][4*((mHeight-1-is)*mWidth+it)]);
}

// http://graphics.stanford.edu/papers/envmap/prefilter.c
void SkyBox::computeSHMatrices(){
    float theta;
    float phi;
    float x,y,z;
    for (int i=0; i<9;i++)
        for (int j=0;j<3; j++)
            coeffs[i][j]=0.f;


#if 1
    const float dtheta = 0.005;
    const float dphi = 0.005;

    for (theta=0.f; theta<M_PI; theta+=dtheta)
        for (phi=0.f; phi<2.f*M_PI; phi+=dphi) {
            x = sin(theta)*cos(phi) ;
            y = sin(theta)*sin(phi) ;
            z = cos(theta) ;
            float *thePixel = getPixel(x,y,z);
            updateCoeffs(thePixel, x, y, z, sin(theta)*dtheta*dphi);

        }
#else



    int testwidth = 1024;

    unsigned char *sphereimage;
    sphereimage = new unsigned char[3*testwidth*testwidth];


    for (int i = 0 ; i < testwidth ; i++)
        for (int j = 0 ; j < testwidth ; j++) {

            /* We now find the cartesian components for the point (i,j) */
            float u,v,r;

            v = (testwidth/2.0 - j)/(testwidth/2.0);  /* v ranges from -1 to 1 */
            u = (testwidth/2.0 - i)/(testwidth/2.0);    /* u ranges from -1 to 1 */
            r = sqrt(u*u+v*v) ;               /* The "radius" */
            if (r > 1.0f) {
                sphereimage[3*(j*testwidth+i) + 0] = 0;
                sphereimage[3*(j*testwidth+i) + 1] = 0;
                sphereimage[3*(j*testwidth+i) + 2] = 0;
                continue ;           /* Consider only circle with r<1 */
            }

            theta = M_PI*r ;                    /* theta parameter of (i,j) */
            phi = atan2(v,u) ;                /* phi parameter */

            x = sin(theta)*cos(phi) ;         /* Cartesian components */
            y = sin(theta)*sin(phi) ;
            z = cos(theta) ;

            //float *thePixel = getPixel(data, width, height, x,y,z);
            float *thePixel = getPixel(x,y,z);
            updateCoeffs(thePixel, x, y, z, (2.f*M_PI/testwidth)*(2.f*M_PI/testwidth)*sinc(theta));


            glm::vec3 color;
            color[0] = thePixel[0];
            color[1] = thePixel[1];
            color[2] = thePixel[2];
            color = glm::clamp(color, glm::vec3(0.f), glm::vec3(1.f));
            sphereimage[3*(j*testwidth+i) + 0] = static_cast<unsigned char>(color[0]*255);
            sphereimage[3*(j*testwidth+i) + 1] = static_cast<unsigned char>(color[1]*255);
            sphereimage[3*(j*testwidth+i) + 2] = static_cast<unsigned char>(color[2]*255);


        }

    QImage theInputImage( sphereimage, testwidth, testwidth, testwidth*3, QImage::Format_RGB888 );
    if (theInputImage.save("sphericalImage.png","PNG"))
        std::cerr << "Saved to sphericalImage.png "<< std::endl;
    else
        std::cerr << "Not Saved" << std::endl;
    delete [] sphereimage;

#endif
    tomatrix();

#if 0
    int ambientWidth = 1024;
    unsigned char *ambientImage;
    ambientImage = new unsigned char[3*ambientWidth*ambientWidth];
    for (int i = 0 ; i < ambientWidth ; i++)
        for (int j = 0 ; j < ambientWidth ; j++) {

            /* We now find the cartesian components for the point (i,j) */
            float u,v,r;

            v = (ambientWidth/2.0 - j)/(ambientWidth/2.0);  /* v ranges from -1 to 1 */
            u = (ambientWidth/2.0 - i)/(ambientWidth/2.0);    /* u ranges from -1 to 1 */
            r = sqrt(u*u+v*v) ;               /* The "radius" */
            if (r > 1.0f) {
                ambientImage[3*(j*ambientWidth+i) + 0] = 0;
                ambientImage[3*(j*ambientWidth+i) + 1] = 0;
                ambientImage[3*(j*ambientWidth+i) + 2] = 0;
                continue ;           /* Consider only circle with r<1 */
            }

            theta = M_PI*r ;                    /* theta parameter of (i,j) */
            phi = atan2(v,u) ;                /* phi parameter */

            x = sin(theta)*cos(phi) ;         /* Cartesian components */
            y = sin(theta)*sin(phi) ;
            z = cos(theta) ;

            // color = NtMN
            glm::vec3 color;
            glm::vec4 normal(x,y,z, 1.f);

            glm::vec4 MN;
            MN= mShMatrices[0]*normal;
            color[0] = glm::dot(normal, MN);
            MN= mShMatrices[1]*normal;
            color[1] = glm::dot(normal, MN);
            MN= mShMatrices[2]*normal;
            color[2] = glm::dot(normal, MN);

            color = color * 0.1f;
            color = glm::clamp(color, glm::vec3(0.f), glm::vec3(1.f));
            ambientImage[3*(j*ambientWidth+i) + 0] = static_cast<unsigned char>(color[0]*255);
            ambientImage[3*(j*ambientWidth+i) + 1] = static_cast<unsigned char>(color[1]*255);
            ambientImage[3*(j*ambientWidth+i) + 2] = static_cast<unsigned char>(color[2]*255);


        }
    QImage theAmbientImage( ambientImage, ambientWidth, ambientWidth, ambientWidth*3, QImage::Format_RGB888 );
    if (theAmbientImage.save("ambientimage.png","PNG"))
        std::cerr << "Saved to ambientimage.png "<< std::endl;
    else
        std::cerr << "Not Saved" << std::endl;
    delete [] ambientImage;
#endif
#if 0
    /* Output Results */


    printf("\n         Lighting Coefficients\n\n") ;
    printf("(l,m)       RED        GREEN     BLUE\n") ;

    printf("L_{0,0}   %9.6f %9.6f %9.6f\n",
           coeffs[0][0],coeffs[0][1],coeffs[0][2]) ;
    printf("L_{1,-1}  %9.6f %9.6f %9.6f\n",
           coeffs[1][0],coeffs[1][1],coeffs[1][2]) ;
    printf("L_{1,0}   %9.6f %9.6f %9.6f\n",
           coeffs[2][0],coeffs[2][1],coeffs[2][2]) ;
    printf("L_{1,1}   %9.6f %9.6f %9.6f\n",
           coeffs[3][0],coeffs[3][1],coeffs[3][2]) ;
    printf("L_{2,-2}  %9.6f %9.6f %9.6f\n",
           coeffs[4][0],coeffs[4][1],coeffs[4][2]) ;
    printf("L_{2,-1}  %9.6f %9.6f %9.6f\n",
           coeffs[5][0],coeffs[5][1],coeffs[5][2]) ;
    printf("L_{2,0}   %9.6f %9.6f %9.6f\n",
           coeffs[6][0],coeffs[6][1],coeffs[6][2]) ;
    printf("L_{2,1}   %9.6f %9.6f %9.6f\n",
           coeffs[7][0],coeffs[7][1],coeffs[7][2]) ;
    printf("L_{2,2}   %9.6f %9.6f %9.6f\n",
           coeffs[8][0],coeffs[8][1],coeffs[8][2]) ;

    printf("\nMATRIX M: RED\n") ;
    for (int i = 0 ; i < 4 ; i++) {
        for (int j = 0 ; j < 4 ; j++)
            printf("%9.6f ",mShMatrices[0][i][j]) ;
        printf("\n") ;
    }
    printf("\nMATRIX M: GREEN\n") ;
    for (int i = 0 ; i < 4 ; i++) {
        for (int j = 0 ; j < 4 ; j++)
            printf("%9.6f ",mShMatrices[1][i][j]) ;
        printf("\n") ;
    }
    printf("\nMATRIX M: BLUE\n") ;
    for (int i = 0 ; i < 4 ; i++) {
        for (int j = 0 ; j < 4 ; j++)
            printf("%9.6f ",mShMatrices[2][i][j]) ;
        printf("\n") ;
    }
#endif
}

} // namespace

