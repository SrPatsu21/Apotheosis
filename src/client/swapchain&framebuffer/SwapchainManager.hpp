#pragma once

#include "../CoreVulkan.hpp"
#include "SwapchainSupportDetails.hpp"

class SwapchainManager
{
private:
        VkSwapchainKHR swapchain;

        std::vector<VkImage> swapchainImages;

        VkFormat swapchainImageFormat;

        VkExtent2D swapchainExtent;

        std::vector<VkImageView> swapchainImageViews;

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&, GLFWwindow*);
        void createSwapchainInternal(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent, const SwapchainSupportDetails& swapChainSupport, uint32_t graphicsQueueFamily);
        void createImageViews();

public:
    SwapchainManager(VkSurfaceKHR surface, uint32_t graphicsQueueFamily, GLFWwindow* window);
    ~SwapchainManager();

    void safeDestroySwapchain();
    void recreate(VkSurfaceKHR surface, GLFWwindow* window, uint32_t graphicsQueueFamily);

    VkSwapchainKHR getSwapchain() const { return this->swapchain; }
    VkFormat getImageFormat() const { return this->swapchainImageFormat; }
    VkExtent2D getExtent() const { return this->swapchainExtent; }
    const std::vector<VkImage>& getImages() const { return this->swapchainImages; }
    const std::vector<VkImageView>& getImageViews() const { return this->swapchainImageViews; }
};