#include "../CoreVulkan.hpp"
#include "TextureImage.hpp"
#include "VulkanImageUtils.hpp"

TextureImage::LoadedImage TextureImage::loadImageFromFile(const char* path) {
    LoadedImage img;

    int channels;
    img.pixels = stbi_load(path, &img.width, &img.height, &channels, STBI_rgb_alpha);

    if (!img.pixels || img.width <= 0 || img.height <= 0) {
        throw std::runtime_error("failed to load texture image");
    }

    img.size = static_cast<VkDeviceSize>(img.width) * img.height * 4;
    return img;
}

void TextureImage::createStagingBuffer(
    BufferManager* bufferManager,
    const LoadedImage& img,
    VkBuffer& buffer,
    VkDeviceMemory& memory
) {
    bufferManager->createBuffer(
        img.size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        buffer
    );

    bufferManager->allocateBufferMemory(
        buffer,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memory
    );

    vkBindBufferMemory(CoreVulkan::getDevice(), buffer, memory, 0);

    void* data;
    vkMapMemory(CoreVulkan::getDevice(), memory, 0, img.size, 0, &data);
    memcpy(data, img.pixels, static_cast<size_t>(img.size));
    vkUnmapMemory(CoreVulkan::getDevice(), memory);
}

void TextureImage::createGpuImage(
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties
) {
    //create image
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    if (vkCreateImage(CoreVulkan::getDevice(), &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    // alocate
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(CoreVulkan::getDevice(), textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    allocInfo.memoryTypeIndex = CoreVulkan::findMemoryType(memRequirements.memoryTypeBits, properties, 0);

    if (vkAllocateMemory(CoreVulkan::getDevice(), &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    //bind
    vkBindImageMemory(CoreVulkan::getDevice(), textureImage, textureImageMemory, 0);
}

void TextureImage::uploadToGpu(
    BufferManager* bufferManager,
    VkCommandPool commandPool,
    VkBuffer stagingBuffer,
    uint32_t width,
    uint32_t height
) {
    transitionImageLayout(
        bufferManager,
        commandPool,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    bufferManager->copyBufferToImage(
        commandPool,
        stagingBuffer,
        textureImage,
        width,
        height
    );

    transitionImageLayout(
        bufferManager,
        commandPool,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
}

void TextureImage::transitionImageLayout(BufferManager* bufferManager,
    VkCommandPool commandPool,
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
) {
    VkCommandBuffer commandBuffer = bufferManager->beginSingleTimeCommands(commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    bufferManager->endSingleTimeCommands(commandPool, commandBuffer);
}

void TextureImage::createTextureImageView(){
    textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void TextureImage::createTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(CoreVulkan::getPhysicalDevice(), &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; //TODO it`s on max quality sould change this

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(CoreVulkan::getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void TextureImage::createTextureImage(
    const char* path,
    BufferManager* bufferManager,
    VkCommandPool commandPool
) {
    LoadedImage img = loadImageFromFile(path);

    StagingBufferRAII staging = {};
    createStagingBuffer(bufferManager, img, staging.buffer, staging.memory);

    createGpuImage(
        img.width,
        img.height,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    uploadToGpu(
        bufferManager,
        commandPool,
        staging.buffer,
        img.width,
        img.height
    );
}

TextureImage::TextureImage(
    const char* path,
    BufferManager* bufferManager,
    VkCommandPool commandPool
) {
    createTextureImage(path, bufferManager, commandPool);
    createTextureImageView();
    createTextureSampler();
}

TextureImage::~TextureImage()
{
    VkDevice device = CoreVulkan::getDevice();
    if (textureImage != VK_NULL_HANDLE)
        vkDestroyImage(device, textureImage, nullptr);

    if (textureImageMemory != VK_NULL_HANDLE)
        vkFreeMemory(device, textureImageMemory, nullptr);

    if (textureSampler != VK_NULL_HANDLE)
        vkDestroySampler(device, textureSampler, nullptr);

    if (textureImageView != VK_NULL_HANDLE)
        vkDestroyImageView(device, textureImageView, nullptr);
}