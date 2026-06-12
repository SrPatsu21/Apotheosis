#include "Animator.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stdexcept>

#include <cmath>

Animator::Animator(
    Skeleton* skeleton
)
{
    setSkeleton(skeleton);
}

void Animator::setSkeleton(
    Skeleton* skeleton
)
{
    this->skeleton = skeleton;

    if (!skeleton)
        return;

    size_t boneCount =
        skeleton->getBoneCount();

    localPose.resize(boneCount);

    globalMatrices.resize(
        boneCount,
        glm::mat4(1.0f)
    );

    pose.resize(boneCount);
}

void Animator::setAnimation(
    Animation* animation
)
{
    if (!animation)
        return;

    if (skeleton && animation->skeletonBoneCount != skeleton->bones.size())
    {
        throw std::runtime_error(
            "Animation incompatible with Skeleton"
        );
    }

    currentAnimation = animation;
    currentTime = 0.0f;
}

void Animator::reset()
{
    currentTime = 0.0f;

    for (auto& transform : localPose)
    {
        transform.translation =
            glm::vec3(0.0f);

        transform.rotation =
            glm::quat(
                1.0f,
                0.0f,
                0.0f,
                0.0f
            );

        transform.scale =
            glm::vec3(1.0f);
    }

    for (auto& matrix : globalMatrices)
    {
        matrix = glm::mat4(1.0f);
    }

    pose.reset();
}


// ***************************
// * helpers
// ***************************
template<typename T>
static T sampleLinear(
    const std::vector<KeyFrame<T>>& keys,
    float time
)
{
    if (keys.empty())
        return T{};

    if (keys.size() == 1)
        return keys[0].value;

    if (time <= keys.front().time)
        return keys.front().value;

    if (time >= keys.back().time)
        return keys.back().value;

    for (size_t i = 0; i < keys.size() - 1; i++)
    {
        const auto& a = keys[i];
        const auto& b = keys[i + 1];

        if (time >= a.time &&
            time <= b.time)
        {
            float t =
                (time - a.time)
                /
                (b.time - a.time);

            return glm::mix(
                a.value,
                b.value,
                t
            );
        }
    }

    return keys.back().value;
}

static glm::quat sampleRotation(
    const std::vector<KeyFrame<glm::quat>>& keys,
    float time
)
{
    if (keys.empty())
    {
        return glm::quat(
            1.0f,
            0.0f,
            0.0f,
            0.0f
        );
    }

    if (keys.size() == 1)
        return keys[0].value;

    if (time <= keys.front().time)
        return keys.front().value;

    if (time >= keys.back().time)
        return keys.back().value;

    for (size_t i = 0; i < keys.size() - 1; i++)
    {
        const auto& a = keys[i];
        const auto& b = keys[i + 1];

        if (time >= a.time &&
            time <= b.time)
        {
            float t =
                (time - a.time)
                /
                (b.time - a.time);

            return glm::slerp(
                a.value,
                b.value,
                t
            );
        }
    }

    return keys.back().value;
}

static glm::vec3 sampleTranslation(
    const std::vector<KeyFrame<glm::vec3>>& keys,
    float time
)
{
    return sampleLinear(keys, time);
}

static glm::vec3 sampleScale(
    const std::vector<KeyFrame<glm::vec3>>& keys,
    float time
)
{
    return sampleLinear(keys, time);
}

// ***************************
// * update
// ***************************
void Animator::update(float dt)
{
    if (!skeleton)
        return;

    if (!currentAnimation)
        return;

    const size_t boneCount =
        skeleton->getBoneCount();

    if (localPose.size() != boneCount)
    {
        localPose.resize(boneCount);
    }

    if (globalMatrices.size() != boneCount)
    {
        globalMatrices.resize(
            boneCount,
            glm::mat4(1.0f)
        );
    }

    if (pose.finalMatrices.size() != boneCount)
    {
        pose.resize(boneCount);
    }

    //--------------------------------------------------
    // Atualiza tempo
    //--------------------------------------------------

    currentTime += dt;

    if (currentAnimation->duration > 0.0f)
    {
        currentTime =
            std::fmod(
                currentTime,
                currentAnimation->duration
            );
    }

    //--------------------------------------------------
    // Começa da bind pose
    //--------------------------------------------------

    localPose = skeleton->bindPose;

    //--------------------------------------------------
    // Aplica canais animados
    //--------------------------------------------------

    for (const auto& channel : currentAnimation->channels)
    {
        const uint32_t boneIndex =
            channel.boneIndex;

        if (boneIndex >= boneCount)
            continue;

        //--------------------------------------------------
        // Translation
        //--------------------------------------------------

        if (!channel.translations.empty())
        {
            localPose[boneIndex].translation =
                sampleTranslation(
                    channel.translations,
                    currentTime
                );
        }

        //--------------------------------------------------
        // Rotation
        //--------------------------------------------------

        if (!channel.rotations.empty())
        {
            localPose[boneIndex].rotation =
                sampleRotation(
                    channel.rotations,
                    currentTime
                );
        }

        //--------------------------------------------------
        // Scale
        //--------------------------------------------------

        if (!channel.scales.empty())
        {
            localPose[boneIndex].scale =
                sampleScale(
                    channel.scales,
                    currentTime
                );
        }
    }

    //--------------------------------------------------
    // Local -> Global
    //--------------------------------------------------

    for (size_t i = 0; i < boneCount; i++)
    {
        const BoneTransform& transform = localPose[i];

        glm::mat4 T =
            glm::translate(
                glm::mat4(1.0f),
                transform.translation
            );

        glm::mat4 R =
            glm::toMat4(
                transform.rotation
            );

        glm::mat4 S =
            glm::scale(
                glm::mat4(1.0f),
                transform.scale
            );

        glm::mat4 localMatrix = T * R * S;

        const int parent =
            skeleton->bones[i]
                .parentIndex;

        if (parent < 0)
        {
            globalMatrices[i] =
                localMatrix;
        }
        else
        {
            globalMatrices[i] =
                globalMatrices[parent]
                *
                localMatrix;
        }
    }

    //--------------------------------------------------
    // Global -> Final Skinning
    //--------------------------------------------------

    for (size_t i = 0; i < boneCount; i++)
    {
        pose.finalMatrices[i] = globalMatrices[i] * skeleton->bones[i].inverseBindMatrix;
    }
}