#pragma once

#include <vulkan/vulkan.h>

class BoneDescriptorManager
{
private:

    VkDevice device;

    VkDescriptorSetLayout descriptorSetLayout;

public:

    BoneDescriptorManager(
        VkDevice device
    );

    ~BoneDescriptorManager();

    BoneDescriptorManager(
        const BoneDescriptorManager&
    ) = delete;

    BoneDescriptorManager& operator=(
        const BoneDescriptorManager&
    ) = delete;

    BoneDescriptorManager(
        BoneDescriptorManager&&
    ) = delete;

    BoneDescriptorManager& operator=(
        BoneDescriptorManager&&
    ) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const
    {
        return descriptorSetLayout;
    }
};