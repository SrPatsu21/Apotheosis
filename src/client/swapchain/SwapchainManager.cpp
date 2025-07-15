#include "SwapchainManager.hpp"

SwapchainManager::SwapchainManager(VkSurfaceKHR surface, uint32_t graphicsQueueFamily, GLFWwindow* window) {
    SwapchainSupportDetails support = querySwapchainSupport(surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
    VkPresentModeKHR presentMode = choosePresentMode(support.presentModes);
    VkExtent2D extent = chooseSwapExtent(support.capabilities, window);

    this->swapchainImageFormat = surfaceFormat.format;
    this->swapchainExtent = extent;

    createSwapchainInternal(surface, surfaceFormat, presentMode, extent, support.capabilities, graphicsQueueFamily);
    createImageViews();
}

SwapchainManager::~SwapchainManager() {
    for (auto view : this->swapchainImageViews) {
        vkDestroyImageView(CoreVulkan::getDevice(), view, nullptr);
    }
    vkDestroySwapchainKHR(CoreVulkan::getDevice(), this->swapchain, nullptr);
}

SwapchainManager::SwapchainSupportDetails SwapchainManager::querySwapchainSupport(VkSurfaceKHR surface) {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(CoreVulkan::getPhysicalDevice(), surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(CoreVulkan::getPhysicalDevice(), surface, &formatCount, nullptr);
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(CoreVulkan::getPhysicalDevice(), surface, &formatCount, details.formats.data());

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(CoreVulkan::getPhysicalDevice(), surface, &presentModeCount, nullptr);
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(CoreVulkan::getPhysicalDevice(), surface, &presentModeCount, details.presentModes.data());

    return details;
}
VkSurfaceFormatKHR SwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }
    return formats[0];
}

VkPresentModeKHR SwapchainManager::choosePresentMode(const std::vector<VkPresentModeKHR>& modes) {
    for (const auto& mode : modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapchainManager::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };
}

void SwapchainManager::createSwapchainInternal(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent, const VkSurfaceCapabilitiesKHR& capabilities, uint32_t graphicsQueueFamily) {
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {graphicsQueueFamily};
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

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

    for (size_t i = 0; i < this->swapchainImages.size(); ++i) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = this->swapchainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = this->swapchainImageFormat;
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