#pragma once

#include "stb_image.h"
#include "../CoreVulkan.hpp"
#include "../mash/BufferManager.hpp"

class TextureImage
{
private:
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    struct LoadedImage {
        int width = 0;
        int height = 0;
        VkDeviceSize size = 0;
        stbi_uc* pixels = nullptr;

        ~LoadedImage() {
            if (pixels) {
                stbi_image_free(pixels);
            }
        }

        LoadedImage()
        : width(0), height(0), size(0), pixels(nullptr) {}
        //block copy
        LoadedImage(const LoadedImage&) = delete;
        LoadedImage& operator=(const LoadedImage&) = delete;

        // move constructor
        LoadedImage(LoadedImage&& other) noexcept {
            //move assignment.
            *this = std::move(other);
        }

        // Move Assignment Operator
        LoadedImage& operator=(LoadedImage&& other) noexcept {
            if (this != &other) {
                if (pixels) {
                    stbi_image_free(pixels);
                }

                width  = other.width;
                height = other.height;
                size   = other.size;
                pixels = other.pixels;

                other.pixels = nullptr;
            }
            return *this;
        }
    };

    struct StagingBufferRAII {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        ~StagingBufferRAII() {
            VkDevice device = CoreVulkan::getDevice();
            if (buffer != VK_NULL_HANDLE)
                vkDestroyBuffer(device, buffer, nullptr);
            if (memory != VK_NULL_HANDLE)
                vkFreeMemory(device, memory, nullptr);
        }

        StagingBufferRAII(const StagingBufferRAII&) = delete;
        StagingBufferRAII& operator=(const StagingBufferRAII&) = delete;
    };

public:

    TextureImage(
        const char* path,
        BufferManager* bufferManager,
        VkCommandPool commandPool
    );
    void loadImageFromFile(
        const char* path,
        LoadedImage& img
    );
    void createStagingBuffer(
        BufferManager* bufferManager,
        const LoadedImage& img,
        VkBuffer& buffer,
        VkDeviceMemory& memory
    );
    static void transitionImageLayout(
        BufferManager* bufferManager,
        VkCommandPool commandPool,
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout
    );
    void createGpuImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );
    void uploadToGpu(
        BufferManager* bufferManager,
        VkCommandPool commandPool,
        VkBuffer stagingBuffer,
        uint32_t width,
        uint32_t height
    );
    void createTextureImage(
        const char* path,
        BufferManager* bufferManager,
        VkCommandPool commandPool
    );
    void createTextureImageView();
    void createTextureSampler();
    ~TextureImage();

const VkImage& getTextureImage() const { return textureImage; }
const VkDeviceMemory& getTextureImageMemory() const { return textureImageMemory; }
const VkImageView& getTextureImageView() const { return textureImageView; }
const VkSampler& getTextureSampler() const { return textureSampler; }
};
