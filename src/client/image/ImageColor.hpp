#pragma once

#include "../CoreVulkan.hpp"
#include "VulkanImageUtils.hpp"
#include "../mash/BufferManager.hpp"

class ImageColor
{
private:
    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
    void createColorResources(
        VkFormat swapchainImageFormat,
        VkExtent2D swapchainExtent,
        VkSampleCountFlagBits msaaSamples
    );
public:
    ImageColor(
        VkFormat swapchainImageFormat,
        VkExtent2D swapchainExtent,
        VkSampleCountFlagBits msaaSamples
    );

    const VkImage& getColorImage() const { return colorImage; }
    const VkDeviceMemory& getColorImageMemory() const { return colorImageMemory; }
    const VkImageView& getColorImageView() const { return colorImageView; }

    ~ImageColor();
};