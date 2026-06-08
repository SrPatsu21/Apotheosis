#pragma once

#include <glm/glm.hpp>
#include <vector>



class SkeletonPose
{
public:

    std::vector<glm::mat4> finalMatrices;

public:

    SkeletonPose() = default;

    explicit SkeletonPose(size_t boneCount)
    {
        resize(boneCount);
    }

    void resize(size_t boneCount)
    {
        finalMatrices.resize(
            boneCount,
            glm::mat4(1.0f)
        );
    }

    size_t size() const
    {
        return finalMatrices.size();
    }

    glm::mat4& operator[](size_t index)
    {
        return finalMatrices[index];
    }

    const glm::mat4& operator[](size_t index) const
    {
        return finalMatrices[index];
    }

    void reset()
    {
        for (auto& matrix : finalMatrices)
        {
            matrix = glm::mat4(1.0f);
        }
    }
};