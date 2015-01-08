/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "distancefield.h"
#include "timer.h"

namespace vortex {
using namespace util;

/* from http://www.geometrictools.com/LibMathematics/Distance/Distance.html */
float distPointToTriangle(Mesh::VertexData *triangleVertices[3], const glm::vec3 triangleEdges[2], const glm::vec3 &point, glm::vec3 &offset, float bar[3]){
    glm::vec3 diff = triangleVertices[0]->mVertex - point;
    const glm::vec3 &edge0(triangleEdges[0]);// = triangleVertices[1]->mVertex - triangleVertices[0]->mVertex;
    const glm::vec3 &edge1(triangleEdges[1]);// = triangleVertices[2]->mVertex - triangleVertices[0]->mVertex;
    float a00 = glm::dot(edge0, edge0);
    float a01 = glm::dot(edge0, edge1);
    float a11 = glm::dot(edge1, edge1);
    float b0 = glm::dot(diff, edge0);
    float b1 = glm::dot(diff, edge1);
    float c = glm::dot(diff, diff);
    float det = fabsf(a00*a11 - a01*a01);
    float s = a01*b1 - a11*b0;
    float t = a01*b0 - a00*b1;
    float sqrDistance;

    if (s + t <= det) {
        if (s < 0.f) {
            if (t < 0.f) {  // region 4
                if (b0 < 0.f) {
                    t = 0.f;
                    if (-b0 >= a00) {
                        s = 1.f;
                        sqrDistance = a00 + (2.f)*b0 + c;
                    } else {
                        s = -b0/a00;
                        sqrDistance = b0*s + c;
                    }
                } else {
                    s = 0.f;
                    if (b1 >= 0.f) {
                        t = 0.f;
                        sqrDistance = c;
                    } else if (-b1 >= a11) {
                        t = 1.f;
                        sqrDistance = a11 + (2.f)*b1 + c;
                    } else {
                        t = -b1/a11;
                        sqrDistance = b1*t + c;
                    }
                }
            } else { // region 3
                s = 0.f;
                if (b1 >= 0.f) {
                    t = 0.f;
                    sqrDistance = c;
                } else if (-b1 >= a11) {
                    t = 1.f;
                    sqrDistance = a11 + (2.f)*b1 + c;
                } else {
                    t = -b1/a11;
                    sqrDistance = b1*t + c;
                }
            }
        } else if (t < 0.f) { // region 5
            t = 0.f;
            if (b0 >= 0.f) {
                s = 0.f;
                sqrDistance = c;
            } else if (-b0 >= a00) {
                s = 1.f;
                sqrDistance = a00 + (2.f)*b0 + c;
            } else {
                s = -b0/a00;
                sqrDistance = b0*s + c;
            }
        } else {  // region 0
            // minimum at interior point
            float invDet = (1.f)/det;
            s *= invDet;
            t *= invDet;
            sqrDistance = s*(a00*s + a01*t + (2.f)*b0) +
                t*(a01*s + a11*t + (2.f)*b1) + c;
        }
    } else {
        float tmp0, tmp1, numer, denom;

        if (s < 0.f) {   // region 2
            tmp0 = a01 + b0;
            tmp1 = a11 + b1;
            if (tmp1 > tmp0) {
                numer = tmp1 - tmp0;
                denom = a00 - (2.f)*a01 + a11;
                if (numer >= denom) {
                    s = 1.f;
                    t = 0.f;
                    sqrDistance = a00 + (2.f)*b0 + c;
                } else {
                    s = numer/denom;
                    t = 1.f - s;
                    sqrDistance = s*(a00*s + a01*t + (2.f)*b0) +
                        t*(a01*s + a11*t + (2.f)*b1) + c;
                }
            } else {
                s = 0.f;
                if (tmp1 <= 0.f) {
                    t = 1.f;
                    sqrDistance = a11 + (2.f)*b1 + c;
                } else if (b1 >= 0.f) {
                    t = 0.f;
                    sqrDistance = c;
                } else {
                    t = -b1/a11;
                    sqrDistance = b1*t + c;
                }
            }
        } else if (t < 0.f) { // region 6
            tmp0 = a01 + b1;
            tmp1 = a00 + b0;
            if (tmp1 > tmp0) {
                numer = tmp1 - tmp0;
                denom = a00 - (2.f)*a01 + a11;
                if (numer >= denom) {
                    t = 1.f;
                    s = 0.f;
                    sqrDistance = a11 + (2.f)*b1 + c;
                } else {
                    t = numer/denom;
                    s = 1.f - t;
                    sqrDistance = s*(a00*s + a01*t + (2.f)*b0) +
                        t*(a01*s + a11*t + (2.f)*b1) + c;
                }
            } else {
                t = 0.f;
                if (tmp1 <= 0.f) {
                    s = 1.f;
                    sqrDistance = a00 + (2.f)*b0 + c;
                } else if (b0 >= 0.f) {
                    s = 0.f;
                    sqrDistance = c;
                } else {
                    s = -b0/a00;
                    sqrDistance = b0*s + c;
                }
            }
        } else {  // region 1
            numer = a11 + b1 - a01 - b0;
            if (numer <= 0.f) {
                s = 0.f;
                t = 1.f;
                sqrDistance = a11 + (2.f)*b1 + c;
            } else {
                denom = a00 - (2.f)*a01 + a11;
                if (numer >= denom) {
                    s = 1.f;
                    t = 0.f;
                    sqrDistance = a00 + (2.f)*b0 + c;
                } else {
                    s = numer/denom;
                    t = 1.f - s;
                    sqrDistance = s*(a00*s + a01*t + (2.f)*b0) +
                        t*(a01*s + a11*t + (2.f)*b1) + c;
                }
            }
        }
    }

    // Account for numerical round-off error.
    if (sqrDistance < 0.f)
    {
        sqrDistance = 0.f;
    }

    /*
    mClosestPoint0 = *mPoint;
    mClosestPoint1 = mTriangle->V[0] + s*edge0 + t*edge1;
    mTriangleBary[1] = s;
    mTriangleBary[2] = t;
    mTriangleBary[0] = (Real)1 - s - t;
    */
    offset = triangleVertices[0]->mVertex + edge0 * s + edge1 * t - point;
    bar[0] = 1.f - s - t;
    bar[1] = s;
    bar[2] = t;

    return sqrDistance;
}


/* Visitor operation for adding an object to a distance field */
DistanceFieldAdder::DistanceFieldAdder(DistanceField *distanceField){
    mDistanceField = distanceField;
}

void DistanceFieldAdder::operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix){
    if (theNode->isLeaf()) {
        SceneGraph::LeafMeshNode *leafNode = static_cast<SceneGraph::LeafMeshNode *>(theNode);
        RenderState *state = NULL;
         for (int i = 0; i < leafNode->nMeshes(); ++i) {
            state = leafNode->getRenderState(i);
            mDistanceField->addMesh((*leafNode)[i], state->getMaterial(), modelViewMatrix );
        }
    }
}



