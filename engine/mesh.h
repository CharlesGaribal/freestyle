/*
*   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
*   Mathias.Paulin@irit.fr
*   vdh@irit.fr
*/

#ifndef MESH_H
#define MESH_H
#include <string>

#include "opengl.h"

#include "bbox.h"
#include "drawable.h"


namespace vortex {

/**
 * Mesh representation class.
 *
 */
class Mesh : public Drawable {
public:

    /**
     * Vertice structure representation.
     *
     */
    struct VertexData {
        glm::vec3 mVertex;
        glm::vec3 mNormal;
        glm::vec3 mTangent;
        glm::vec4 mTexCoord;

        VertexData(const VertexData &from, const glm::mat4 &matrix);
        VertexData(const VertexData &from);
        VertexData(){}
    };

    /**
     * Mesh pointer definition.
     *
     */
    typedef Mesh * MeshPtr;

    /**
     * Constructor : initialize Mesh attributes.
     *
     * @param name The name of the Mesh Object.
     */
    Mesh(std::string name);

    /**
     * Constructor, pointed data are replicated in the constructed object.
     *
     * @param name The name of the Mesh Object.
     * @param vertices Pointer to the vertices structures defining the mesh points.
     * @param numVertices Number of vertices structures in "vertices".
     * @param indices Pointer to the vertices structures indices that define the triangular faces of the mesh.
     * @param numIndices Number of indices in "indices".
     */
    Mesh(std::string name, const VertexData *vertices, int numVertices, const int *indices, int numIndices);

    /**
     * Reinitialize, pointed data are replicated in the constructed object.
     *
     * @param name The name of the Mesh Object.
     * @param vertices Pointer to the vertices structures defining the mesh points.
     * @param numVertices Number of vertices structures in "vertices".
     * @param indices Pointer to the vertices structures indices that define the triangular faces of the mesh.
     * @param numIndices Number of indices in "indices".
     */

    void setData(std::string name, const VertexData *vertices, int numVertices, const int *indices, int numIndices);
    /**
     * @brief copy from another mesh, all data is allocated and copied
     * @param m
     */
    void copy(const Mesh *m);

    virtual ~Mesh();

    /**
     *  Mesh OpenGL deletion : delete VBOs.
     *
     */
    void release();

    /**
     * Mesh OpenGL creation : create VBOs.
     *
     */
    void init();

    /**
     * Mesh drawing method : draw the VBOs.
     *
     */
    virtual void draw();
    virtual void drawPatch(int patchSize = 3);

    //! draw but use lines mode
    virtual void drawLines();

    /**
     * @return The Mesh Bounding Box
     *
     */
    BBox boundingBox() {
        return mBbox;
    }

    /**
     * @return The Mesh Object name.
     *
     */
    std::string name() const {
        return mName;
    }

    /**
     * Mesh Bounding Box drawing method : draw the BBox edges.
     *
     */
    void drawBbox();

    /**
     * Secify the Mesh has been selected for edition.
     *
     */
    void setSelected(bool selected) {
        mSelected = selected;
    }

    /**
     * Specify the selected face for face edition
     */
    void setSelectedFace(int faceId) {
        mSelectedFaceId = faceId;
    }

    /**
     * Specify the selected vertice for vertice edition
     */
    void setSelectedFace(int faceId, int verticeId) {
        mSelectedVerticeId = mIndices[ 3*faceId + verticeId-1 ];
    }

    /**
     * Specific draw of the selected face
     */
    void drawSelectedFace();

    /**
     * Specific draw of the selected vertice
     */
    void drawSelectedVertice();

    void drawLineStrip();

    bool mSelected;
    int mSelectedFaceId;
    int mSelectedVerticeId;

    /**
      * Update mesh vertices (for animated meshes
      * @todo modify to give time parameter ...
      */
    virtual void update() {}

    virtual void reset();

    bool isSelected() const {return mSelected;}
    int numIndices() const { return mNumIndices; }
    int numVertices() const { return mNumVertices;}
    const VertexData * const vertices() const {return mVertices;}
    const int * const indices() const {return mIndices;}


    int meshId() const;
    void setMeshId(int meshId);


    glm::mat4x4 getTransformMatrix() const;
    void setTransformMatrix(const glm::mat4x4 &value);

protected :
    std::string mName;
    // http://www.opengl.org/wiki/Vertex_Buffer_Object

    int mNumVertices;
    VertexData *mVertices;

    // faces are all triangles, there are nNumIndices/3 faces
    int mNumIndices;
    int *mIndices;

