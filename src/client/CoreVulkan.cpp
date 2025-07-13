#include "CoreVulkan.hpp"

void CoreVulkan::init(uint32_t& graphicsQueueFamily, VkQueue& graphicsQueue)
{
    if (CoreVulkan::instance != nullptr) {
        return;
    }

    // Create Vulkan instance
    CoreVulkan::createInstance();

    // Pick a physical GPU
    CoreVulkan::physicalDevice = VK_NULL_HANDLE;
    CoreVulkan::pickPhysicalDevice();

    // Create logical device
    CoreVulkan::createLogicalDevice(graphicsQueueFamily, graphicsQueue);

    // (Optionally store graphicsQueue and queueFamilyIndex somewhere if needed)

    // find the Depth Format of GPU
    findDepthFormat();
}

void CoreVulkan::destroy()
{
    vkDestroyDevice(CoreVulkan::device, nullptr);
    vkDestroyInstance(CoreVulkan::instance, nullptr);
};

void CoreVulkan::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(CoreVulkan::instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(CoreVulkan::instance, &deviceCount, devices.data());

    // Just pick the first one for now
    CoreVulkan::physicalDevice = devices[0];
}

void CoreVulkan::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ProjectD";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtCount = 0;
    const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
    createInfo.enabledExtensionCount = glfwExtCount;
    createInfo.ppEnabledExtensionNames = glfwExts;

    if (vkCreateInstance(&createInfo, nullptr, &CoreVulkan::instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance\n";
    }
}

void CoreVulkan::createLogicalDevice(uint32_t& graphicsQueueFamily, VkQueue& graphicsQueue) {
    // Find queue family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(CoreVulkan::physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(CoreVulkan::physicalDevice, &queueFamilyCount, queueFamilies.data());

    bool found = false;
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamily = i;
            found = true;
            break;
        }
    }

    if (!found) {
        throw std::runtime_error("failed to find a graphics queue family!");
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsQueueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{}; // enable features if needed

    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (vkCreateDevice(CoreVulkan::physicalDevice, &createInfo, nullptr, &CoreVulkan::device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(CoreVulkan::device, graphicsQueueFamily, 0, &graphicsQueue);
}

void CoreVulkan::findDepthFormat() {
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(CoreVulkan::physicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            CoreVulkan::depthFormat = format;
        }
    }

    throw std::runtime_error("failed to find supported depth format!");
}