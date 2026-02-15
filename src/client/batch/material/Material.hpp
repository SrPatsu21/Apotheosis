#pragma once

#include "../../CoreVulkan.hpp"
#include "TextureImage.hpp"
#include <memory>

class Material
{
private:
    VkDevice device;

    std::shared_ptr<TextureImage> texture;
    VkDescriptorSet descriptorSet{VK_NULL_HANDLE};
public:
    Material(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        VkDescriptorSetLayout layout,
        std::shared_ptr<TextureImage> texture
    );

    VkDescriptorSet getDescriptorSet() const { return descriptorSet; }
};
