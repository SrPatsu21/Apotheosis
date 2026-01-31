#pragma once

#include "../CoreVulkan.hpp"

class DepthBufferManager
{
private:
    VkDevice device;
    /*
    A GPU image resource that stores the depth buffer for your render pass.
    It is needed for proper depth testing in 3D scenes.
    */
    VkImage depthImage;
    /*
    Memory backing the depthImage.
    */
    VkDeviceMemory depthImageMemory;
    /*
    A view of the depthImage, which is used when attaching it to a framebuffer.
    */
    VkImageView depthImageView;

public:
    /**
    @brief Constructs a DepthBufferManager, creating the GPU resources for depth testing.

    This constructor creates:
    - a Vulkan depth image,
    - allocates GPU memory for it,
    - and creates an image view for use as a depth attachment.

    The created resources are used as the depth buffer in a render pass.

    @param swapchainExtent The extent (width & height) of the swapchain images, used for depth buffer size.

    @throws std::runtime_error if image, memory, or image view creation fails.
    */
    DepthBufferManager(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkExtent2D swapchainExtent,
        VkSampleCountFlagBits msaaSamples,
        VkFormat depthFormat
    );
    /**
    @brief Destructor, cleans up the Vulkan depth buffer resources.

    Destroys the depth image view, the image itself, and frees the GPU memory allocated for it.
    */
    ~DepthBufferManager();


    VkImage getDepthImage() const {
        return depthImage;
    }
    VkDeviceMemory getDepthImageMemory() const {
        return depthImageMemory;
    }
    VkImageView getDepthImageView() const {
        return depthImageView;
    }
};