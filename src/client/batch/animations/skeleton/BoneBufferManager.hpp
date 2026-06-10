#include "../../../CoreVulkan.hpp"

class BoneBufferManager
{
private:

    VkBuffer buffer;
    VkDeviceMemory memory;

public:

    void update(
        const std::vector<glm::mat4>& matrices
    );

    VkBuffer getBuffer() const;
};