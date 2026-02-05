#pragma once

#include "../CoreVulkan.hpp"
#include "SwapchainSupportDetails.hpp"

class SwapchainManager
{
private:
    VkDevice device;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;;
    VkExtent2D swapchainExtent;
    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat;
    std::vector<VkImageView> swapchainImageViews;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&, GLFWwindow*);

    struct ISwapchainConfigProvider {
        virtual void contribute(
            VkSwapchainCreateInfoKHR& info,
            const SwapchainSupportDetails& support
        ) = 0;
    };

    void createSwapchainInternal(
        VkSurfaceKHR surface,
        const QueueFamilyIndices& queueFamilies,
        VkSurfaceFormatKHR surfaceFormat,
        VkPresentModeKHR presentMode,
        VkExtent2D extent,
        const SwapchainSupportDetails& swapChainSupport,
        const std::vector<ISwapchainConfigProvider*>& swapchainProviders,
        VkSwapchainKHR oldSwapchain
    );
    void createImageViews();

public:
    SwapchainManager(
        VkDevice device,
        const QueueFamilyIndices& queueFamilies,
        const SwapchainSupportDetails& swapchainSupportDetails,
        VkSurfaceKHR surface,
        GLFWwindow* window,
        const std::vector<ISwapchainConfigProvider*>& swapchainProviders
    );
    ~SwapchainManager();

    void recreate(
        const QueueFamilyIndices& queueFamilies,
        const SwapchainSupportDetails& swapchainSupportDetails,
        VkSurfaceKHR surface,
        GLFWwindow* window,
        const std::vector<ISwapchainConfigProvider*>& swapchainProviders
    );

    VkSwapchainKHR getSwapchain() const { return this->swapchain; }
    VkFormat getImageFormat() const { return this->swapchainImageFormat; }
    VkExtent2D getExtent() const { return this->swapchainExtent; }
    const std::vector<VkImage>& getImages() const { return this->swapchainImages; }
    const std::vector<VkImageView>& getImageViews() const { return this->swapchainImageViews; }
};