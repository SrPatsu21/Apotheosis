#pragma once

#include "../../../CoreVulkan.hpp"
#include <glm/glm.hpp>
#include <vector>

class BufferManager;

class BoneBufferManager
{
private:

    VkDevice device;

    VkBuffer buffer;
    VkDeviceMemory memory;

    size_t maxBones;

public:

    BoneBufferManager(
        VkDevice device,
        BufferManager* bufferManager,
        size_t maxBones
    );

    ~BoneBufferManager();

    void update(
        const std::vector<glm::mat4>& matrices
    );

    VkBuffer getBuffer() const
    {
        return buffer;
    }
};