#pragma once
#include "../../../CoreVulkan.hpp"
#include "BoneDescriptorSetLayout.hpp"
#include "BoneBufferManager.hpp"

class BoneDescriptorManager
{
private:
    VkDevice device;

    BoneDescriptorSetLayout* layout;

    VkDescriptorPool descriptorPool;

    std::vector<VkDescriptorSet> descriptorSets;

public:

    BoneDescriptorManager(
        VkDevice device,
        BoneDescriptorSetLayout* layout,
        BoneBufferManager* boneBuffer,
        uint32_t framesInFlight
    );

    ~BoneDescriptorManager();

    VkDescriptorSetLayout getDescriptorSetLayout() const;

    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return descriptorSets; }
};