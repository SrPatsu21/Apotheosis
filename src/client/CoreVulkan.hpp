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
    std::vector<const char*> deviceExtensions;

//* functions
private:
    bool checkValidationLayerSupport();
    void createInstance();
    void createSurface(GLFWwindow* window);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice);
    bool isDeviceSuitable(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);
    int rateDeviceSuitability(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);
    VkSampleCountFlagBits findMaxLimitedUsableSampleCount(VkSampleCountFlagBits maxDesiredSamples, VkPhysicalDevice physicalDevice);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void cleanup();
public:
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void updateSwapchainDetails();

    explicit CoreVulkan();

    ~CoreVulkan();
    // Deleting the copy constructor and copy assignment to prevent copies
    CoreVulkan(const CoreVulkan& obj) = delete;
    CoreVulkan& operator=(const CoreVulkan& other) = delete;

    // move constructor and move assignment
    CoreVulkan(CoreVulkan&& other) noexcept;
    CoreVulkan& operator=(CoreVulkan&& other) noexcept;

    static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags required, VkMemoryPropertyFlags preferred);

    virtual void init(GLFWwindow* window);

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
    const std::vector<const char*>& getDeviceExtensions() const { return deviceExtensions; }
};