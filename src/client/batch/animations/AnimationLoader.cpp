#include "AnimationLoader.hpp"

std::vector<Animation> AnimationLoader::loadFromGLTF(
    const std::string& path,
    const Skeleton* skeleton
)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate
    );

    if (!scene)
    {
        throw std::runtime_error(
            importer.GetErrorString()
        );
    }

    std::unordered_map<
        std::string,
        uint32_t
    > boneMap;

    for (uint32_t i = 0; i < skeleton->bones.size(); i++)
    {
        boneMap[
            skeleton->bones[i].name
        ] = i;
    }

    std::vector<Animation> animations;

    for (unsigned int animIndex = 0; animIndex < scene->mNumAnimations; animIndex++)
    {
        aiAnimation* aiAnim = scene->mAnimations[animIndex];

        Animation animation;

        animation.name = aiAnim->mName.C_Str();

        animation.duration =
            static_cast<float>(
                aiAnim->mDuration /
                aiAnim->mTicksPerSecond
            );
        
        animation.skeletonBoneCount =
            static_cast<uint32_t>(
                skeleton->getBoneCount()
            );

        animation.channels.resize(
            skeleton->getBoneCount()
        );

        for (unsigned int c = 0; c < aiAnim->mNumChannels; c++)
        {
            aiNodeAnim* node =
                aiAnim->mChannels[c];

            auto it =
                boneMap.find(
                    node->mNodeName.C_Str()
                );

            if (it == boneMap.end())
                continue;

            uint32_t boneIndex =
                it->second;

            AnimationChannel& channel =
                animation.channels[boneIndex];

            channel.boneIndex = boneIndex;

            // Translations
            for (unsigned int k = 0;
                k < node->mNumPositionKeys;
                k++)
            {
                auto& key =
                    node->mPositionKeys[k];

                channel.translations.push_back(
                {
                    static_cast<float>(
                        key.mTime /
                        aiAnim->mTicksPerSecond
                    ),
                    glm::vec3(
                        key.mValue.x,
                        key.mValue.y,
                        key.mValue.z
                    )
                });
            }

            // Rotations
            for (unsigned int k = 0;
                k < node->mNumRotationKeys;
                k++)
            {
                auto& key =
                    node->mRotationKeys[k];

                channel.rotations.push_back(
                {
                    static_cast<float>(
                        key.mTime /
                        aiAnim->mTicksPerSecond
                    ),
                    glm::quat(
                        key.mValue.w,
                        key.mValue.x,
                        key.mValue.y,
                        key.mValue.z
                    )
                });
            }

            // Scales
            for (unsigned int k = 0;
                k < node->mNumScalingKeys;
                k++)
            {
                auto& key =
                    node->mScalingKeys[k];

                channel.scales.push_back(
                {
                    static_cast<float>(
                        key.mTime /
                        aiAnim->mTicksPerSecond
                    ),
                    glm::vec3(
                        key.mValue.x,
                        key.mValue.y,
                        key.mValue.z
                    )
                });
            }
        }

        animations.push_back(
            std::move(animation)
        );
    }

    return animations;
}