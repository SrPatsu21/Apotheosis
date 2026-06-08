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
};