#include "DepthBufferManager.hpp"
#include "../image/VulkanImageUtils.hpp"

DepthBufferManager::DepthBufferManager(VkExtent2D swapchainExtent) {

    createImage(
        swapchainExtent.width,
        swapchainExtent.height,
        CoreVulkan::getDepthFormat(),
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory
    );

        depthImageView = createImageView(this->depthImage, CoreVulkan::getDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
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
    vkDestroyImageView(CoreVulkan::getDevice(), this->depthImageView, nullptr);
    vkDestroyImage(CoreVulkan::getDevice(), this->depthImage, nullptr);
    vkFreeMemory(CoreVulkan::getDevice(), this->depthImageMemory, nullptr);
};