#pragma once

#include "../CoreVulkan.hpp"

class DepthBufferManager
{
private:
    /*
    A GPU image resource that stores the depth buffer for your render pass.
    It is needed for proper depth testing in 3D scenes.
    */
    VkImage depthImage;
    /*
    Memory backing the depthImage.
    */
    VkDeviceMemory depthImageMemory;
    /*
    A view of the depthImage, which is used when attaching it to a framebuffer.
    */
    VkImageView depthImageView;
public:
    DepthBufferManager(/* args */);
    ~DepthBufferManager();
};

void createDepthResources(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapchainExtent) {
    VkFormat depthFormat = findDepthFormat(physicalDevice);

    createImage(
        device,
        physicalDevice,
        swapchainExtent.width,
        swapchainExtent.height,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory
    );

    depthImageView = createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

~DepthBufferManager()
{
    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);
};
