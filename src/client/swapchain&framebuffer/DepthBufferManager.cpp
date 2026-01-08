#include "DepthBufferManager.hpp"
#include "../image/VulkanImageUtils.hpp"

DepthBufferManager::DepthBufferManager(VkExtent2D swapchainExtent) :
        depthImage(VK_NULL_HANDLE),
        depthImageMemory(VK_NULL_HANDLE),
        depthImageView(VK_NULL_HANDLE)
{

    createImage(
        swapchainExtent.width,
        swapchainExtent.height,
        CoreVulkan::getDepthFormat(),
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        this->depthImage,
        this->depthImageMemory
    );

    depthImageView = createImageView(CoreVulkan::getDevice(), this->depthImage, CoreVulkan::getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkImageView DepthBufferManager::createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    return imageView;
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