/* ---------------- */

DistanceField::DistanceField()
{
    mVoxelGrid = NULL;
}

bool  DistanceField::inVoxel(int i, int j, int k, const glm::vec4 &triangle){
    // TODO : detect precisely if triangle lies in voxel i, j, k
    // for now, as this is called from bbox rasterization, return true
    return true;
}


void DistanceField::DFVoxel::addTriangle(int t, Mesh::VertexData *triangleVertices[3], const glm::vec4 &trianglePlane, const glm::vec3 triangleEdges[2]){
    if (0 == nbTriangles){
        triangles = new std::vector<int>;
    }
    triangles->push_back(t);
    //normal = (normal * (float)nbTriangles + glm::vec3(trianglePlane))/ (float)(++nbTriangles);
    glm::vec3 toNearestPoint;
    float bar[3];
    float d = distPointToTriangle(triangleVertices, triangleEdges, point, toNearestPoint, bar);
    if (d<dist){
        dist = d;
        offset = toNearestPoint;
        barycentricCoords[0] = bar[0];
        barycentricCoords[1] = bar[1];
        barycentricCoords[2] = bar[2];
        nearestTriangle = t;
    }

}
/* precision : un DF est constitué de voxels cubique.
    On donne la taille d'un voxel et le nombre de subdivision sur chaque axe
    sera calculé à partir de la boite englobante : nbsubdiv_axe = (int)(precision/lg_axe) + 1
*/
#define index(i, j, k) ( (k*mGridSize.y + j) * mGridSize.x + i)

