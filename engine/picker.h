/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef PICKER_H
#define PICKER_H
#include "scenegraph.h"
#include "assetmanager.h"
#include "renderloop.h"
#include "fbo.h"

namespace vortex {

/*******************************************************************************************
 * Rendering loop specialisation for selection mode rendering
 * NOTE : I've inverted transform and meterial due to the visualisation of several traces :
 *  Several objects may share the same material but differ by their transformation
 * TODO : do the same inversion on the general rendering loops (quite easy to do)
 *******************************************************************************************/

/**
 * @brief The IdentifiableMesh class
 * This class is used in the selection mode rendering loop and bind the meshId to the currently used shader before drawing the mesh
 */
class IdentifiableMesh : Drawable {
    Mesh *themesh_;
    ShaderProgram *shader_;
public :
    IdentifiableMesh(): Drawable(), themesh_(NULL), shader_(NULL){}
    IdentifiableMesh(Mesh *themesh, ShaderProgram *shader): Drawable(), themesh_(themesh), shader_(shader){}
    ~IdentifiableMesh(){}
    void draw();
};

/**
 * @brief The IdentifiableMaterialState class
 * This class is used in the selection mode rendering loop and bind the materialId to the currently used shader before binding the material
 * @todo extend this to use configurations so that objects might be alpha-textured
 */
class IdentifiableMaterialState : public Bindable {
public:
    IdentifiableMaterialState(Material *mat, ShaderProgram *shader);
    /**
      * Bind the IdentifiableMaterialState to the current rendering pipeline.
      */
    void bind() const;

    /**
      * Bind a Material to the shader shader.
      */
    static void bind(Material *mat, ShaderProgram * shader) ;

    /**
      * Strict order on IdentifiableMaterialState.
      */
    bool operator< (const IdentifiableMaterialState &other) const;

private:
    Material *mMaterial;
    ShaderProgram *mShader;
};

/**
  *  Draw identified meshes ordered by transformation
  *
  */
typedef DrawableMap< TransformState, DrawableVector<IdentifiableMesh> > MeshSelectionLoop;


/**
  *  Draw MeshSelectionLoop ordered by material
  */
typedef DrawableMap< IdentifiableMaterialState, MeshSelectionLoop > SelectionLoop;

template<>
void DrawableMap<IdentifiableMaterialState, MeshSelectionLoop>::draw(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix);

template<>
void DrawableMap<TransformState, DrawableVector<IdentifiableMesh>>::draw(const glm::mat4x4 &modelviewMatrix, const glm::mat4x4 &projectionMatrix);


/**
 * @brief The SelectionLoopBuilder class
 * This visitor will be used to construct a selection mode rendering loop
 * Such a loop does not impose a shader but only the interface for this shader : (set of uniform values)
 *
 */
class SelectionLoopBuilder : public SceneGraph::VisitorOperation {
public:
    SelectionLoopBuilder(AssetManager *resourcesManager, SceneGraph *sceneGraph, SelectionLoop *loop, ShaderProgram *shader);

    void operator()(SceneGraph::Node *theNode, const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix);
private:
    AssetManager *mResourcesManager;
    SceneGraph *mSceneGraph;
    SelectionLoop *mLoop;
    ShaderProgram *mShader;
};


/**
 * @brief The Picker class
 * This class allow to select a material, a mesh and one face of this mesh from the asset.
 * The used ids are indices of the property selected in the asset. (see mesh.h, material.h and assetmanager
 * After running the select() method, one might acces to these indices using the selectedmaterial(), selectedmesh() or selectedface() methods.
 */
class Picker {
public:
    Picker(vortex::AssetManager *assetmanager, vortex::SceneGraph * scenegraph, int w, int h);
    ~Picker();

    void setPickingViewport(int w, int h);

    int select(const glm::mat4x4 &modelViewMatrix, const glm::mat4x4 &projectionMatrix, int x, int y);

    void updateSelectionLoop();

    vortex::AssetManager *assetmanager() const;
    void setAssetmanager(vortex::AssetManager *assetmanager);

    vortex::SceneGraph *scenegraph() const;
    void setScenegraph(vortex::SceneGraph *scenegraph);

    int width() const;
    void setWidth(int width);

    int height() const;
    void setHeight(int height);

    int selectedmesh() const;
    void setSelectedmesh(int selectedmesh);

    int selectedface() const;
    void setSelectedface(int selectedface);

    int selectedmaterial() const;
    void setSelectedmaterial(int selectedmaterial);

    vortex::Texture  *getTexture(){return debugTexture_;}

private:
    vortex::Texture *selectionTexture_;
    vortex::Texture *debugTexture_;
    vortex::FBO *selectionFBO_;
    ShaderProgram *shader_;

    SelectionLoop *selectionloop_;

    vortex::AssetManager *assetmanager_;
    vortex::SceneGraph * scenegraph_;
    int width_;
    int height_;

    int selectedmesh_;
    int selectedface_;
    int selectedmaterial_;

};


} // namespace vortex


#endif
