/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef PFMREADER_H
#define PFMREADER_H

namespace vortex{
class PfmReader{
public :
    typedef struct {
        float r,g,b;
    } HDRPIXEL;

    typedef struct {
        int width;
        int height;
        HDRPIXEL *pixels;
    } HDRIMAGE;
    /*
   Read a possibly byte swapped floating point number
   Assume IEEE format
   */
    static HDRIMAGE * open(std::string fileName){
        FILE *fptr = fopen(fileName.c_str(), "rb");
        if(NULL == fptr){
            std::cerr << "could not open " << fileName << std::endl;
            return NULL;
        }
        int width, height, color, swap;
        if(!readHeader(fptr, width, height, color, swap)){
            std::cerr<< "bad header " << fileName << std::endl;
            return NULL;
        }

        //@todo : modulate swap with endianess
//        std::cerr << "pfm  " << fileName << " " << width << "x" << height << " " << color << " " << swap << std::endl;
        HDRPIXEL *pixels = new HDRPIXEL[width*height];
        for(int i=0; i<width*height; ++i){
            float *ptr = &(pixels[i].r);
            if(!readFloat(fptr, ptr, swap)) { std::cerr << "could not read pixel " << i << std::endl; }
            if(color == 3){
                if(!readFloat(fptr, ptr+1, swap)) { std::cerr << "could not read pixel " << i << std::endl; }
                if(!readFloat(fptr, ptr+2, swap)) { std::cerr << "could not read pixel " << i << std::endl; }
            }
        }
        HDRIMAGE *ret = new HDRIMAGE;
        ret->width = width;
        ret->height = height;
        ret->pixels = pixels;
        return ret;
    }

    static bool readFloat(FILE *fptr,float *n,int swap)
    {
        unsigned char *cptr, tmp;

        if (fread(n,4,1,fptr) != 1)
            return false;
        if (swap) {
            cptr = (unsigned char *)n;
            tmp = cptr[0];
            cptr[0] = cptr[3];
            cptr[3] =tmp;
            tmp = cptr[1];
            cptr[1] = cptr[2];
            cptr[2] = tmp;
        }

        return true;
    }

    static bool readHeader(FILE *fptr, int &width, int &height, int &color, int &swap){
        char buf[32];
        // read first line
        if(NULL == fgets(buf,32,fptr)) return false;
        // determine gray or color : PF -> color, Pf -> gray
        if(0 == strncmp("PF", buf, 2)) color = 3;
        else if(0 == strncmp("Pf", buf, 2)) color = 1;
        else return false;
        // read second line
        if(NULL == fgets(buf,32,fptr)) return false;
        // determine image size
        char *endptr = NULL;
        width = strtol(buf, &endptr, 10);
        if(endptr == buf) return false;
        height = strtol(endptr, (char **) NULL, 10);
        // read third line
        if(NULL == fgets(buf,32,fptr)) return false;
        if(endptr == buf) return false;
        float aspect = strtof(buf, &endptr);
        if(aspect>0) swap = 1;
        else swap = 0;
        return true;
    }
};
}


#endif // PFMREADER_H
