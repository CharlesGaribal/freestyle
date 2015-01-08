/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "skeleton.h"


namespace vortex {

/* SkeletonGraph */
SkeletonGraph::SkeletonGraph(Node* rootNode) :
    mRootNode(rootNode),
    mDisplaySkeleton(false)
{
    // Build map
    //! The tree (skeleton) MUST BE build before this object
    //buildBonesMap(rootNode);
}

SkeletonGraph::~SkeletonGraph() { }

/* Node */
SkeletonGraph::Node::Node(std::string name, Node *parent = NULL) :
    mName(name),
    mParent(parent),
    mNumChilds(0),
    mChilds()
{
    // Default values identity
    mGlobalTransform = glm::mat4(1.f);
    mLocalTransform = glm::mat4(1.f);
    mOffsetTransform = glm::mat4(1.f);
}

SkeletonGraph::Node::~Node() {
    for (unsigned int i = 0; i < mNumChilds; ++i) {
        delete mChilds[i];
    }
}

/***************************************************/

void SkeletonGraph::buildBonesMap(Node* node) {
    // Add bone to map
    mBonesByName[node->name()] = node;

    // Recursive
    for (unsigned int i = 0; i < node->nChilds(); ++i) {
        buildBonesMap(node->child(i));
    }
}

void SkeletonGraph::drawSkeleton() {

    if(mDisplaySkeleton) {

        /*
         * First, we list all bsetLocalTransformones an her parents
         */
        std::vector<glm::vec3> bonesList;
        for (std::map<std::string, SkeletonGraph::Node *>::iterator it = mBonesByName.begin(); it != mBonesByName.end(); ++it) {
            SkeletonGraph::Node* node = it->second;

            // get the translation only
            glm::mat4x4 global = node->globalTransform();
            glm::vec3 vertex = glm::vec3(global[3][0],
                                         global[3][1],
                                         global[3][2]);
            // add to vector
            bonesList.push_back(vertex);

            // same for parent...
            if(node->parent() != NULL) {
                global = node->parent()->globalTransform();
                vertex = glm::vec3(global[3][0],
                                   global[3][1],
                                   global[3][2]);

            } else {
                vertex = glm::vec3(100, 100, 100);
            }

            bonesList.push_back(vertex);
        }

        /*
         * Second, we draw skeleton
         */
        GLuint VAO;
        GLuint VBO;
        int nbVertex = bonesList.size();

        glAssert( glGenVertexArrays(1, &VAO) );
        glAssert( glBindVertexArray(VAO) );

        glAssert( glGenBuffers(1, &VBO) );
        glAssert( glBindBuffer(GL_ARRAY_BUFFER, VBO) );

        // copy of vertex on gpu memory
        glAssert( glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * nbVertex, &(bonesList[0]), GL_STATIC_DRAW) );

        /* Draw points */
        glAssert( glVertexAttribPointer(0/*attr idx*/,
                                        3/*nb_components*/,
                                        GL_FLOAT,
                                        false/*normalized*/,
                                        sizeof(glm::vec3) * 2/*stride*/,
                                        0/*offset*/) );

        glAssert( glEnableVertexAttribArray(0) );

        glAssert( glPointSize(10.) );

        // draw points on vertex positions
        glAssert( glDrawArrays(GL_POINTS, 0, nbVertex) );

        /* Draw lines */
        glAssert( glVertexAttribPointer(0/*attr idx*/,
                                        3/*nb_components*/,
                                        GL_FLOAT,
                                        false/*normalized*/,
                                        sizeof(glm::vec3)/*stride*/,
                                        0/*offset*/) );

        glAssert( glEnableVertexAttribArray(0) );

        // draw line between parents & childs
        glAssert( glDrawArrays(GL_LINES, 0, nbVertex) );

        // free
        glAssert( glBindVertexArray(0) );
        glAssert( glDeleteBuffers(1, &VBO) );
        glAssert( glDeleteVertexArrays(1, &VAO) );

    }
}

void SkeletonGraph::printSkeleton(SkeletonGraph::Node *node, unsigned int level) {
    std::string nodeInfo;
    ++level;

    for (unsigned int i = 1; i < level+1; ++i) {
        nodeInfo.append("--");
    }

    nodeInfo.append(">");
    nodeInfo.append(node->name());

    std::cerr << nodeInfo << std::endl;

    for (unsigned int i = 0; i < node->nChilds(); ++i) {
        printSkeleton(node->child(i), level);
    }
}

/**
 * Display a mat4x4 with a debug message
 */
void SkeletonGraph::debugDisplayMatrix(const glm::mat4x4 &mat, const std::string &info) {
    if(!info.empty()) {
        std::cerr << info << std::endl;
    }

    std::cerr
        << mat[0][0] << "\t" << mat[0][1] << "\t" << mat[0][2] << "\t" << mat[0][3] << "\n"
        << mat[1][0] << "\t" << mat[1][1] << "\t" << mat[1][2] << "\t" << mat[1][3] << "\n"
        << mat[2][0] << "\t" << mat[2][1] << "\t" << mat[2][2] << "\t" << mat[2][3] << "\n"
        << mat[3][0] << "\t" << mat[3][1] << "\t" << mat[3][2] << "\t" << mat[3][3] << "\n";
}

} // namespace


