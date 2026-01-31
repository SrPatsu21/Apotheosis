#pragma once

#include "../CoreVulkan.hpp"
#include "VulkanImageUtils.hpp"
#include "../mash/BufferManager.hpp"

class ImageColor
{
private:
    VkDevice device;
    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
public:
    ImageColor(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkFormat swapchainImageFormat,
        VkExtent2D swapchainExtent,
        VkSampleCountFlagBits msaaSamples
    );

    const VkImage& getColorImage() const { return colorImage; }
    const VkDeviceMemory& getColorImageMemory() const { return colorImageMemory; }
    const VkImageView& getColorImageView() const { return colorImageView; }

    ~ImageColor();
};