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

const static std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
#ifdef DISABLE_VALIDATION_LAYERS
        const static bool enableValidationLayers = false;
#else
        const static bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

//TODO VkPhysicalDeviceProperties

class CoreVulkan
{
protected:
    static VkInstance instance;

    static VkPhysicalDevice physicalDevice;

    static VkDevice device;

    static VkFormat depthFormat;

    static VkSurfaceKHR surface;

    static VkQueue presentQueue;

    static VkQueue graphicsQueue;

    static const std::vector<const char*> DEVICE_EXTENSIONS;

    static QueueFamilyIndices graphicsQueueFamilyIndices;

    static VkSampleCountFlagBits msaaSamples;

private:
    //* functions
    static bool checkValidationLayerSupport();

    static void createInstance();

    static void createSurface(GLFWwindow* window);

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);

public:
    static SwapchainSupportDetails querySwapchainSupport(VkSurfaceKHR surface);
private:

    static bool isDeviceSuitable(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);

    static int rateDeviceSuitability(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);

    static VkSampleCountFlagBits getMaxUsableSampleCount(VkSampleCountFlagBits maxDesiredSamples);

    static void pickPhysicalDevice();

    static void createLogicalDevice();


public:

    static void destroy();

    explicit CoreVulkan();
    ~CoreVulkan();

    // Deleting the copy constructor and copy assignment to prevent copies
    CoreVulkan(const CoreVulkan& obj) = delete;
    CoreVulkan& operator=(const CoreVulkan& other) = delete;

    // move constructor and move assignment
    CoreVulkan(CoreVulkan&& other) noexcept;
    CoreVulkan& operator=(CoreVulkan&& other) noexcept;

    //TODO remove form class or make global
    static VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags required, VkMemoryPropertyFlags preferred);

    static void init(GLFWwindow* window);

    static VkInstance getInstance() { return instance; }
    static VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
    static VkDevice getDevice() { return device; }
    static VkFormat getDepthFormat() { return depthFormat; }
    static VkQueue getGraphicsQueue() { return graphicsQueue; }
    static VkQueue getPresentQueue() { return presentQueue; }
    static QueueFamilyIndices getGraphicsQueueFamilyIndices() { return graphicsQueueFamilyIndices; }
    static VkSurfaceKHR getSurface() { return surface; }
    static VkSampleCountFlagBits getMsaaSamples() { return msaaSamples; }

};