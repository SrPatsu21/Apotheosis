#include "DepthBufferManager.hpp"
#include "../image/VulkanImageUtils.hpp"

DepthBufferManager::DepthBufferManager(VkExtent2D swapchainExtent) :
        depthImage(VK_NULL_HANDLE),
        depthImageMemory(VK_NULL_HANDLE),
        depthImageView(VK_NULL_HANDLE)
{
    VkFormat depthFormat = CoreVulkan::getDepthFormat();

    createImage(
        swapchainExtent.width,
        swapchainExtent.height,
        1,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        this->depthImage,
        this->depthImageMemory
    );

    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

DepthBufferManager::~DepthBufferManager()
{
    if (this->depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(CoreVulkan::getDevice(), this->depthImageView, nullptr);
        this->depthImageView = VK_NULL_HANDLE;
    }
    if (this->depthImage != VK_NULL_HANDLE) {
        vkDestroyImage(CoreVulkan::getDevice(), this->depthImage, nullptr);
        this->depthImage = VK_NULL_HANDLE;
    }
    if (this->depthImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(CoreVulkan::getDevice(), this->depthImageMemory, nullptr);
        this->depthImageMemory = VK_NULL_HANDLE;
    }
};