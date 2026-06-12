#pragma once

#include "../../../CoreVulkan.hpp"
#include "../../../BufferManager.hpp"
#include "BoneDescriptorSetLayout.hpp"

#include <vector>

class BoneOffsetBufferManager
{
private:

    VkDevice device;

    VkBuffer buffer;
    VkDeviceMemory memory;

    size_t maxInstances;

    VkDescriptorPool descriptorPool;

    std::vector<VkDescriptorSet> descriptorSets;

public:

    BoneOffsetBufferManager(
        VkDevice device,
        BufferManager* bufferManager,
        size_t maxInstances,
        BoneDescriptorSetLayout* layout,
        uint32_t framesInFlight
    );

    ~BoneOffsetBufferManager();

    void update(
        uint32_t offset,
        const std::vector<uint32_t>& data
    );

    VkBuffer getBuffer() const
    {
        return buffer;
    }

    const std::vector<VkDescriptorSet>& getDescriptorSets() const
    {
        return descriptorSets;
    }
};