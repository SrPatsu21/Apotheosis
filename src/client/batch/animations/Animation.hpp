#include <stream.h>

enum class AnimationPath
{
    Translation,
    Rotation,
    Scale
};

template<typename T>
struct KeyFrame
{
    float time;
    T value;
};

struct AnimationChannel
{
    uint32_t boneIndex;

    std::vector<KeyFrame<glm::vec3>> translations;
    std::vector<KeyFrame<glm::quat>> rotations;
    std::vector<KeyFrame<glm::vec3>> scales;
};

class Animation
{
public:

    std::string name;

    float duration = 0.0f;

    uint32_t skeletonBoneCount = 0;

    std::vector<AnimationChannel> channels;
};