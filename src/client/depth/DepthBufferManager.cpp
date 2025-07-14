#include "DepthBufferManager.hpp"

DepthBufferManager::DepthBufferManager(VkExtent2D swapchainExtent) {

    createImage(
        CoreVulkan::getDevice(),
        CoreVulkan::getPhysicalDevice(),
        swapchainExtent.width,
        swapchainExtent.height,
        CoreVulkan::getDepthFormat(),
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory
    );

        depthImageView = createImageView(CoreVulkan::getDevice(), this->depthImage, CoreVulkan::getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);
}

DepthBufferManager::~DepthBufferManager()
{
    vkDestroyImageView(CoreVulkan::getDevice(), this->depthImageView, nullptr);
    vkDestroyImage(CoreVulkan::getDevice(), this->depthImage, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), this->depthImageMemory, nullptr);
};