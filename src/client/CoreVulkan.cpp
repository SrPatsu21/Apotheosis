#include "CoreVulkan.hpp"
#include <set>

//* pre set vars, need because of static
VkInstance CoreVulkan::instance = VK_NULL_HANDLE;
VkDevice CoreVulkan::device = VK_NULL_HANDLE;
VkPhysicalDevice CoreVulkan::physicalDevice = VK_NULL_HANDLE;
VkFormat CoreVulkan::depthFormat = VK_FORMAT_UNDEFINED;
QueueFamilyIndices CoreVulkan::graphicsQueueFamilyIndices{};
VkQueue CoreVulkan::presentQueue = VK_NULL_HANDLE;
VkSurfaceKHR CoreVulkan::surface = VK_NULL_HANDLE;


void CoreVulkan::init(GLFWwindow* window)
{
    if (CoreVulkan::instance != VK_NULL_HANDLE) {
        return;
    }

    // Create Vulkan instance
    createInstance();

    // create surface
    createSurface(window);

    // Pick a physical GPU
    physicalDevice = VK_NULL_HANDLE;
    pickPhysicalDevice();

    // Create logical device
    createLogicalDevice();

    // find the Depth Format of GPU
    findDepthFormat();
}

void CoreVulkan::destroy()
{
    vkDestroyDevice(CoreVulkan::device, nullptr);
    vkDestroyInstance(CoreVulkan::instance, nullptr);
    vkDestroySurfaceKHR(CoreVulkan::instance, CoreVulkan::surface, nullptr);

};

//TODO make user able to select the device and save this config
void CoreVulkan::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(CoreVulkan::instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(CoreVulkan::instance, &deviceCount, devices.data());

    // Use an ordered map to automatically sort candidates by score
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        CoreVulkan::physicalDevice = candidates.rbegin()->second;

        #ifndef NDEBUG
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(CoreVulkan::physicalDevice, &deviceProperties);
            std::cout << "GPU name: " << deviceProperties.deviceName << std::endl;
        #endif

    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void CoreVulkan::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ProjectD";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan instance\n";
    }

    // Checking for extension support and print
        #ifndef NDEBUG
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

            std::cout << "available extensions:\n";
            for (const auto& extension : extensions) {
                std::cout << extension.extensionName << '\t' << '\t';
            }
            std::cout << std::endl;
        #endif
}

void CoreVulkan::createLogicalDevice() {
    // Find queue family
    CoreVulkan::graphicsQueueFamilyIndices = findQueueFamilies(CoreVulkan::physicalDevice);

    if (!CoreVulkan::graphicsQueueFamilyIndices.isComplete()) {
        throw std::runtime_error("failed to find a graphics queue family!");
    }

    // set queue info
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {CoreVulkan::graphicsQueueFamilyIndices.graphicsFamily.value(), CoreVulkan::graphicsQueueFamilyIndices.presentFamily.value()};

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Device info
    VkPhysicalDeviceFeatures deviceFeatures{}; //* enable features if needed

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        //debug
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(CoreVulkan::physicalDevice, &createInfo, nullptr, &CoreVulkan::device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(CoreVulkan::device, CoreVulkan::graphicsQueueFamilyIndices.graphicsFamily.value(), 0, &CoreVulkan::presentQueue);
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
            return;
        }
    }

    throw std::runtime_error("failed to find supported depth format!");
}

uint32_t CoreVulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(CoreVulkan::physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

bool CoreVulkan::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

int CoreVulkan::rateDeviceSuitability(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    return score;
}

QueueFamilyIndices CoreVulkan::findQueueFamilies(VkPhysicalDevice physicalDevice) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, CoreVulkan::surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool CoreVulkan::isDeviceSuitable(VkPhysicalDevice physicalDevice) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    return indices.graphicsFamily.has_value();
}

void CoreVulkan::createSurface(GLFWwindow* window)
{
    if (glfwCreateWindowSurface(CoreVulkan::getInstance(), window, nullptr, &CoreVulkan::surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
};