void DistanceField::build(float precision){
    Timer tbuild;
    mGridStep = precision;
    // Computing dimensions
    // 1 - extend bbox a little
    glm::vec3 diag = mDistanceFieldBox.getMax() - mDistanceFieldBox.getMin();
    diag = diag * 0.1f;
    mDistanceFieldBox += (mDistanceFieldBox.getMin() - diag);
    mDistanceFieldBox += (mDistanceFieldBox.getMax() + diag);
    diag = mDistanceFieldBox.getMax() - mDistanceFieldBox.getMin();

    // Compute per-axis subdivision
    mGridSize = glm::ivec3(diag / mGridStep) + glm::ivec3(1);
    glm::ivec3 log2size(glm::log2(glm::vec3(mGridSize)));
    log2size+=1;
    glm::ivec3 pow2size(glm::pow( glm::vec3(2.f), glm::vec3(log2size) ));
    glm::vec3 newDiag = diag / glm::vec3(pow2size);
    float newSize = glm::max(newDiag.x, glm::max(newDiag.y, newDiag.z));
    mGridStep = newSize;
    mGridSize = pow2size;

    std::cerr << "mDistanceFieldBox " << mDistanceFieldBox << std::endl;
    std::cerr << "diag " << diag << std::endl;
    std::cerr << "mGridSize " << mGridSize << std::endl;
    std::cerr << "mGridStep " << mGridStep << std::endl;


    tbuild.start();
    // linear 3D grid : access with (k*mGridSize.y +j) * mGridSize.x + i;
    mVoxelGrid = new ptrVoxel[mGridSize.x * mGridSize.y * mGridSize.z];
    memset(mVoxelGrid, 0, mGridSize.x * mGridSize.y * mGridSize.z * sizeof(ptrVoxel));

    for (unsigned int t=0; t<mIndices.size(); t+=3){
        // Get triangle info
        Mesh::VertexData *triangleVertices[3]={&(mVertices[mIndices[t]]), &(mVertices[mIndices[t+1]]), &(mVertices[mIndices[t+2]])};
        glm::vec4 trianglePlane( glm::cross(
                                                (triangleVertices[1]->mVertex - triangleVertices[0]->mVertex),
                                                (triangleVertices[2]->mVertex - triangleVertices[0]->mVertex)
                                            ),
                                  0.0);
        trianglePlane.w = - glm::dot(glm::vec3(trianglePlane), triangleVertices[0]->mVertex);
        glm::vec3 triangleEdges[2]={
                    triangleVertices[1]->mVertex - triangleVertices[0]->mVertex,
                    triangleVertices[2]->mVertex - triangleVertices[0]->mVertex
        };

        BBox triangleBox;
        triangleBox += triangleVertices[0]->mVertex;
        triangleBox += triangleVertices[1]->mVertex;
        triangleBox += triangleVertices[2]->mVertex;

        // compute triangle extend in grid coordinates
#ifdef LIMITTOBBOX
        glm::ivec3 startIndices( (triangleBox.getMin() - mDistanceFieldBox.getMin() ) / mGridStep);
        glm::ivec3 endIndices( (triangleBox.getMax() - mDistanceFieldBox.getMin() ) / mGridStep);
#else
#define VOXEL_BORDER 4
        glm::ivec3 imin( (triangleBox.getMin() - mDistanceFieldBox.getMin() ) / mGridStep);
        glm::ivec3 imax( (triangleBox.getMax() - mDistanceFieldBox.getMin() ) / mGridStep);

        glm::ivec3 startIndices( glm::max( imin - glm::ivec3(VOXEL_BORDER), glm::ivec3(0) ) );
        glm::ivec3 endIndices( glm::min( imax + glm::ivec3(VOXEL_BORDER), mGridSize-glm::ivec3(1) ) );
#endif

        // Rasterize triangle
        for (int i=startIndices.x; i<=endIndices.x; i++) {
            for (int j=startIndices.y; j<=endIndices.y; j++) {
                for (int k=startIndices.z; k<=endIndices.z; k++){
                    if ( inVoxel(i, j, k, trianglePlane) ) {
                         ptrVoxel *theVoxel = &(mVoxelGrid[ index(i, j, k) ]);
                         if (*theVoxel==NULL) {
                             *theVoxel=new DFVoxel(mDistanceFieldBox.getMin() + glm::vec3( (i+0.5f), (j+0.5f), (k+0.5f) )*mGridStep);
                         }
                         (*theVoxel)->addTriangle(t, triangleVertices, trianglePlane, triangleEdges);
                    }
                }
            }
        }
    }

    tbuild.stop();
    std::cerr << "Building grid done : " << tbuild.value() << std::endl;
    /* prepare to draw ... (just to debug) ... */
    mNumDrawIndices = 0;
    for (int i=0; i<mGridSize.x; i++) {
        for (int j=0; j<mGridSize.y; j++) {
            for (int k=0; k<mGridSize.z; k++){
                 ptrVoxel theVoxel(mVoxelGrid[ index(i, j, k) ]);
                 if (theVoxel!=NULL) {
                        Mesh::VertexData toDraw;

                        toDraw.mVertex = theVoxel->point; //mDistanceFieldBox.getMin() + glm::vec3( (i+0.5f), (j+0.5f), (k+0.5f) )*mGridStep;
                        toDraw.mNormal = theVoxel->offset;
                        toDraw.mTexCoord = glm::vec4(0.f);
                        mDrawVertices.push_back(toDraw);
                        mDrawIndices.push_back(mNumDrawIndices++);

                        toDraw.mVertex = theVoxel->point + theVoxel->offset; //mDistanceFieldBox.getMin() + glm::vec3( (i+0.5f), (j+0.5f), (k+0.5f) )*mGridStep;
                        toDraw.mNormal = theVoxel->offset;
                        toDraw.mTexCoord = glm::vec4(theVoxel->offset, 1.f);
                        mDrawVertices.push_back(toDraw);
                        mDrawIndices.push_back(mNumDrawIndices++);
                 }
            }
        }
    }
    mNumDrawVertices = mDrawVertices.size();
    init();
    std::cerr << "Init draw done." << std::endl;
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
void DistanceField::init()
{

    glAssert(glGenVertexArrays(1, &mVertexArrayObject));

    // bind vertex Array
    glAssert(glBindVertexArray(mVertexArrayObject));

    // always generate all buffers : one for vertexdata one for indices
    glAssert(glGenBuffers(2, mVertexBufferObjects));

    // bind vertexdata
    glAssert(glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjects[VBO_VERTICES]));
    glAssert(glBufferData(GL_ARRAY_BUFFER, mNumDrawVertices * sizeof(Mesh::VertexData),  &(mDrawVertices[0]), GL_STATIC_DRAW));

    // global values
    GLuint stride = sizeof(Mesh::VertexData);
    GLboolean normalized = GL_FALSE;
    GLenum type = GL_FLOAT;

    // mVertex
    GLuint index = 0;
    GLint size = 3;
    GLvoid *pointer = BUFFER_OFFSET(0);

    glAssert(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
    glAssert(glEnableVertexAttribArray(index));

    // mNormal
    index = 1;
    size = 3;
    pointer = BUFFER_OFFSET(sizeof(glm::vec3));

    glAssert(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
    glAssert(glEnableVertexAttribArray(index));

    // mTangente
    index = 2;
    size = 3;
    pointer = BUFFER_OFFSET(2 * sizeof(glm::vec3));

    glAssert(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
    glAssert(glEnableVertexAttribArray(index));

    // mTexCoord
    index = 3;
    size = 4;
    pointer = BUFFER_OFFSET(3 * sizeof(glm::vec3));

    glAssert(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
    glAssert(glEnableVertexAttribArray(index));

    // bind index data
    glAssert(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVertexBufferObjects[VBO_INDICES]));
    glAssert(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumDrawIndices * sizeof(int),   &(mDrawIndices[0]), GL_STATIC_DRAW));
}

void DistanceField::draw(){
    //glPointSize(3.f);
    glAssert(glBindVertexArray(mVertexArrayObject));

    // draw count elements in indices
    GLenum type = GL_UNSIGNED_INT;
    GLenum mode = GL_LINES;
    GLint count = mNumDrawIndices;
    void *indices = NULL;
    glAssert(glDrawElements(mode, count, type, indices));
    //glPointSize(1.f);

}

/* Add the mesh to the distance field structure */
void DistanceField::addMesh(Mesh *theMesh, Material *theMaterial, const glm::mat4 &matrix){
    const Mesh::VertexData *vertices =  theMesh->vertices();
    int nbVertices = theMesh->numVertices();
    const int *indices = theMesh->indices();
    int nbIndices = theMesh->numIndices();
    int vertexOffset = mVertices.size();
    // add vertices
    for (int i=0; i<nbVertices; i++) {
        Mesh::VertexData toInsert(vertices[i], matrix);
        mDistanceFieldBox += toInsert.mVertex;
        mVertices.push_back(toInsert);
    }
    // add triangles
    for (int i=0; i<nbIndices; i++)
        mIndices.push_back(indices[i]+vertexOffset);
}

Mesh::VertexData DistanceField::findNearest(const glm::vec3 &which){
    Mesh::VertexData v;
    return v;
}


}
