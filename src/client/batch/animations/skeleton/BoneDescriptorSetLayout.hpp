#pragma once

#include <vulkan/vulkan.h>

class BoneDescriptorSetLayout
{
private:

    VkDevice device;

    VkDescriptorSetLayout descriptorSetLayout;

public:

    BoneDescriptorSetLayout(
        VkDevice device
    );

    ~BoneDescriptorSetLayout();

    BoneDescriptorSetLayout(
        const BoneDescriptorSetLayout&
    ) = delete;

    BoneDescriptorSetLayout& operator=(
        const BoneDescriptorSetLayout&
    ) = delete;

    BoneDescriptorSetLayout(
        BoneDescriptorSetLayout&&
    ) = delete;

    BoneDescriptorSetLayout& operator=(
        BoneDescriptorSetLayout&&
    ) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const
    {
        return descriptorSetLayout;
    }
};