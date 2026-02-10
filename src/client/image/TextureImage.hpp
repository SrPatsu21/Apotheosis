#pragma once

#include "stb_image.h"
#include "../CoreVulkan.hpp"
#include "../mash/BufferManager.hpp"

class TextureImage
{
public:

    struct TextureImageDesc {
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
        bool generateMipmaps = true;
    };

    class IImageTransitionPolicy {
    public:
        virtual ~IImageTransitionPolicy() = default;

        virtual void transition(
            BufferManager* bufferManager,
            VkImage image,
            VkFormat format,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            uint32_t mipLevels
        ) = 0;
    };

    class DefaultImageTransitionPolicy : public IImageTransitionPolicy{
    public:
        static DefaultImageTransitionPolicy& instance() {
            static DefaultImageTransitionPolicy policy;
            return policy;
        }

        void transition(
            BufferManager* bufferManager,
            VkImage image,
            VkFormat format,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            uint32_t mipLevels
        ) override;

    private:
        DefaultImageTransitionPolicy() = default;
    };

protected:
    VkDevice device;
    uint32_t mipLevels;
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
        VkDevice device = VK_NULL_HANDLE;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        StagingBufferRAII() = delete;

        explicit StagingBufferRAII(VkDevice device_)
        : device(device_) {}

        ~StagingBufferRAII() {
            if (buffer != VK_NULL_HANDLE)
                vkDestroyBuffer(device, buffer, nullptr);
            if (memory != VK_NULL_HANDLE)
                vkFreeMemory(device, memory, nullptr);
        }

        StagingBufferRAII(const StagingBufferRAII&) = delete;
        StagingBufferRAII& operator=(const StagingBufferRAII&) = delete;

        StagingBufferRAII(StagingBufferRAII&& other) = delete;
        StagingBufferRAII& operator=(StagingBufferRAII&& other) = delete;
    };

    void createTextureImage(
        VkPhysicalDevice physicalDevice,
        const char* path,
        BufferManager* bufferManager,
        const TextureImageDesc& desc,
        IImageTransitionPolicy* transitionPolicy
    );
    void createTextureImageView();
    void createTextureSampler(
        VkPhysicalDevice physicalDevice
    );
    void createStagingBuffer(
        BufferManager* bufferManager,
        const LoadedImage& img,
        VkBuffer& buffer,
        VkDeviceMemory& memory
    );
    void loadImageFromFile(
        const char* path,
        LoadedImage& img
    );

public:
    TextureImage(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        const char* path,
        BufferManager* bufferManager,
        const TextureImageDesc& desc,
        IImageTransitionPolicy* transitionPolicy
    );
    ~TextureImage();

    const VkImage& getTextureImage() const { return textureImage; }
    const VkDeviceMemory& getTextureImageMemory() const { return textureImageMemory; }
    const VkImageView& getTextureImageView() const { return textureImageView; }
    const VkSampler& getTextureSampler() const { return textureSampler; }
};