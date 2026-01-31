#pragma once

#include "../CoreVulkan.hpp"
#include "SwapchainSupportDetails.hpp"

class SwapchainManager
{
private:
    VkDevice device;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;;
    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImageView> swapchainImageViews;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&, GLFWwindow*);
    void createSwapchainInternal(
        VkSurfaceKHR surface,
        const QueueFamilyIndices& queueFamilies,
        VkSurfaceFormatKHR surfaceFormat,
        VkPresentModeKHR presentMode,
        VkExtent2D extent,
        const SwapchainSupportDetails& swapChainSupport
    );
    void createImageViews();

public:
    SwapchainManager(
        VkDevice device,
        const QueueFamilyIndices& queueFamilies,
        const SwapchainSupportDetails& swapchainSupportDetails,
        VkSurfaceKHR surface,
        GLFWwindow* window
    );
    ~SwapchainManager();

    void safeDestroySwapchain();
    void recreate(
        const QueueFamilyIndices& queueFamilies,
        const SwapchainSupportDetails& swapchainSupportDetails,
        VkSurfaceKHR surface,
        GLFWwindow* window
    );

    VkSwapchainKHR getSwapchain() const { return this->swapchain; }
    VkFormat getImageFormat() const { return this->swapchainImageFormat; }
    VkExtent2D getExtent() const { return this->swapchainExtent; }
    const std::vector<VkImage>& getImages() const { return this->swapchainImages; }
    const std::vector<VkImageView>& getImageViews() const { return this->swapchainImageViews; }
};