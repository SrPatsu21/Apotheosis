#pragma once

#include "../CoreVulkan.hpp"
#include "../mash/BufferManager.hpp"

/**
@brief Creates a 2D Vulkan image and allocates & binds its memory.
@param width Image width in pixels.
@param height Image height in pixels.
@param format Format of the image (e.g., depth or color).
@param tiling Image tiling (optimal or linear).
@param usage Usage flags describing how the image will be used.
@param properties Memory property flags (e.g., device local).
@param image (out) Created VkImage handle.
@param imageMemory (out) Allocated VkDeviceMemory handle.
*/
void createImage(
    uint32_t width,
    uint32_t height,
    uint32_t mipLevels,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory
);

/**
@brief Creates a 2D image view for a given image.

@param image Image handle to create a view for.
@param format Image format.
@param aspectFlags Aspect mask (e.g., VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT).
@return VkImageView The created image view handle.

@throws std::runtime_error if image view creation fails.
 */
VkImageView createImageView(
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags,
    uint32_t mipLevels
);

void generateMipmaps(
    BufferManager* bufferManager,
    VkCommandPool commandPool,
    VkImage image,
    VkFormat imageFormat,
    int32_t texWidth,
    int32_t texHeight,
    uint32_t mipLevels
);