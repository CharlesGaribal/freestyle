/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#include "animation.h"
namespace vortex {


inline float clamp(float x, float a, float b)

{

    return x < a ? a : (x > b ? b : x);

}

Animation::Animation() :
    mDuration(0),
    mTicksPerSecond(0),
    mNumChannels(0),
    mChannels(),
    mSkeleton(NULL),
    mNumMeshNames(0)
{
}

Animation::Animation(std::string name, double duration, double ticksPerSecond,
                     unsigned int numChannels, std::vector<BoneAnim *> channels,
                     unsigned int numMeshNames, std::vector<std::string> names) :
    mName(name),
    mDuration(duration),
    mTicksPerSecond(ticksPerSecond),
    mNumChannels(numChannels),
    mChannels(channels),
    mSkeleton(NULL),
    mNumMeshNames(numMeshNames),
    mMeshNames(names)
{
}

/*
static void print_map(const std::map<std::string, Animation::BoneAnim *>& map)
{
    std::map<std::string, Animation::BoneAnim*>::const_iterator it = map.begin();
    for(; it != map.end(); ++it)
    {
        std::cout << "name map: " << it->first << " bone name: ";
        std::cout << it->second->mBoneName << std::endl;
    }
    std::cout << std::endl;
}
*/

/*
 * Copy feild by feild
 */
Animation::Animation(aiAnimation *animation) :
    mSkeleton(NULL)
{

    mName = std::string(animation->mName.data);
    mDuration = animation->mDuration;

    // extract ticks per second. Assume default value if not given
    mTicksPerSecond = (animation->mTicksPerSecond != 0.0) ? animation->mTicksPerSecond : 25.0;
    mDuration /= mTicksPerSecond;

    std::cerr << "Animation : " <<  mName << " -- " << mDuration << " (" << mTicksPerSecond << ")" << std::endl;

    mNumChannels = animation->mNumChannels;
    mChannels.resize(mNumChannels);
    for (unsigned int i = 0; i < mNumChannels; ++i) {

        aiNodeAnim* aiAnim = animation->mChannels[i];

        BoneAnim* boneAnim = new BoneAnim();
        boneAnim->mBoneName = std::string(aiAnim->mNodeName.data);

        // Position keys
        boneAnim->mNumPositionKeys = aiAnim->mNumPositionKeys;
        boneAnim->mPositionKeys.resize(boneAnim->mNumPositionKeys);

        for (unsigned int j = 0; j < boneAnim->mNumPositionKeys; ++j) {

            // Convertion aiVector3D to glm::vec3
            glm::vec3 posVec(aiAnim->mPositionKeys[j].mValue.x,
                             aiAnim->mPositionKeys[j].mValue.y,
                             aiAnim->mPositionKeys[j].mValue.z);

            boneAnim->mPositionKeys[j] = new VectorKey(aiAnim->mPositionKeys[j].mTime / mTicksPerSecond, posVec);
        }

        // Rotation keys
        boneAnim->mNumRotationKeys = aiAnim->mNumRotationKeys;
        boneAnim->mRotationKeys.resize(boneAnim->mNumRotationKeys);

        for (unsigned int j = 0; j < boneAnim->mNumRotationKeys; ++j) {

            // Convertion aiQuatKey to local glm::quat representation

            glm::quat quat(aiAnim->mRotationKeys[j].mValue.w,
                           aiAnim->mRotationKeys[j].mValue.x,
                            aiAnim->mRotationKeys[j].mValue.y,
                            aiAnim->mRotationKeys[j].mValue.z
                            );

            boneAnim->mRotationKeys[j] = new QuatKey(aiAnim->mRotationKeys[j].mTime / mTicksPerSecond, quat);
        }

        // Scaling keys
        boneAnim->mNumScalingKeys = aiAnim->mNumScalingKeys;
        boneAnim->mScalingKeys.resize(boneAnim->mNumScalingKeys);

        for (unsigned int j = 0; j < boneAnim->mNumScalingKeys; ++j) {

            // Convertion aiVector3D to glm::vec3
            glm::vec3 posVec(aiAnim->mScalingKeys[j].mValue.x,
                             aiAnim->mScalingKeys[j].mValue.y,
                             aiAnim->mScalingKeys[j].mValue.z);

            boneAnim->mScalingKeys[j] = new VectorKey(aiAnim->mScalingKeys[j].mTime / mTicksPerSecond, posVec);
        }

        // Add the BoneAnim to vector
        mChannels[i] = boneAnim;

        // Add to map
        //TODO Check if any animation is UNIQUE
        mAnimByBoneName[boneAnim->mBoneName] = boneAnim;
    }

    // list of mesh anim name
    mNumMeshNames = animation->mNumMeshChannels;
    mMeshNames.resize(mNumMeshNames);
    for (unsigned int i = 0; i < mNumMeshNames; ++i) {
        mMeshNames[i] = std::string(animation->mMeshChannels[i]->mName.data);
    }

}

Animation::~Animation() { }


/*******************************************************/

void Animation::update(double time) {
    if (time >=0) {
        mCurrentTime = time;
        updateSkeletonTransform(mSkeleton->getRoot());
    } else {
        std::cerr << "Set at bind pose" << std::endl;
        mCurrentTime = 0.0;
        setAtBindPose(mSkeleton->getRoot());
    }
}

/**
 * Update global transformation matrix
 */
void Animation::updateSkeletonTransform(SkeletonGraph::Node *node) {
    /*
     * Update current node
     */
    // Calcul local transform
    std::map<std::string, BoneAnim *>::iterator it = mAnimByBoneName.find(node->name());
    if(it != mAnimByBoneName.end()) { // Node found

        // Calcul local transform matrix with animation datas
        // and update the matrix in the node...
        node->setLocalTransform(computeLocalTransform(it->second));
    }

    // Calcul global transform
    if(mSkeleton->getRoot() != node) {
        // G * L (G: parent global, L: Child local)
        node->setGlobalTransform( node->parent()->globalTransform() * node->localTransform() );

    } else {
        // Root node
        // global == local
        node->setGlobalTransform(node->localTransform());
    }

    /*
     * Recursive update
     */
    for(unsigned int i = 0; i < node->nChilds(); ++i) {
        updateSkeletonTransform(node->child(i));
    }
}

/**
 * Update global transformation matrix
 */
void Animation::setAtBindPose(SkeletonGraph::Node *node) {
    /*
     * Update current node
     */
    // Calcul local transform
    std::map<std::string, BoneAnim *>::iterator it = mAnimByBoneName.find(node->name());
    if(it != mAnimByBoneName.end()) { // Node found

        // Calcul local transform matrix with animation datas
        // and update the matrix in the node...
        node->setLocalTransform(node->bindTransform());
    }

    // Calcul global transform
    if(mSkeleton->getRoot() != node) {
        // G * L (G: parent global, L: Child local)
        node->setGlobalTransform(node->parent()->globalTransform() * node->localTransform());

    } else {
        // Root node
        // global == local
        node->setGlobalTransform(node->localTransform());
    }

    /*
     * Recursive update
     */
    for(unsigned int i = 0; i < node->nChilds(); ++i) {
        setAtBindPose(node->child(i));
    }
}

/**
 * Calcul local tranformation matrix of a bone
 */
glm::mat4x4 Animation::computeLocalTransform(BoneAnim *channel) {

    // Calcul du numéro de la frame
    unsigned int frame = 0; // Frame dans l'animation

    float timeRemains=0.0;
#define TIME
#ifdef TIME
    float time = mCurrentTime;
    if(mDuration > 0.0)
        time = fmod(time, mDuration);
#endif

    //@TODO Interpolation des position

    /******* Rotation *******/

#ifdef TIME
    //@TODO cleanup clamp, timeRemains should stay between 0 1 but if frame = 0 there is an hitch -> mTime for first frame is not necessary 0.
    frame = 0;
    while( frame < channel->mNumRotationKeys - 1)
    {
        if( time < channel->mRotationKeys[frame+1]->mTime)
            break;
        frame++;
    }
    timeRemains = (channel->mRotationKeys[(frame+1)%channel->mNumRotationKeys]->mTime - time) /
                  (channel->mRotationKeys[(frame+1)%channel->mNumRotationKeys]->mTime - channel->mRotationKeys[frame]->mTime);
    timeRemains = clamp(timeRemains, 0.f, 1.f);
#endif

    glm::quat q0;
    glm::quat q1;
    q0 = channel->mRotationKeys[frame]->mValue;
    q1 = channel->mRotationKeys[(frame+1)%channel->mNumRotationKeys]->mValue;
    glm::quat q = glm::slerp(q0, q1, 1.f-timeRemains);

    glm::mat3x3 kRotation;
    kRotation = glm::mat3_cast(q);
    glm::mat4x4 rotationMatrix(kRotation);

    /******* Position *******/
#ifdef TIME
    frame = 0;
    while( frame < channel->mNumPositionKeys - 1)
    {
        if( time < channel->mPositionKeys[frame+1]->mTime)
            break;
        frame++;
    }
    timeRemains = (channel->mPositionKeys[(frame+1)%channel->mNumPositionKeys]->mTime - time) /
                  (channel->mPositionKeys[(frame+1)%channel->mNumPositionKeys]->mTime - channel->mPositionKeys[frame]->mTime);
    timeRemains = clamp(timeRemains, 0.f, 1.f);
#endif
    glm::vec3 p0 = channel->mPositionKeys[frame]->mValue;
    glm::vec3 p1 = channel->mPositionKeys[(frame+1)%channel->mNumPositionKeys]->mValue;
    glm::vec3 p = timeRemains*p0 + (1.0f-timeRemains)*p1;
    //p=p0;
    glm::mat4x4 positionMatrix = glm::translate(glm::mat4(1.f), p );

    /******* Scale *******/
#ifdef TIME
    frame = 0;
    while( frame < channel->mNumScalingKeys - 1)
    {
        if( time < channel->mScalingKeys[frame+1]->mTime)
            break;
        frame++;
    }
    timeRemains = (channel->mScalingKeys[(frame+1)%channel->mNumScalingKeys]->mTime - time) /
                  (channel->mScalingKeys[(frame+1)%channel->mNumScalingKeys]->mTime - channel->mScalingKeys[frame]->mTime);
    timeRemains = clamp(timeRemains, 0.f, 1.f);
#endif
    // TODO : try logarithmic interpolation
    glm::vec3 s0 = channel->mScalingKeys[frame]->mValue;
    glm::vec3 s1 = channel->mScalingKeys[(frame+1)%channel->mNumScalingKeys]->mValue;
    glm::vec3 s = timeRemains*s0 + (1.0f-timeRemains)*s1; // channel->mPositionKeys[frame]->mValue

    //s = s0;
    glm::mat4x4 scaleMatrix = glm::scale(glm::mat4(1.f), s);

    // Calcule de la matrice de transformation
    glm::mat4x4 transformMatrix = positionMatrix * rotationMatrix * scaleMatrix; // MATHIAS


    return transformMatrix;
}

} // namespace vortex
