#include <cmath>

#include "TextureImage.hpp"
#include "../../image/VulkanImageUtils.hpp"

void TextureImage::DefaultImageTransitionPolicy::transition(
    BufferManager& bufferManager,
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    uint32_t mipLevels
) {
    VkCommandBuffer commandBuffer = bufferManager.beginImmediate();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (CoreVulkan::hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
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
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    bufferManager.endImmediate();
}

void TextureImage::loadImageFromFile(
    const std::string& path,
    LoadedImage& img
) {
    int channels;
    img.pixels = stbi_load(path.c_str(), &img.width, &img.height, &channels, STBI_rgb_alpha);

    if (!img.pixels || img.width <= 0 || img.height <= 0) {
        throw std::runtime_error("failed to load texture image");
    }
    img.size = static_cast<VkDeviceSize>(img.width) * img.height * 4;
}

void TextureImage::createStagingBuffer(
    BufferManager& bufferManager,
    const LoadedImage& img,
    VkBuffer& buffer,
    VkDeviceMemory& memory
) {
    bufferManager.createBuffer(
        img.size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        buffer
    );

    bufferManager.allocateBufferMemory(
        buffer,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        memory
    );

    vkBindBufferMemory(device, buffer, memory, 0);

    void* data;
    vkMapMemory(device, memory, 0, img.size, 0, &data);
    memcpy(data, img.pixels, static_cast<size_t>(img.size));
    vkUnmapMemory(device, memory);
}

void TextureImage::createTextureImageView(){
    textureImageView = createImageView(
        device,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_ASPECT_COLOR_BIT,
        mipLevels
    );
}

void TextureImage::createTextureSampler(
    VkPhysicalDevice physicalDevice
) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; //TODO it`s on max quality sould change this?

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void TextureImage::createTextureImage(
    VkPhysicalDevice physicalDevice,
    const std::string& path,
    BufferManager& bufferManager,
    const TextureImageDesc& desc,
    IImageTransitionPolicy* transitionPolicy
) {
    LoadedImage img{};
    loadImageFromFile(
        path,
        img
    );

    if (desc.generateMipmaps) {
        mipLevels = static_cast<uint32_t>(
            std::floor(std::log2(std::max(img.width, img.height)))
        ) + 1;
    } else {
        mipLevels = 1;
    }

    StagingBufferRAII staging(device);
    createStagingBuffer(bufferManager, img, staging.buffer, staging.memory);

    // createGpuImage
    createImage(
        physicalDevice,
        device,
        img.width,
        img.height,
        mipLevels,
        desc.samples,
        desc.format,
        VK_IMAGE_TILING_OPTIMAL,
        desc.usage,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        textureImage,
        textureImageMemory
    );

    // uploadToGpu
    transitionPolicy->transition(
        bufferManager,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        mipLevels
    );

    bufferManager.copyBufferToImage(
        staging.buffer,
        textureImage,
        img.width,
        img.height
    );

    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
    generateMipmaps(
        physicalDevice,
        &bufferManager,
        textureImage,
        VK_FORMAT_R8G8B8A8_SRGB,
        img.width,
        img.height,
        mipLevels
    );
}

TextureImage::TextureImage(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    const std::string& path,
    BufferManager& bufferManager,
    const TextureImageDesc& desc,
    IImageTransitionPolicy* transitionPolicy
) :
    device(device)
{
    createTextureImage(physicalDevice, path, bufferManager, desc, transitionPolicy);
    createTextureImageView();
    createTextureSampler(physicalDevice);
}

TextureImage::~TextureImage()
{
    if (textureImage != VK_NULL_HANDLE)
        vkDestroyImage(device, textureImage, nullptr);

    if (textureImageMemory != VK_NULL_HANDLE)
        vkFreeMemory(device, textureImageMemory, nullptr);

    if (textureSampler != VK_NULL_HANDLE)
        vkDestroySampler(device, textureSampler, nullptr);

    if (textureImageView != VK_NULL_HANDLE)
        vkDestroyImageView(device, textureImageView, nullptr);
}