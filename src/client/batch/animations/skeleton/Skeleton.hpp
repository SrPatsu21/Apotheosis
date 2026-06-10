#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "BoneTransform.hpp"

struct Bone
{
    std::string name;

    int parentIndex;

    glm::mat4 inverseBindMatrix;
};

class Skeleton
{
public:

    std::vector<Bone> bones;
    std::vector<BoneTransform> bindPose;

    size_t getBoneCount() const
    {
        return bones.size();
    }

    uint32_t Skeleton::findBoneIndex(
        const std::string& name
    ) const
    {
        for (uint32_t i = 0; i < bones.size(); i++)
        {
            if (bones[i].name == name)
                return i;
        }

        throw std::runtime_error("Bone not found");
    }
};