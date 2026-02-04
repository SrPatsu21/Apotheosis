#pragma once

#ifndef GLFW_INCLUDE_VULKAN
    #define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <optional>
#include "./swapchain&framebuffer/SwapchainSupportDetails.hpp"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
#ifdef DISABLE_VALIDATION_LAYERS
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};


class CoreVulkan
{
//* vars
protected:
    VkInstance instance;
    VkSurfaceKHR surface;
    QueueFamilyIndices graphicsQueueFamilyIndices;
    VkPhysicalDevice physicalDevice;
    SwapchainSupportDetails swapchainSupportDetails;
    VkSampleCountFlagBits msaaSamples;
    VkDevice device;
    VkQueue presentQueue;
    VkQueue graphicsQueue;
    VkFormat depthFormat;
    const std::vector<const char*> DEVICE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,  // * Enables swapchain functionality for presenting images to the screen
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, // * Allows binding descriptors with dynamic indexing to improve resource management
        VK_EXT_MEMORY_BUDGET_EXTENSION_NAME, // * Provides information about memory budgets, allowing applications to make better memory usage decisions
        // VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME, // * Allows querying performance-related metrics to help optimize graphics performance
        // VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME, // * Provides a mechanism for high-precision timestamps for better timing and synchronization in applications
        // VK_KHR_MULTIVIEW_EXTENSION_NAME, // * Enables rendering to multiple views within a single pass, useful for VR and stereoscopic rendering
    };


//* behaviors
public:

    // instance
    struct InstanceConfig {
        uint32_t apiVersion = VK_API_VERSION_1_3;
        std::vector<const char*> extensions;
        std::vector<const char*> layers;
    };
    struct IInstanceConfigProvider {
        virtual ~IInstanceConfigProvider() = default;

        virtual void contribute(
            InstanceConfig& config
        ) = 0;
    };
    //physicaldevice
    struct DeviceRequirements {
        std::vector<const char*> requiredExtensions;
        VkPhysicalDeviceFeatures requiredFeatures{};
    };
    struct IDeviceSelector {
        virtual ~IDeviceSelector() = default;

        virtual bool isDeviceCompatible(
            VkPhysicalDevice device,
            const DeviceRequirements& requirements
        ) = 0;

        virtual void scoreDevice(
            VkPhysicalDevice device,
            int& score
        ) = 0;
    };

//* functions
private:
    bool checkValidationLayerSupport();
    void createInstance(
        const std::vector<IInstanceConfigProvider*>& providers
    );
    void createSurface(GLFWwindow* window);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice);
    bool isDeviceSuitable(
        VkPhysicalDevice physicalDevice,
        const DeviceRequirements& reqs,
        const std::vector<IDeviceSelector*>& selectors
    );
    int rateDeviceSuitability(
        VkPhysicalDevice physicalDevice,
        const DeviceRequirements& reqs,
        const std::vector<IDeviceSelector*>& selectors
    );
    VkSampleCountFlagBits findMaxLimitedUsableSampleCount(VkSampleCountFlagBits maxDesiredSamples, VkPhysicalDevice physicalDevice);
    void pickPhysicalDevice(
        const std::vector<IDeviceSelector*>& selectors
    );
    void createLogicalDevice();
    void cleanup();
public:
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void updateSwapchainDetails();

    explicit CoreVulkan(
        GLFWwindow* window,
        const std::vector<IInstanceConfigProvider*>& instanceProviders,
        const std::vector<IDeviceSelector*>& DeviceSelector
    );

    ~CoreVulkan();
    // Deleting the copy constructor and copy assignment to prevent copies
    CoreVulkan(const CoreVulkan& obj) = delete;
    CoreVulkan& operator=(const CoreVulkan& other) = delete;

    // move constructor and move assignment
    CoreVulkan(CoreVulkan&& other) noexcept;
    CoreVulkan& operator=(CoreVulkan&& other) noexcept;

    static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags required, VkMemoryPropertyFlags preferred);

//* get
    const VkInstance& getInstance() const { return instance; }
    const VkSurfaceKHR& getSurface() const { return surface; }
    const QueueFamilyIndices& getGraphicsQueueFamilyIndices() const { return graphicsQueueFamilyIndices; }
    const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
    const SwapchainSupportDetails& getSwapchainSupportDetails() const { return swapchainSupportDetails; }
    const VkSampleCountFlagBits& getMsaaSamples() const { return msaaSamples; }
    const VkDevice& getDevice() const { return device; }
    const VkQueue& getGraphicsQueue() const { return graphicsQueue; }
    const VkQueue& getPresentQueue() const { return presentQueue; }
    const VkFormat& getDepthFormat() const { return depthFormat; }
    const std::vector<const char*>& getDeviceExtensions() const { return DEVICE_EXTENSIONS; }
};