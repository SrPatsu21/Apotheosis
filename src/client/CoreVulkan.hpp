#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class CoreVulkan
{
private:
    //* Core Vulkan objects
    /*
    The Vulkan instance represents your connection to the Vulkan library and driver.
    It owns the API state for your application. Think of it as "initializing Vulkan."
    */
    static VkInstance instance;

    /*
    Represents the GPU (hardware) you select to run your Vulkan app on.
    You query the available GPUs and pick one that supports the features you need.
    */
    static VkPhysicalDevice physicalDevice;

    /*
    Represents the logical device, which is your app's interface to the GPU you selected (physicalDevice).
    It is configured with queues, extensions, etc. — and is what you use to actually create and control GPU resources.
    */
    static VkDevice device;

    /**
    @brief Cached depth format supported by the selected GPU.

    Set by `CoreVulkan::findDepthFormat()` during initialization, this value is used when creating
    depth images and views for render passes.

    The value is guaranteed to be valid only after calling `CoreVulkan::init()`.
    */
    static VkFormat depthFormat;
    /*
    Constructor.
    Creates the Vulkan instance, picks a suitable physical device, and creates a logical device.

    Throws std::runtime_error on failure.
     */
    CoreVulkan();

    // TODO extend this to implement device selection based on features, queue families, performance, etc.
    /*
    Selects a physical device (GPU) to use with Vulkan.

    This function queries all physical devices available on the system that support Vulkan
    and chooses one to use. Currently, it simply picks the first available device.

    @throws std::runtime_error if no Vulkan-compatible GPUs are found.
    */
    static void pickPhysicalDevice();

    /*
    @brief Creates the Vulkan logical device and selects a graphics-capable queue.

    This method of the CoreVulkan singleton:
    - Queries the physical device for available queue families.
    - Finds a queue family that supports graphics operations (`VK_QUEUE_GRAPHICS_BIT`).
    - Records the index of that queue family in `graphicsQueueFamily`.
    - Creates a Vulkan logical device (`VkDevice`) using that queue family and enables the `VK_KHR_swapchain` extension.
    - Retrieves a handle to the graphics queue from the logical device and stores it in `graphicsQueue`.

    The logical device (`CoreVulkan::device`) is stored internally as part of the CoreVulkan state.

    @param[out] graphicsQueueFamily The index of the queue family that supports graphics operations.
    @param[out] graphicsQueue The handle to the graphics queue from the logical device.

    @throws std::runtime_error If no graphics-capable queue family is found, or if logical device creation fails.

    @note
    The function assumes `CoreVulkan::physicalDevice` has already been initialized.
    The logical device is configured with default physical device features and enables the swapchain extension.
    */
    static void createLogicalDevice(uint32_t& graphicsQueueFamily, VkQueue& graphicsQueue);

    /**
    @brief Determines the best supported depth format for the selected GPU.

    Queries the GPU (`CoreVulkan::physicalDevice`) to find the most suitable format
    for depth-stencil attachments, in the following preferred order:

    - `VK_FORMAT_D32_SFLOAT`
    - `VK_FORMAT_D32_SFLOAT_S8_UINT`
    - `VK_FORMAT_D24_UNORM_S8_UINT`

    The first format with `VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT` support in
    optimal tiling mode is chosen and stored in `CoreVulkan::depthFormat`.

    This function must be called after selecting the physical device.

    @throws std::runtime_error if none of the candidate formats are supported.
    */
    static void findDepthFormat();
    /**
    Creates and initializes the Vulkan instance.

    Sets up the VkApplicationInfo and VkInstanceCreateInfo structures,
    enables required extensions (as reported by GLFW), and calls vkCreateInstance.

    The created instance is stored in CoreVulkan::instance.

    @throws std::runtime_error if instance creation fails.
    */
    static void createInstance();
public:
    // Deleting the copy constructor to prevent copies
    CoreVulkan(const CoreVulkan& obj) = delete;

    /**
    @brief Initializes the Vulkan core objects.

    This function performs the one-time setup of the Vulkan API for your application:
    - Creates the Vulkan instance.
    - Selects a suitable physical device (GPU) that supports Vulkan.
    - Creates a logical device and retrieves the graphics queue.
    - Queries and caches the optimal depth format supported by the GPU.

    If the Vulkan instance was already initialized (`CoreVulkan::instance != nullptr`), 
    this function returns immediately and does nothing.

    @param graphicsQueueFamily Reference to an integer to receive the index of the selected graphics queue family.
    @param graphicsQueue Reference to a VkQueue handle to receive the created graphics queue.

    @throws std::runtime_error if any of the initialization steps fail (e.g., instance creation, device creation, depth format not supported).
    */
    static void init(uint32_t& graphicsQueueFamily, VkQueue& graphicsQueue);
    /*
    Cleans up Vulkan objects in the correct order.
     */
    static void destroy();

    static VkInstance getInstance() { return instance; }
    static VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
    static VkDevice getDevice() { return device; }
    static VkFormat getDepthFormat() { return depthFormat; }


};