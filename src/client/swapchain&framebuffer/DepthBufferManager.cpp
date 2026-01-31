#include "DepthBufferManager.hpp"
#include "../image/VulkanImageUtils.hpp"

DepthBufferManager::DepthBufferManager(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkExtent2D swapchainExtent,
    VkSampleCountFlagBits msaaSamples,
    VkFormat depthFormat
) :
        device(device),
        depthImage(VK_NULL_HANDLE),
        depthImageMemory(VK_NULL_HANDLE),
        depthImageView(VK_NULL_HANDLE)
{
    createImage(
        physicalDevice,
        device,
        swapchainExtent.width,
        swapchainExtent.height,
        1,
        msaaSamples,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        this->depthImage,
        this->depthImageMemory
    );

    depthImageView = createImageView(
        device,
        depthImage,
        depthFormat,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        1
    );
}

DepthBufferManager::~DepthBufferManager()
{
    if (this->depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(device, this->depthImageView, nullptr);
        this->depthImageView = VK_NULL_HANDLE;
    }
    if (this->depthImage != VK_NULL_HANDLE) {
        vkDestroyImage(device, this->depthImage, nullptr);
        this->depthImage = VK_NULL_HANDLE;
    }
    if (this->depthImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, this->depthImageMemory, nullptr);
        this->depthImageMemory = VK_NULL_HANDLE;
    }
};