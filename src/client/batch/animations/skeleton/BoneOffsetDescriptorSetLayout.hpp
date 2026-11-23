#pragma once

#include <vulkan/vulkan.h>

class BoneOffsetDescriptorSetLayout
{
private:

    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout;

public:

    BoneOffsetDescriptorSetLayout(
        VkDevice device
    );

    ~BoneOffsetDescriptorSetLayout();

    VkDescriptorSetLayout getDescriptorSetLayout() const
    {
        return descriptorSetLayout;
    }
};