/*
 *   Copyright (C) 2008-2013 by Mathias Paulin, David Vanderhaeghe
 *   Mathias.Paulin@irit.fr
 *   vdh@irit.fr
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <vector>
#include <map>

#include "opengl.h"

#include "assimp/anim.h"

#include <glm/gtx/quaternion.hpp>

#include "skeleton.h"

namespace vortex {

class Animation {

public:

    struct VectorKey {
        double mTime;
        glm::vec3 mValue;

        // Default constructor
        VectorKey() { }

        // Construction from a given time and key value
        VectorKey(double time, const glm::vec3 &value) :
            mTime(time),
            mValue(value)
        { }
    };

    struct QuatKey {
        double mTime;
        glm::quat mValue;

        // Default constructor
        QuatKey() { }

        // Construction from a given time and key value
        QuatKey(double time, const glm::quat &value) :
            mTime(time),
            mValue(value)
        { }
    };

    /**
     * Struct of an animation attached to a bone
     */
    struct BoneAnim {
        std::string mBoneName;

        unsigned int mNumPositionKeys;
        std::vector<VectorKey *> mPositionKeys;

        unsigned int mNumRotationKeys;
        std::vector<QuatKey *> mRotationKeys;

        unsigned int mNumScalingKeys;
        std::vector<VectorKey *> mScalingKeys;

        // Default constructors
        BoneAnim() : mNumPositionKeys(0), mNumRotationKeys(0), mNumScalingKeys(0) { }
        ~BoneAnim() { }
    };

    // Default constructors
    Animation();
    Animation(std::string name, double duration, double ticksPerSecond,
              unsigned int numChannels, std::vector<BoneAnim *> channels,
              unsigned int numMeshNames, std::vector<std::string> names);
    /**
     * Constructor from aiAnimation structure
     * @param animation An aiAnimation
     */
    Animation(aiAnimation *animation);
    ~Animation();

    /**
     * @return The name of the animation
     */
    const std::string &name() {
        return mName;
    }

    /**
     * @return The duration of animation in ticks
     */
    double duration() const {
        return mDuration;
    }

    /**
     * @return The number of ticks per second
     */
    double ticksPerSecond() const {
        return mTicksPerSecond;
    }

    /**
     * @return The number of channels
     */
    unsigned int nChannels() const {
        return mNumChannels;
    }

    /**
     * Get a specific channel
     * @param index Index of channel in array
     * @return The channel selected
     */
    BoneAnim *getChannel(unsigned int index) {
        return mChannels[index];
    }

    /**
     * Set the skeleton of the animation
     * @param skeleton The skeleton
     */
    void setSkeleton(SkeletonGraph *skeleton) {
        mSkeleton = skeleton;
    }

    /**
     * @return The skeleton root
     */
    SkeletonGraph::Node* getSkeletonRoot() {
        return mSkeleton->getRoot();
    }

    /**
     * @return The skeleton
     */
    SkeletonGraph* getSkeleton() {
        return mSkeleton;
    }

    /**
     * Return an animation attached to a bone
     * @param boneName Bone wich attache to the animation
     * @return Animation attached to the bone
     */
    BoneAnim* getAnimByBoneName(std::string boneName) {
        std::map<std::string, BoneAnim *>::iterator it = mAnimByBoneName.find(boneName);
        if(it != mAnimByBoneName.end()) {
            return it->second;
        } else {
            return NULL;
        }
    }

    /**
     * Update the animation
     * @param time The current time in second
     */
    void update(double time);

private:

    void updateSkeletonTransform(SkeletonGraph::Node *node);
    void setAtBindPose(SkeletonGraph::Node *node);

    glm::mat4x4 computeLocalTransform(BoneAnim *channel);

    // Name of animation (may be empty)
    std::string mName;

    // Duration of animation in ticks
    double mDuration;

    // Ticks per seconde (may be 0)
    double mTicksPerSecond;

    // Current time
    double mCurrentTime;

    // Array of bone animation channels
    unsigned int mNumChannels;
    std::vector<BoneAnim *> mChannels;

    // Skeleton of animation
    SkeletonGraph *mSkeleton;

    // Map of anim by bone name
    std::map<std::string, BoneAnim *> mAnimByBoneName;

    // list of name mesh
    unsigned int mNumMeshNames;
    std::vector<std::string> mMeshNames;

};

} // namespace vortex

#endif // ANIMATION_H
