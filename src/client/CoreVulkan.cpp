#include "CoreVulkan.hpp"
#include <set>

CoreVulkan::CoreVulkan(
    GLFWwindow* window,
    const std::vector<IInstanceConfigProvider*>& instanceProviders,
    const std::vector<IPhysicalDeviceSelector*>& PhysicalDeviceSelectors,
    const std::vector<IDeviceConfigProvider*>& DeviceProviders
){
    // test debug mode
    #ifndef NDEBUG
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
    #endif

    // instance and surface
    createInstance(instanceProviders);
    createSurface(window);

    // device extensions
    pickPhysicalDevice(PhysicalDeviceSelectors);
    msaaSamples = findMaxLimitedUsableSampleCount(VK_SAMPLE_COUNT_8_BIT, physicalDevice);
    #ifndef NDEBUG
        std::cout << "Sample Count: " << msaaSamples << std::endl;
    #endif
    graphicsQueueFamilyIndices = findQueueFamilies(physicalDevice);
    if (!graphicsQueueFamilyIndices.isComplete()) {
        throw std::runtime_error("failed to find a graphics queue family!");
    }
    updateSwapchainDetails();
    createLogicalDevice(DeviceProviders);
    vkGetDeviceQueue(device, graphicsQueueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, graphicsQueueFamilyIndices.presentFamily.value(), 0, &presentQueue);
    depthFormat = findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

CoreVulkan::CoreVulkan(CoreVulkan&& other) noexcept
{
    instance = other.instance;
    surface = other.surface;
    graphicsQueueFamilyIndices = std::move(other.graphicsQueueFamilyIndices);
    physicalDevice = other.physicalDevice;
    swapchainSupportDetails = std::move(other.swapchainSupportDetails);
    msaaSamples = other.msaaSamples;
    device = other.device;
    presentQueue = other.presentQueue;
    graphicsQueue = other.graphicsQueue;
    depthFormat = other.depthFormat;

    // deixa o objeto movido em estado seguro
    other.instance = VK_NULL_HANDLE;
    other.surface = VK_NULL_HANDLE;
    other.physicalDevice = VK_NULL_HANDLE;
    other.device = VK_NULL_HANDLE;
    other.presentQueue = VK_NULL_HANDLE;
    other.graphicsQueue = VK_NULL_HANDLE;
    other.msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    other.depthFormat = VK_FORMAT_UNDEFINED;
    other.graphicsQueueFamilyIndices = {};
    other.swapchainSupportDetails = {};
}

CoreVulkan& CoreVulkan::operator=(CoreVulkan&& other) noexcept
{
    if (this != &other)
    {
        cleanup(); //avoid zombie memory

        instance = other.instance;
        surface = other.surface;
        physicalDevice = other.physicalDevice;
        device = other.device;
        presentQueue = other.presentQueue;
        graphicsQueue = other.graphicsQueue;
        msaaSamples = other.msaaSamples;
        depthFormat = other.depthFormat;
        graphicsQueueFamilyIndices = std::move(other.graphicsQueueFamilyIndices);
        swapchainSupportDetails = std::move(other.swapchainSupportDetails);

        other.instance = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.device = VK_NULL_HANDLE;
        other.physicalDevice = VK_NULL_HANDLE;
        other.presentQueue = VK_NULL_HANDLE;
        other.graphicsQueue = VK_NULL_HANDLE;
        other.msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        other.depthFormat = VK_FORMAT_UNDEFINED;
        other.graphicsQueueFamilyIndices =  {};
        other.swapchainSupportDetails = {};
    }
    return *this;
}

bool CoreVulkan::checkValidationLayerSupport()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* requiredLayer : validationLayers) {
        bool found = false;

        for (const VkLayerProperties& layer : availableLayers) {
            if (std::strcmp(requiredLayer, layer.layerName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

void CoreVulkan::createInstance(
    const std::vector<IInstanceConfigProvider*>& providers
) {
    // base config
    InstanceConfig config{};
    config.apiVersion = VK_API_VERSION_1_3;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
        config.extensions.push_back(glfwExtensions[i]);
    }
    #ifndef NDEBUG
        config.layers.insert(
            config.layers.end(),
            validationLayers.begin(),
            validationLayers.end()
        );
    #endif

    // mods
    for (auto* provider : providers) {
        provider->contribute(config);
    }

    // build VkInstanceCreateInfo
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ProjectD";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = config.apiVersion;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(config.extensions.size());
    createInfo.ppEnabledExtensionNames = config.extensions.data();

    createInfo.enabledLayerCount = static_cast<uint32_t>(config.layers.size());
    createInfo.ppEnabledLayerNames = config.layers.data();

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

void CoreVulkan::createSurface(
    GLFWwindow* window
) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
};

SwapchainSupportDetails CoreVulkan::querySwapchainSupport(
    VkPhysicalDevice physicalDevice
) {
    SwapchainSupportDetails swapchainSupportDetails;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupportDetails.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        swapchainSupportDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapchainSupportDetails.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        swapchainSupportDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, swapchainSupportDetails.presentModes.data());
    }
    return swapchainSupportDetails;
}

QueueFamilyIndices CoreVulkan::findQueueFamilies(
    VkPhysicalDevice physicalDevice
) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        const auto& queueFamily = queueFamilies[i];

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            physicalDevice,
            i,
            surface,
            &presentSupport
        );

        if (presentSupport) {
            indices.presentFamily = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (indices.isComplete()) {
            return indices;
        }
    }

    return indices;
}

