#pragma once

#include "../../CoreVulkan.hpp"

class MaterialDescriptorManager
{
public:
    MaterialDescriptorManager(
        VkDevice device,
        uint32_t maxMaterials
    );

    ~MaterialDescriptorManager();

    VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

private:
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorPool descriptorPool{};
};
