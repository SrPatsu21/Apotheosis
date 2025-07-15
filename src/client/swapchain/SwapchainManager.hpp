#pragma once

#include "../CoreVulkan.hpp"

/**
 * @class SwapchainManager
 * @brief Manages the Vulkan swapchain and its associated resources.
 *
 * This class encapsulates the creation, management, and destruction
 * of a Vulkan swapchain. It handles querying the surface capabilities,
 * choosing the appropriate surface format, present mode, and extent,
 * and creating the swapchain, its images, and their views.
 *
 * It relies on CoreVulkan to provide the active VkDevice and VkPhysicalDevice.
 */
class SwapchainManager
{
private:
    /*
    The Vulkan swapchain handle.
    The swapchain represents the series of images that are presented to the screen.
    It is created for a specific surface and configured with the desired format, extent, and presentation mode.
    */
    VkSwapchainKHR swapchain;

    /*
    The format of the swapchain images.
    This defines the color format (e.g., VK_FORMAT_B8G8R8A8_SRGB) used for rendering and presenting.
    It is selected when creating the swapchain, based on what the surface supports.
    */
    VkFormat swapchainImageFormat;

    /*
    The dimensions (width & height) of the swapchain images, in pixels.
    This usually matches the window's size and defines the resolution you render at.
    */
    VkExtent2D swapchainExtent;

    /*
    The list of Vulkan image handles belonging to the swapchain.
    These images are the actual framebuffers you render to, and which are then presented to the screen.
    */
    std::vector<VkImage> swapchainImages;

    /*
    The list of Vulkan image views for each swapchain image.
    Each image view describes how to access and interpret the corresponding swapchain image.
    These are needed to attach the images to framebuffers and render passes.
    */
    std::vector<VkImageView> swapchainImageViews;
    /**
     * @struct SwapchainSupportDetails
     * @brief Holds the capabilities and supported formats/present modes of a surface.
     *
     * Contains the surface capabilities, supported image formats, and
     * supported present modes, as queried from the physical device.
     */
    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    /**
     * @brief Queries the swapchain support details for the given surface.
     *
     * Retrieves the surface capabilities, supported formats, and present modes.
     *
     * @param surface The Vulkan surface to query.
     * @return SwapchainSupportDetails describing what is supported.
     */
    SwapchainSupportDetails querySwapchainSupport(VkSurfaceKHR);
    /**
     * @brief Chooses the most suitable surface format from the available options.
     *
     * Prefers VK_FORMAT_B8G8R8A8_SRGB with VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
     * falling back to the first available format otherwise.
     *
     * @param formats List of supported formats.
     * @return Chosen VkSurfaceFormatKHR.
     */
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
    /**
     * @brief Chooses the best available present mode.
     *
     * Prefers VK_PRESENT_MODE_MAILBOX_KHR for lower latency,
     * falling back to VK_PRESENT_MODE_FIFO_KHR if unavailable.
     *
     * @param modes List of supported present modes.
     * @return Chosen VkPresentModeKHR.
     */
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>&);
    /**
     * @brief Chooses the swapchain extent (resolution).
     *
     * Uses the current framebuffer size of the given GLFW window.
     *
     * @param capabilities Surface capabilities.
     * @param window The GLFW window handle.
     * @return Chosen VkExtent2D.
     */
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&, GLFWwindow*);
    /**
     * @brief Creates the Vulkan swapchain and retrieves its images.
     *
     * Allocates the swapchain with the specified parameters and
     * stores the handles of its images in swapchainImages.
     *
     * @param surface The Vulkan surface.
     * @param surfaceFormat Chosen surface format.
     * @param presentMode Chosen present mode.
     * @param extent Chosen swapchain extent.
     * @param capabilities Surface capabilities.
     * @param graphicsQueueFamily Index of the graphics queue family.
     */
    void createSwapchainInternal(VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode, VkExtent2D extent, const VkSurfaceCapabilitiesKHR& capabilities, uint32_t graphicsQueueFamily);
    /**
     * @brief Creates image views for all swapchain images.
     *
     * Each image view describes how to access the corresponding swapchain image.
     * The created image views are stored in swapchainImageViews.
     */
    void createImageViews();

public:
    /**
     * @brief Constructs a new SwapchainManager and initializes the swapchain.
     *
     * Queries the surface capabilities, selects a suitable surface format,
     * present mode, and extent, then creates the swapchain and image views.
     *
     * @param surface The Vulkan surface to present to.
     * @param graphicsQueueFamily The index of the graphics queue family.
     * @param window The GLFW window handle, used to determine framebuffer size.
     */
    SwapchainManager(VkSurfaceKHR surface, uint32_t graphicsQueueFamily, GLFWwindow* window);
    /**
     * @brief Destroys the swapchain and associated image views.
     *
     * Cleans up all Vulkan resources allocated by this class.
     * Relies on CoreVulkan to provide the active VkDevice.
     */
    ~SwapchainManager();

    VkSwapchainKHR getSwapchain() const { return this->swapchain; }
    VkFormat getImageFormat() const { return this->swapchainImageFormat; }
    VkExtent2D getExtent() const { return this->swapchainExtent; }
    const std::vector<VkImage>& getImages() const { return this->swapchainImages; }
    const std::vector<VkImageView>& getImageViews() const { return this->swapchainImageViews; }
};