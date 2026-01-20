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
private:
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

    CoreVulkan();

    static void createInstance();

public:
    static void init();

    static void createSurface(GLFWwindow* window);

    static void pickPhysicalDevice();

    static void createLogicalDevice();

    static void findDepthFormat();

    static VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    static VkSampleCountFlagBits getMaxUsableSampleCount(VkSampleCountFlagBits maxDesiredSamples);

    static bool hasStencilComponent(VkFormat format);

    static bool checkValidationLayerSupport();

    static int rateDeviceSuitability(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);
    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
    static bool isDeviceSuitable(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);

    // Deleting the copy constructor to prevent copies
    CoreVulkan(const CoreVulkan& obj) = delete;

    static void destroy();

    //* utils

    static SwapchainSupportDetails querySwapchainSupport(VkSurfaceKHR surface);

    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags required, VkMemoryPropertyFlags preferred);

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