#include "SwapchainManager.hpp"
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp

SwapchainManager::SwapchainManager(VkSurfaceKHR surface, uint32_t graphicsQueueFamily, GLFWwindow* window) {
    SwapchainSupportDetails support = CoreVulkan::querySwapchainSupport(surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
    VkExtent2D extent = chooseSwapExtent(support.capabilities, window);

    this->swapchainImageFormat = surfaceFormat.format;
    this->swapchainExtent = extent;

    createSwapchainInternal(surface, surfaceFormat, presentMode, extent, support, graphicsQueueFamily);
    createImageViews();
}

SwapchainManager::~SwapchainManager() {
    for (auto view : this->swapchainImageViews) {
        vkDestroyImageView(CoreVulkan::getDevice(), view, nullptr);
    }
    vkDestroySwapchainKHR(CoreVulkan::getDevice(), this->swapchain, nullptr);
}

VkSurfaceFormatKHR SwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR SwapchainManager::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapchainManager::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void SwapchainManager::createSwapchainInternal(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent, const SwapchainSupportDetails& swapChainSupport, uint32_t graphicsQueueFamily) {
    // how many images we would like to have in the swap chain
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // swapChainSupport
    uint32_t queueFamilyIndices[] = {CoreVulkan::getGraphicsQueueFamilyIndices().graphicsFamily.value(), CoreVulkan::getGraphicsQueueFamilyIndices().presentFamily.value()};
    if (CoreVulkan::getGraphicsQueueFamilyIndices().graphicsFamily != CoreVulkan::getGraphicsQueueFamilyIndices().presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE; //TODO remake the swapchain

    if (vkCreateSwapchainKHR(CoreVulkan::getDevice(), &createInfo, nullptr, &this->swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    uint32_t count;
    vkGetSwapchainImagesKHR(CoreVulkan::getDevice(), this->swapchain, &count, nullptr);
    this->swapchainImages.resize(count);
    vkGetSwapchainImagesKHR(CoreVulkan::getDevice(), this->swapchain, &count, this->swapchainImages.data());
}


void SwapchainManager::createImageViews() {
    this->swapchainImageViews.resize(this->swapchainImages.size());

    for (size_t i = 0; i < this->swapchainImages.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = this->swapchainImages[i];
        //TODO 3D?
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = this->swapchainImageFormat;

        /*
        The components field allows you to swizzle the color channels around.
        For example, you can map all of the channels to the red channel for a monochrome texture.
        You can also map constant values of 0 and 1 to a channel.
        */
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        /*
        The subresourceRange field describes what the image's purpose is and which part of the image should be accessed
        */
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(CoreVulkan::getDevice(), &viewInfo, nullptr, &this->swapchainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view!");
        }
    }
}

void SwapchainManager::safeDestroySwapchain(){
    if (!this->swapchainImageViews.empty()) {
        for (auto view : swapchainImageViews) {
            vkDestroyImageView(CoreVulkan::getDevice(), view, nullptr);
        }
        swapchainImageViews.clear();
    }
    if (this->swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(CoreVulkan::getDevice(), this->swapchain, nullptr);
        this->swapchain = VK_NULL_HANDLE; // reset
    }
}

void SwapchainManager::recreate(VkSurfaceKHR surface, GLFWwindow* window, uint32_t graphicsQueueFamily)
{
    // details
    SwapchainSupportDetails swapChainSupport = CoreVulkan::querySwapchainSupport(surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

    // create a new one
    createSwapchainInternal(surface, surfaceFormat, presentMode, extent, swapChainSupport, graphicsQueueFamily);

    // recreate imageViews
    createImageViews();
}