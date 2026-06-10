#pragma once

#include "../../../CoreVulkan.hpp"
#include "../../../BufferManager.hpp"

#include <vector>

class BoneOffsetBufferManager
{
private:

    VkDevice device;

    VkBuffer buffer;
    VkDeviceMemory memory;

    size_t maxInstances;

public:

    BoneOffsetBufferManager(
        VkDevice device,
        BufferManager* bufferManager,
        size_t maxInstances
    );

    ~BoneOffsetBufferManager();

    void update(
        const std::vector<uint32_t>& offsets
    );

    VkBuffer getBuffer() const
    {
        return buffer;
    }
};