    glm::mat4x4 mTransformMatrix;

    // OpenGL stuffs
    GLuint mVertexArrayObject;
    enum {VBO_VERTICES, VBO_INDICES};
    GLuint mVertexBufferObjects[2];

    BBox mBbox;

    int meshId_; // The mesh Id for picking : -1 if mesh not store in assetmanager

};

class MeshBuilder{
public:
    virtual Mesh *build(std::string name)=0;
};

class ScreenQuadBuilder:public MeshBuilder{
 public:
    Mesh *build(std::string name){

        Mesh::VertexData vertices[4];
        vertices[0].mVertex = glm::vec3(-1, -1, 0.);
        vertices[0].mNormal = glm::vec3(0., 0., 0.);
        vertices[0].mTangent = glm::vec3(0., 0., 0.);
        vertices[0].mTexCoord = glm::vec4(0., 0., 0., 0.);
        vertices[1].mVertex = glm::vec3(-1, 1, 0.);
        vertices[1].mNormal = glm::vec3(0., 0., 0.);
        vertices[1].mTangent = glm::vec3(0., 0., 0.);
        vertices[1].mTexCoord = glm::vec4(0., 1., 0., 0.);
        vertices[2].mVertex = glm::vec3(1, 1, 0.);
        vertices[2].mNormal = glm::vec3(0., 0., 0.);
        vertices[2].mTangent = glm::vec3(0., 0., 0.);
        vertices[2].mTexCoord = glm::vec4(1., 1., 0., 0.);
        vertices[3].mVertex = glm::vec3(1, -1, 0.);
        vertices[3].mNormal = glm::vec3(0., 0., 0.);
        vertices[3].mTangent = glm::vec3(0., 0., 0.);
        vertices[3].mTexCoord = glm::vec4(1., 0., 0., 0.);
        int indices[6] = { 0, 1, 2, 0, 2, 3 };

        return new Mesh(name, vertices, 4, indices, 6);
    }

    Mesh *buildRevert(std::string name){

        Mesh::VertexData vertices[4];
        vertices[0].mVertex = glm::vec3(-1, -1, 0.);
        vertices[0].mNormal = glm::vec3(0., 0., 0.);
        vertices[0].mTangent = glm::vec3(0., 0., 0.);
        vertices[0].mTexCoord = glm::vec4(0., 1., 0., 0.);
        vertices[1].mVertex = glm::vec3(-1, 1, 0.);
        vertices[1].mNormal = glm::vec3(0., 0., 0.);
        vertices[1].mTangent = glm::vec3(0., 0., 0.);
        vertices[1].mTexCoord = glm::vec4(0., 0., 0., 0.);
        vertices[2].mVertex = glm::vec3(1, 1, 0.);
        vertices[2].mNormal = glm::vec3(0., 0., 0.);
        vertices[2].mTangent = glm::vec3(0., 0., 0.);
        vertices[2].mTexCoord = glm::vec4(1., 0., 0., 0.);
        vertices[3].mVertex = glm::vec3(1, -1, 0.);
        vertices[3].mNormal = glm::vec3(0., 0., 0.);
        vertices[3].mTangent = glm::vec3(0., 0., 0.);
        vertices[3].mTexCoord = glm::vec4(1., 1., 0., 0.);
        int indices[6] = { 0, 1, 2, 0, 2, 3 };

        return new Mesh(name, vertices, 4, indices, 6);
    }


};
class BBoxMeshBuilder {
public:
    Mesh *build(std::string name, BBox bbox) {
        Mesh::VertexData vertices[8];
        std::vector<glm::vec3> theCorners;
        bbox.getCorners(theCorners);

        for (int i = 0 ; i < 8 ; i++)
            vertices[i].mVertex = theCorners[i];

        int indices[16] = {
            0,3,4,2,
            4,7,6,2,
            0,1,6,7,
            5,1,5,3
        };

        return new Mesh(name, vertices, 8, indices, 16);
    }
};

class BBoxLineMeshBuilder {
public:
    Mesh *build(std::string name, BBox bbox) {
        Mesh::VertexData vertices[8];
        std::vector<glm::vec3> theCorners;
        bbox.getCorners(theCorners);

        for (int i = 0 ; i < 8 ; i++)
            vertices[i].mVertex = theCorners[i];

        int indices[24] = {
            0,1,0,3,0,2,1,5,1,6,6,7,5,7,4,7,3,4,2,4,2,6,3,5
        };

        return new Mesh(name, vertices, 8, indices, 24);
    }
};


} // namespace vortex
#endif // MESH_H
