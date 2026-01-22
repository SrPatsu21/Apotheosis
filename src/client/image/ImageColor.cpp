#include "ImageColor.hpp"

ImageColor::ImageColor(
    VkFormat swapchainImageFormat,
    VkExtent2D swapchainExtent,
    VkSampleCountFlagBits msaaSamples
) {
    createColorResources(
        swapchainImageFormat,
        swapchainExtent,
        msaaSamples
    );
}

ImageColor::~ImageColor()
{
    VkDevice device = CoreVulkan::getDevice();

    vkDestroyImageView(device, colorImageView, nullptr);
    vkDestroyImage(device, colorImage, nullptr);
    vkFreeMemory(device, colorImageMemory, nullptr);
}

void ImageColor::createColorResources(
    VkFormat swapchainImageFormat,
    VkExtent2D swapchainExtent,
    VkSampleCountFlagBits msaaSamples
) {
    VkFormat colorFormat = swapchainImageFormat;

    createImage(
        swapchainExtent.width,
        swapchainExtent.height,
        1,
        msaaSamples,
        colorFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        colorImage,
        colorImageMemory
    );
    colorImageView = createImageView(
        colorImage,
        colorFormat,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1
    );
}