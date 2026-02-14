#pragma once

#include "../CoreVulkan.hpp"
#include <vector>

class CameraBufferManager;

class GlobalDescriptorManager
{
private:
    VkDevice device;

    VkDescriptorSetLayout descriptorSetLayout{VK_NULL_HANDLE};
    VkDescriptorPool descriptorPool{VK_NULL_HANDLE};
    std::vector<VkDescriptorSet> descriptorSets;

public:
    GlobalDescriptorManager(
        VkDevice device,
        CameraBufferManager* cameraBufferManager,
        uint32_t maxFramesInFlight
    );

    ~GlobalDescriptorManager();

    VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
    std::vector<VkDescriptorSet> getDescriptorSets() const { return descriptorSets; }
};
