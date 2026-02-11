#pragma once

#include "stb_image.h"
#include "../CoreVulkan.hpp"
#include "../mash/BufferManager.hpp"

/**
 * @brief Represents a GPU texture loaded from an image file.
 *
 * TextureImage encapsulates the full lifetime and upload process of a 2D texture:
 * - Image loading via stb_image
 * - CPU staging buffer creation
 * - GPU image creation
 * - Layout transitions
 * - Optional mipmap generation
 * - Image view and sampler creation
 *
 * The class owns all Vulkan resources it creates and releases them on destruction.
 */
class TextureImage
{
public:

    /**
     * @brief Describes how the texture image should be created.
     *
     * This structure controls GPU-side behavior only.
     * Image decoding and pixel format conversion are handled separately.
     */
    struct TextureImageDesc {
        /// GPU image format
        VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

        /// Image usage flags (transfer + sampling by default)
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                VK_IMAGE_USAGE_SAMPLED_BIT;

        /// Sample count (normally 1 for textures)
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        /// Whether mipmaps should be generated on the GPU
        bool generateMipmaps = true;
    };


    /**
     * @brief Strategy interface for image layout transitions.
     *
     * Allows different layout transition behaviors without hard-coding
     * synchronization logic into TextureImage.
     */
    class IImageTransitionPolicy {
    public:
        virtual ~IImageTransitionPolicy() = default;

        /**
         * @brief Performs a layout transition for a Vulkan image.
         *
         * @param bufferManager Command submission helper.
         * @param image Image to transition.
         * @param format Image format.
         * @param oldLayout Current image layout.
         * @param newLayout Target image layout.
         * @param mipLevels Number of mip levels affected.
         */
        virtual void transition(
            BufferManager* bufferManager,
            VkImage image,
            VkFormat format,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            uint32_t mipLevels
        ) = 0;
    };

    /**
     * @brief Default image layout transition implementation.
     *
     * Supports the following transitions:
     * - UNDEFINED -> TRANSFER_DST_OPTIMAL
     * - TRANSFER_DST_OPTIMAL -> SHADER_READ_ONLY_OPTIMAL
     * - UNDEFINED -> DEPTH_STENCIL_ATTACHMENT_OPTIMAL
     *
     * Uses immediate command buffers for simplicity.
     */
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

    /**
     * @brief RAII wrapper for image data loaded from disk.
     *
     * Owns stb_image pixel memory and guarantees cleanup.
     * Copying is disabled; moving is allowed.
     */
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

    /**
     * @brief RAII wrapper for a staging buffer used during texture upload.
     *
     * Ensures temporary GPU resources are freed even if exceptions occur.
     */
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


        /**
     * @brief Loads an image from disk into CPU memory.
     *
     * The image is always converted to RGBA8 format.
     *
     * @param path File path.
     * @param img Output loaded image.
     */
    void loadImageFromFile(
        const char* path,
        LoadedImage& img
    );

    /**
     * @brief Creates a host-visible staging buffer and uploads pixel data.
     *
     * @param bufferManager Buffer creation utility.
     * @param img Loaded CPU image.
     * @param buffer Output staging buffer.
     * @param memory Output staging memory.
     */
    void createStagingBuffer(
        BufferManager* bufferManager,
        const LoadedImage& img,
        VkBuffer& buffer,
        VkDeviceMemory& memory
    );

    /**
     * @brief Creates the GPU image and uploads texture data.
     *
     * Handles:
     * - Mip level calculation
     * - GPU image allocation
     * - Layout transitions
     * - Buffer-to-image copy
     * - Optional mipmap generation
     */
    void createTextureImage(
        VkPhysicalDevice physicalDevice,
        const char* path,
        BufferManager* bufferManager,
        const TextureImageDesc& desc,
        IImageTransitionPolicy* transitionPolicy
    );
    /**
     * @brief Creates the image view for the texture.
     */
    void createTextureImageView();
    /**
     * @brief Creates the Vulkan sampler for the texture.
     *
     * Uses linear filtering, repeat addressing and max supported anisotropy.
     */
    void createTextureSampler(
        VkPhysicalDevice physicalDevice
    );

public:
    /**
     * @brief Loads a texture from disk and uploads it to the GPU.
     *
     * @param physicalDevice Physical device used for limits and memory selection.
     * @param device Logical Vulkan device.
     * @param path Image file path.
     * @param bufferManager Command and buffer helper.
     * @param desc Texture creation parameters.
     * @param transitionPolicy Image layout transition policy.
     */
    TextureImage(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        const char* path,
        BufferManager* bufferManager,
        const TextureImageDesc& desc,
        IImageTransitionPolicy* transitionPolicy
    );
    /**
     * @brief Releases all Vulkan resources owned by the texture.
     */
    ~TextureImage();

    const VkImage& getTextureImage() const { return textureImage; }
    const VkDeviceMemory& getTextureImageMemory() const { return textureImageMemory; }
    const VkImageView& getTextureImageView() const { return textureImageView; }
    const VkSampler& getTextureSampler() const { return textureSampler; }
};