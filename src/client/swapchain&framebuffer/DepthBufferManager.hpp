#pragma once

#include "../CoreVulkan.hpp"

class DepthBufferManager
{
private:
    VkDevice device;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

public:
    static bool hasStencilComponent(VkFormat format);

    DepthBufferManager(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkExtent2D swapchainExtent,
        VkSampleCountFlagBits msaaSamples,
        VkFormat depthFormat,
        VkImageAspectFlags aspect
    );

    ~DepthBufferManager();


    VkImage getDepthImage() const { return depthImage; }
    VkDeviceMemory getDepthImageMemory() const { return depthImageMemory; }
    VkImageView getDepthImageView() const { return depthImageView; }
};