bool CoreVulkan::isDeviceSuitable(
    VkPhysicalDevice physicalDevice,
    const PhysicalDeviceRequirements& reqs,
    const std::vector<IPhysicalDeviceSelector*>& selectors
) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    if (!indices.isComplete())
        return false;

    // check Device Extension Support
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        nullptr
    );
    std::vector<VkExtensionProperties> available(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        physicalDevice,
        nullptr,
        &extensionCount,
        available.data()
    );

    for (auto* required : reqs.requiredExtensions) {
        bool found = false;
        for (auto& avail : available) {
            if (strcmp(required, avail.extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }

    // features
    VkPhysicalDeviceFeatures supported{};
    vkGetPhysicalDeviceFeatures(physicalDevice, &supported);

    if ((supported.samplerAnisotropy & reqs.requiredFeatures.samplerAnisotropy) != reqs.requiredFeatures.samplerAnisotropy)
        return false;

    if ((supported.geometryShader & reqs.requiredFeatures.geometryShader) != reqs.requiredFeatures.geometryShader)
        return false;

    // swapchain
    SwapchainSupportDetails swapchainSupportDetails = querySwapchainSupport(physicalDevice);
    if (swapchainSupportDetails.formats.empty() || swapchainSupportDetails.presentModes.empty())
        return false;

    // mods
    for (auto* sel : selectors) {
        if (!sel->isDeviceCompatible(physicalDevice, reqs))
            return false;
    }

    return true;
}

int CoreVulkan::rateDeviceSuitability(
    VkPhysicalDevice physicalDevice,
    const PhysicalDeviceRequirements& reqs,
    const std::vector<IPhysicalDeviceSelector*>& selectors
) {
    if (!isDeviceSuitable(physicalDevice, reqs, selectors))
        return 0;

    int score = 0;
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // mods
    for (auto* sel : selectors) {
        sel->scoreDevice(physicalDevice, score);
    }

    return score;
}

VkSampleCountFlagBits CoreVulkan::findMaxLimitedUsableSampleCount(
    VkSampleCountFlagBits maxDesiredSamples,
    VkPhysicalDevice physicalDevice
) {
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(physicalDevice, &props);

    VkSampleCountFlags supported =
        props.limits.framebufferColorSampleCounts &
        props.limits.framebufferDepthSampleCounts &
        maxDesiredSamples;

    if (supported == 0) {
        return VK_SAMPLE_COUNT_1_BIT;
    }

    return static_cast<VkSampleCountFlagBits>(
        1u << (31 - __builtin_clz(supported)) // get the max desired
    );
}

void CoreVulkan::pickPhysicalDevice(
    const std::vector<IPhysicalDeviceSelector*>& selectors
) {
    PhysicalDeviceRequirements reqs{};
    reqs.requiredExtensions = DEVICE_EXTENSIONS;
    reqs.requiredFeatures.samplerAnisotropy = VK_TRUE;
    reqs.requiredFeatures.geometryShader = VK_TRUE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int bestScore = 0;

    for (const auto& device : devices) {
        int score = rateDeviceSuitability(device, reqs, selectors);
        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
        }
    }

    // Check if the best candidate is suitable at all
    if (bestDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    physicalDevice = bestDevice;

    // debug
    #ifndef NDEBUG
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        std::cout << "GPU name: " << deviceProperties.deviceName << std::endl;
    #endif
}

void CoreVulkan::updateSwapchainDetails()
{
    swapchainSupportDetails = querySwapchainSupport(physicalDevice);
};

void CoreVulkan::createLogicalDevice(
    const std::vector<IDeviceConfigProvider*>& providers
) {
    // set queue info
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        graphicsQueueFamilyIndices.graphicsFamily.value(),
        graphicsQueueFamilyIndices.presentFamily.value()
    };
    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // base config
    DeviceConfig config{};
    config.extensions = DEVICE_EXTENSIONS;

    config.requiredFeatures.samplerAnisotropy = VK_TRUE;
    config.optionalFeatures.sampleRateShading = VK_TRUE;

    // mods
    for (auto* p : providers) {
        p->contribute(config);
    }

    // resolve feature suport
    VkPhysicalDeviceFeatures supported{};
    vkGetPhysicalDeviceFeatures(physicalDevice, &supported);
    VkPhysicalDeviceFeatures enabled{};

    auto enableIfSupported = [&](
        VkBool32 required,
        VkBool32 supportedFeature,
        VkBool32& out
    ) {
        if (required && !supportedFeature)
            throw std::runtime_error("Required device feature not supported");
        out = required || supportedFeature;
    };

    enableIfSupported(
        config.requiredFeatures.samplerAnisotropy,
        supported.samplerAnisotropy,
        enabled.samplerAnisotropy
    );
    enableIfSupported(
        config.optionalFeatures.sampleRateShading,
        supported.sampleRateShading,
        enabled.sampleRateShading
    );

    // create info
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &enabled;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(config.extensions.size());
    createInfo.ppEnabledExtensionNames = config.extensions.data();

    // #ifndef NDEBUG
    //     createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    //     createInfo.ppEnabledLayerNames = validationLayers.data();
    // #endif

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }
}

VkFormat CoreVulkan::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

void CoreVulkan::cleanup()
{
    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
    }

    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }

    if (surface != VK_NULL_HANDLE && instance != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}

CoreVulkan::~CoreVulkan(){
    cleanup();
}

uint32_t CoreVulkan::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags required, VkMemoryPropertyFlags preferred) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    uint32_t fallback = UINT32_MAX;

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (!(typeFilter & (1u << i))) continue;

        VkMemoryPropertyFlags flags = memProperties.memoryTypes[i].propertyFlags;

        if ((flags & required) == required) {
            if ((flags & preferred) == preferred) {
                return i;
            }
            if (fallback == UINT32_MAX)
                fallback = i;
        }
    }

    if (fallback != UINT32_MAX) return fallback;
    throw std::runtime_error("no suitable memory type found");
}
