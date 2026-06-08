#pragma once

#include "SkeletonPose.hpp"
#include "Animation.hpp"
#include "Skeleton.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

class Animator
{
public:

    Skeleton* skeleton = nullptr;

    Animation* currentAnimation = nullptr;

    SkeletonPose pose;

    float currentTime = 0.0f;

private:

    std::vector<BoneTransform> localPose;
    std::vector<glm::mat4> globalMatrices;

public:

    Animator() = default;

    explicit Animator(
        Skeleton* skeleton
    );

    void setSkeleton(
        Skeleton* skeleton
    );

    void setAnimation(
        Animation* animation
    );

    void reset();

    void update(
        float dt
    );
};