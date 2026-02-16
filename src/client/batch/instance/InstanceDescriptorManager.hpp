#pragma once

#include "../../CoreVulkan.hpp"
#include "../../BufferManager.hpp"
#include "InstanceData.hpp"

class InstanceDescriptorManager {
private:
    VkDevice device;
    uint32_t maxInstances;

    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceMemory> memory;
    std::vector<void*> mapped;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
public:
    InstanceDescriptorManager(
        VkDevice device,
        BufferManager* bufferManager,
        uint32_t maxFramesInFlight,
        uint32_t maxInstances
    );
    ~InstanceDescriptorManager();

    void update(
        uint32_t frameIndex,
        const std::vector<InstanceData>& models
    );

    VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return descriptorSets; }
};
