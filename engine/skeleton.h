/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "opengl.h"

namespace vortex {

/**
 * Skeleton graph definition
 */
class SkeletonGraph {

public:

    class Node {
    public:
        /**
         * Constructor
         * @param name Name of node
         * @param parent Parent reference
         */
        Node(std::string name, Node *parent);
        ~Node();

        /**
         * @return the node name
         */
        const std::string &name() const {
            return mName;
        }

        /**
         * @return the node parent
         */
        Node* parent() const {
            return mParent;
        }

        /**************** Childs ****************/
        /**
         * @return the number of child nodes
         */
        unsigned int nChilds() const {
            return mNumChilds;
        }

        /**
         * @param i Index of a child node
         * @return The node as corresponding index
         */
        Node* child(unsigned int i) {
            return mChilds[i];
        }

        /**
         * Add un child to the child array
         * @param node Node to add
         */
        void addChild(Node* node) {
            mChilds.push_back(node);
            ++mNumChilds;
        }

        /**************** Global transform matrix ****************/
        /**
         * Set the global matrix of the node
         * @param global The global matrix
         */
        void setGlobalTransform(const glm::mat4x4 &global) {
            mGlobalTransform = global;
        }

        /**
         * @return The global matrix
         */
        glm::mat4x4 &globalTransform() {
            return mGlobalTransform;
        }

        /**************** Local transform matrix ****************/
        /**
         * Set the local matrix of the node
         * @param global The local matrix
         */
        void setLocalTransform(const glm::mat4x4 &local) {
            mLocalTransform = local;
        }

        void setBindPoseTransform(const glm::mat4x4 &local) {
            mLocalTransform = local;
            mBindTransform = local;
        }

        /**
         * @return The local matrix
         */
        glm::mat4x4 &localTransform() {
            return mLocalTransform;
        }

        /**************** Offset transform matrix ****************/
        /**
         * Set the offset matrix of the node
         * @param global The offset matrix
         */
        void setOffsetTransform(const glm::mat4x4 &offset) {
            mOffsetTransform = offset;
        }

        /**
         * @return The offset matrix
         */
        glm::mat4x4 & offsetTransform() {
            return mOffsetTransform;
        }

        /**
         * @return The offset matrix
         */
        glm::mat4x4 & bindTransform() {
            return mBindTransform;
        }


    private:
        std::string mName;

        Node *mParent;

        unsigned int mNumChilds;
        std::vector<Node*> mChilds;

        glm::mat4x4 mGlobalTransform;
        glm::mat4x4 mLocalTransform;
        glm::mat4x4 mBindTransform;
        glm::mat4x4 mOffsetTransform;
    };

public:
//    void addBonesToMap(std::string name, SkeletonGraph::Node *node) { mBonesByName[name] = node; }
//    SkeletonGraph::Node* getBoneByName(std::string name) {
//        std::map<std::string, SkeletonGraph::Node *>::iterator it = mBonesByName.find(name);
//        if(it != mBonesByName.end()) {
//            return it->second;
//        } else {
//            return NULL;
//        }
//    }

    SkeletonGraph(Node* rootNode);
    ~SkeletonGraph();

    /**
     * @return The root of tree bones
     */
    Node* getRoot() {
        // DEBUG
        // TODO : correct getRoot()
        SkeletonGraph::Node* root = mRootNode;//?????????????? mRootNode->child(0);
        // DEBUG
        return root;
    }

    /**
     * Build the bones map
     * Name of bone -> Reference in the tree bones (the skeleton)
     * @param node Initially the root of tree
     */
    void buildBonesMap(Node *node);

    SkeletonGraph::Node* getBone(const std::string &name) {
       std::map<std::string, SkeletonGraph::Node *>::iterator it =mBonesByName.find(name);
       if(it != mBonesByName.end()) {
           return it->second;
       } else {
           return NULL;
       }
    }

    /**
     * Draw a the skeleton of an animated mesh
     */
    void drawSkeleton();

    /**
     * Print skeleton hierarchy
     * @param node The root
     * @param level 0 for the root
     */
    void printSkeleton(SkeletonGraph::Node *node, unsigned int level = 0);

    /**
     * Display a message follow by a mat4x4
     * @param mat Matrix to print
     * @param info Message to print
     */
    static void debugDisplayMatrix(const glm::mat4x4 &mat, const std::string &info = NULL);


    /**
     * @return True if the skeleton display is actived
     */
    bool isSkeletonDisplay() {
        return mDisplaySkeleton;
    }

    /**
     * Enable/Disable the skeleton display
     */
    void setSkeletonDisplay(bool enable) {
        mDisplaySkeleton = enable;
    }
private:

    // Root of skeleton
    Node* mRootNode;

    // Map of bones
    std::map<std::string, SkeletonGraph::Node *> mBonesByName;

    // to draw skeleton or not
    bool mDisplaySkeleton;
};

}
#endif // SKELETON_H
