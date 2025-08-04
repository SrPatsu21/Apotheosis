#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <optional>

/**
@brief Requested Vulkan validation layers.

List of validation layers to enable during instance creation.
Primarily used to catch common Vulkan API misuses and provide detailed debugging information.
"VK_LAYER_KHRONOS_validation" is the standard layer maintained by Khronos Group.
Only enabled in debug builds.
*/
const static std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    /**
    @brief Flag to enable Vulkan validation layers.

    Controls whether validation layers should be enabled.
    Set to true in debug builds for additional runtime error checking and diagnostics.
    Set to false in release builds to avoid performance overhead.
    */
    const static bool enableValidationLayers = false;
#else
    /**
    @brief Flag to enable Vulkan validation layers.

    Controls whether validation layers should be enabled.
    Set to true in debug builds for additional runtime error checking and diagnostics.
    Set to false in release builds to avoid performance overhead.
    */
    const static bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class CoreVulkan
{
private:
    /**
     * The Vulkan instance represents your connection to the Vulkan library and driver.
     * It owns the API state for your application. Think of it as "initializing Vulkan."
     */
    static VkInstance instance;

    /**
     * Represents the GPU (hardware) you select to run your Vulkan app on.
     * You query the available GPUs and pick one that supports the features you need.
     */
    static VkPhysicalDevice physicalDevice;

    /**
     * Represents the logical device, which is your app's interface to the GPU you selected (physicalDevice).
     * It is configured with queues, extensions, etc. — and is what you use to actually create and control GPU resources.
     */
    static VkDevice device;

    /**
     * @brief Cached depth format supported by the selected GPU.
     *
     * Set by `CoreVulkan::findDepthFormat()` during initialization, this value is used when creating
     * depth images and views for render passes.
     *
     * The value is guaranteed to be valid only after calling `CoreVulkan::init()`.
     */
    static VkFormat depthFormat;
    /**
     * @brief Vulkan window surface handle.
     *
     * Represents the platform-specific surface to which Vulkan will present rendered images.
     * Created using GLFW (or another windowing system) and required for swapchain creation.
     * Acts as the bridge between Vulkan and the window system.
     */
    static VkSurfaceKHR surface;

    /**
     * @brief Vulkan queue used for presenting rendered images to the screen.
     *
     * This queue handles presentation commands, allowing rendered images to be
     * displayed on the surface created with GLFW. It is retrieved from the logical
     * device during initialization after confirming that the associated queue
     * family supports presenting to the window surface.
     *
     * @see createLogicalDevice()
     * @see findQueueFamilies()
     */
    static VkQueue presentQueue;
    /**
     * @brief Stores the indices of queue families that support required operations.
     *
     * This structure holds the indices for queue families on the selected physical device,
     * such as the graphics and presentation queues. It is filled during device selection
     * by `findQueueFamilies()` and used later when creating the logical device and retrieving
     * Vulkan queues like `graphicsQueue` and `presentQueue`.
     *
     * @see findQueueFamilies()
     * @see createLogicalDevice()
     */
    static QueueFamilyIndices graphicsQueueFamilyIndices;
    /**
     * Constructor.
     * Creates the Vulkan instance, picks a suitable physical device, and creates a logical device.
     *
     * Throws std::runtime_error on failure.
     */
    CoreVulkan();

    // TODO extend this to implement device selection based on features, queue families, performance, etc.
    /**
     * Selects a physical device (GPU) to use with Vulkan.
     *
     * This function queries all physical devices available on the system that support Vulkan
     * and chooses one to use. Currently, it simply picks the first available device.
     *
     * @throws std::runtime_error if no Vulkan-compatible GPUs are found.
     */
    static void pickPhysicalDevice();

    /**
     * @brief Creates the Vulkan logical device and selects a graphics-capable queue.
     *
     * This method of the CoreVulkan singleton:
     * - Queries the physical device for available queue families.
     * - Finds a queue family that supports graphics operations (`VK_QUEUE_GRAPHICS_BIT`).
     * - Records the index of that queue family in `graphicsQueueFamily`.
     * - Creates a Vulkan logical device (`VkDevice`) using that queue family and enables the `VK_KHR_swapchain` extension.
     * - Retrieves a handle to the graphics queue from the logical device and stores it in `graphicsQueue`.
     *
     * The logical device (`CoreVulkan::device`) is stored internally as part of the CoreVulkan state.
     *
     * @param[out] graphicsQueueFamily The index of the queue family that supports graphics operations.
     * @param[out] graphicsQueue The handle to the graphics queue from the logical device.
     *
     * @throws std::runtime_error If no graphics-capable queue family is found, or if logical device creation fails.
     *
     * @note
     * The function assumes `CoreVulkan::physicalDevice` has already been initialized.
     * The logical device is configured with default physical device features and enables the swapchain extension.
     */
    static void createLogicalDevice();

    /**
     * @brief Determines the best supported depth format for the selected GPU.
     *
     * Queries the GPU (`CoreVulkan::physicalDevice`) to find the most suitable format
     * for depth-stencil attachments, in the following preferred order:
     *
     * - `VK_FORMAT_D32_SFLOAT`
     * - `VK_FORMAT_D32_SFLOAT_S8_UINT`
     * - `VK_FORMAT_D24_UNORM_S8_UINT`
     *
     * The first format with `VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT` support in
     * optimal tiling mode is chosen and stored in `CoreVulkan::depthFormat`.
     *
     * This function must be called after selecting the physical device.
     *
     * @throws std::runtime_error if none of the candidate formats are supported.
     */
    static void findDepthFormat();
    /**
     * Creates and initializes the Vulkan instance.
     *
     * Sets up the VkApplicationInfo and VkInstanceCreateInfo structures,
     * enables required extensions (as reported by GLFW), and calls vkCreateInstance.
     *
     * The created instance is stored in CoreVulkan::instance.
     *
     * @throws std::runtime_error if instance creation fails.
     */
    static void createInstance();

    /**
     * @brief Checks if all required validation layers are available.
     *
     * This function enumerates the instance layers available on the system and verifies
     * that all layers listed in `validationLayers` are supported. This is typically used
     * before creating the Vulkan instance to ensure that debug validation layers can be enabled.
     *
     * @return true if all required validation layers are available, false otherwise.
     */
    bool static checkValidationLayerSupport();

    /**
     * @brief Scores a physical device based on its suitability for the application.
     *
     * This function rates the given GPU to determine whether it is a good candidate for use.
     * The scoring is based on several criteria, such as:
     * - Prefer discrete GPUs.
     * - Prefer devices with larger maximum image dimensions.
     * - Reject devices that do not support geometry shaders.
     *
     * Higher scores indicate better suitability.
     *
     * @param physicalDevice The Vulkan physical device (GPU) to evaluate.
     * @return An integer score representing the suitability of the device.
     *         A score of 0 means the device is not suitable.
     */
    int static rateDeviceSuitability(VkPhysicalDevice physicalDevice);
    /**
     * @brief Finds queue families with graphics and presentation support.
     *
     * This function examines the queue families of the given physical device to find one that
     * supports graphics operations (`VK_QUEUE_GRAPHICS_BIT`) and one that supports presenting
     * images to the surface (via `vkGetPhysicalDeviceSurfaceSupportKHR`).
     *
     * @param physicalDevice The physical device to query.
     * @return A `QueueFamilyIndices` struct indicating the indices of the found queue families.
     *         It may be incomplete if the device does not support required features.
     */
    QueueFamilyIndices static findQueueFamilies(VkPhysicalDevice physicalDevice);
    /**
     * @brief Determines whether a physical device is suitable for use.
     *
     * This function calls `findQueueFamilies` and checks whether the necessary queue
     * families (in particular, graphics) are available on the device.
     *
     * @param physicalDevice The physical device to test.
     * @return true if the device meets the minimum requirements, false otherwise.
     */
    bool static isDeviceSuitable(VkPhysicalDevice physicalDevice);
    /**
     * @brief Creates a Vulkan surface from a GLFW window.
     *
     * This function uses `glfwCreateWindowSurface` to bind the Vulkan instance
     * to a window created with GLFW, enabling Vulkan to present rendered images.
     *
     * @param window The GLFW window to bind the surface to.
     *
     * @throws std::runtime_error if the surface creation fails.
     */
    void static createSurface(GLFWwindow* window);

public:
    // Deleting the copy constructor to prevent copies
    CoreVulkan(const CoreVulkan& obj) = delete;

    /**
     * @brief Initializes the Vulkan core objects.
     *
     * This function performs the one-time setup of the Vulkan API for your application:
     * - Creates the Vulkan instance.
     * - Creates the rendering surface from the provided GLFW window.
     * - Selects a suitable physical device (GPU) that supports the required Vulkan features.
     * - Creates a logical device and retrieves the graphics queue.
     * - Queries and caches the optimal depth format supported by the selected GPU.
     *
     * If the Vulkan instance was already initialized (`CoreVulkan::instance != VK_NULL_HANDLE`),
     * this function returns immediately and does nothing.
     *
     * @param window A pointer to the GLFW window used to create the Vulkan surface.
     *
     * @throws std::runtime_error if any of the initialization steps fail
     * (e.g., instance creation, surface creation, device selection, logical device creation, or depth format query).
     */
    static void init(GLFWwindow* window);
    /**
     * Cleans up Vulkan objects in the correct order.
     */
    static void destroy();

    /**
     * @brief Finds a suitable memory type for Vulkan resource allocation.
     *
     * This function searches for a memory type supported by the GPU that satisfies both the
     * type filter and the required property flags (e.g., host-visible, device-local).
     * It is commonly used when allocating memory for buffers or images.
     *
     * @param typeFilter A bitmask indicating the acceptable memory types for the resource.
     * @param properties A combination of `VkMemoryPropertyFlagBits` specifying the desired memory properties.
     *
     * @return The index of a suitable memory type.
     *
     * @throws std::runtime_error if no suitable memory type is found.
     */
    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static VkInstance getInstance() { return instance; }
    static VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
    static VkDevice getDevice() { return device; }
    static VkFormat getDepthFormat() { return depthFormat; }
    static VkQueue getPresentQueue() { return presentQueue; }
    static QueueFamilyIndices getGraphicsQueueFamilyIndices() { return graphicsQueueFamilyIndices; }
    static VkSurfaceKHR getSurface() { return surface; }

};